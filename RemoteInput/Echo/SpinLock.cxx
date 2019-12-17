//
//  SpinLock.cxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-15.
//  Copyright © 2019 XIO. All rights reserved.
//

#include "SpinLock.hxx"
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

bool spinlock_timedlock(std::atomic_flag* lock, const struct timespec* timeout)
{
    bool retcode = false;
	struct timespec sleeptime;
	sleeptime.tv_sec = 0;
	sleeptime.tv_nsec = 1000000; //1ms
	sleeptime.tv_nsec *= 5; //5ms

    while (!(retcode = lock->test_and_set(std::memory_order_acquire)))
    {
        //clock_gettime(CLOCK_REALTIME, &now); gettimeofday(&now, nullptr); timespec_get(&now, UTC_TIME)
		auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
		auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

		struct timespec now = { seconds.count(), nanoseconds.count() };
		if (now.tv_sec >= timeout->tv_sec && now.tv_nsec >= timeout->tv_nsec)
		{
			errno = EAGAIN;
			return 0;
		}

		nanosleep (&sleeptime, nullptr);
    }

    return retcode;
}

template<typename T>
T* spinlock_cast(void* &ptr)
{
	std::uint8_t* it = static_cast<std::uint8_t*>(ptr);
	T* result = static_cast<T*>(ptr);
	ptr = (it + sizeof(T));
	return result;
}

SpinLock::SpinLock() : shared(false), flag(nullptr), ref(nullptr), name("\0"), shm_fd(-1)
{
	flag = new std::atomic_flag(false);
}

SpinLock::SpinLock(std::string name) : shared(true), flag(nullptr), ref(nullptr), name(name), shm_fd(-1)
{
	bool created = true;
	shm_fd = shm_open(name.c_str(), O_CREAT | O_EXCL | O_RDWR, S_IRWXU); //0660 -> S_IRWXU
	if (shm_fd == -1)
	{
		if (errno == EEXIST)
		{
			created = false;
			shm_fd = shm_open(name.c_str(), O_RDWR, S_IRWXU);
		}
	}

	if (shm_fd == -1)
	{
		perror(strerror(errno));
		throw std::runtime_error("Cannot create or open memory map");
	}

	if (created && ftruncate(shm_fd, sizeof(*this)) == -1)
	{
		shm_unlink(name.c_str());
		shm_fd = -1;
		throw std::runtime_error("Cannot truncate memory map");
	}

	void* mapping = mmap(nullptr, sizeof(*this), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (mapping == MAP_FAILED)
	{
		if (created)
		{
			shm_unlink(name.c_str());
		}
		shm_fd = -1;
		throw std::runtime_error("Mapping into process memory failed");
	}

	void* data = mapping;
	std::atomic_flag* flag = spinlock_cast<std::atomic_flag>(data);
	std::int32_t* ref = spinlock_cast<std::int32_t>(data);

	if (created)
	{
		new(flag)(std::atomic_flag);
	}

	*ref += 1;
	this->flag = flag;
	this->ref = ref;
}

SpinLock::~SpinLock()
{
	if (flag && shared)
    {
        void* data = static_cast<void*>(flag);
        std::atomic_flag* flag = spinlock_cast<std::atomic_flag>(data);
        std::int32_t* ref = spinlock_cast<std::int32_t>(data);
        *ref -= 1;

        if (*ref == 0)
        {
			flag->~atomic_flag();
            shm_unlink(name.c_str());
        }
        munmap(flag, sizeof(*this));
    }

	if (flag && !shared)
	{
		delete flag;
	}

    if (shm_fd != -1)
    {
        close(shm_fd);
    }

    flag = nullptr;
    ref = nullptr;
    shm_fd = -1;
}

bool SpinLock::lock()
{
	while (flag->test_and_set(std::memory_order_acquire)) { ; }
	return true;
}

bool SpinLock::try_lock()
{
	return flag->test_and_set(std::memory_order_acquire);
}

bool SpinLock::timed_lock(std::uint32_t milliseconds)
{
	if(!milliseconds)
    {
        return lock();
    }
	
	auto duration = (std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(milliseconds)).time_since_epoch();
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

	struct timespec ts = { seconds.count(), nanoseconds.count() };
	return spinlock_timedlock(flag, &ts) == 1;
}

bool SpinLock::unlock()
{
	flag->clear(std::memory_order_release);
	return true;
}


template<typename Rep, typename Period>
bool SpinLock::try_lock_for(const std::chrono::duration<Rep, Period>& relative_time)
{
	std::chrono::steady_clock::duration rtime = std::chrono::duration_cast<std::chrono::steady_clock::duration>(relative_time);
    if(std::ratio_greater<std::chrono::steady_clock::period, Period>())
    {
        ++rtime;
    }
    return try_lock_until(std::chrono::steady_clock::now() + rtime);
}

template<typename Duration>
bool SpinLock::try_lock_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time)
{
	std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::seconds> sec = std::chrono::time_point_cast<std::chrono::seconds>(absolute_time);
    std::chrono::nanoseconds nano = std::chrono::duration_cast<std::chrono::nanoseconds>(absolute_time - sec);

	struct timespec ts = { sec.time_since_epoch().count(), nano.count() };
	return spinlock_timedlock(flag, &ts) == 1;
}

template<typename Clock, typename Duration>
bool SpinLock::try_lock_until(const std::chrono::time_point<Clock, Duration>& absolute_time)
{
	return try_wait_until(std::chrono::high_resolution_clock::now() + (absolute_time - Clock::now()));
}


void spinning_semaphore_sleep(int* count)
{
	if (*count++ >= 500)
	{
		struct timespec sleeptime;
		sleeptime.tv_sec = 0;
		sleeptime.tv_nsec = 1000000; //1ms
		sleeptime.tv_nsec *= 5; //5ms

		nanosleep (&sleeptime, nullptr);
		count = 0;
	}
	else
	{
		std::this_thread::yield();
	}
}

bool spinning_semaphore_timedlock(std::atomic_flag* lock, std::atomic_int32_t* count, const struct timespec* timeout)
{
	struct timespec sleeptime;
	sleeptime.tv_sec = 0;
	sleeptime.tv_nsec = 1000000; //1ms
	sleeptime.tv_nsec *= 5; //5ms

    while (!lock->test_and_set(std::memory_order_acquire))
    {
		auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
		auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

		struct timespec now = { seconds.count(), nanoseconds.count() };
		if (now.tv_sec >= timeout->tv_sec && now.tv_nsec >= timeout->tv_nsec)
		{
			errno = EAGAIN;
			return 0;
		}

		nanosleep (&sleeptime, nullptr);
    }
	
	while (!(count->load(std::memory_order_relaxed) <= 0))
	{
		auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
		auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

		struct timespec now = { seconds.count(), nanoseconds.count() };
		if (now.tv_sec >= timeout->tv_sec && now.tv_nsec >= timeout->tv_nsec)
		{
			lock->clear(std::memory_order_release);
			errno = EAGAIN;
			return 0;
		}

		nanosleep (&sleeptime, nullptr);
	}
	
	count->fetch_sub(1, std::memory_order_acq_rel);
	lock->clear(std::memory_order_release);
    return true;
}

template<typename T>
T* spinning_semaphore_cast(void* &ptr)
{
	std::uint8_t* it = static_cast<std::uint8_t*>(ptr);
	T* result = static_cast<T*>(ptr);
	ptr = (it + sizeof(T));
	return result;
}

SpinningSemaphore::SpinningSemaphore(std::int32_t count) : shared(false), lock(new std::atomic_flag(false)), count(new std::atomic_int32_t(count)), ref(nullptr), name("\0"), shm_fd(-1)
{
}

SpinningSemaphore::SpinningSemaphore(std::string name, std::int32_t count) : shared(true), lock(nullptr), count(nullptr), ref(nullptr), name(name), shm_fd(-1)
{
	bool created = true;
	shm_fd = shm_open(name.c_str(), O_CREAT | O_EXCL | O_RDWR, S_IRWXU); //0660 -> S_IRWXU
	if (shm_fd == -1)
	{
		if (errno == EEXIST)
		{
			created = false;
			shm_fd = shm_open(name.c_str(), O_RDWR, S_IRWXU);
		}
	}

	if (shm_fd == -1)
	{
		perror(strerror(errno));
		throw std::runtime_error("Cannot create or open memory map");
	}

	if (created && ftruncate(shm_fd, sizeof(*this)) == -1)
	{
		shm_unlink(name.c_str());
		shm_fd = -1;
		throw std::runtime_error("Cannot truncate memory map");
	}

	void* mapping = mmap(nullptr, sizeof(*this), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (mapping == MAP_FAILED)
	{
		if (created)
		{
			shm_unlink(name.c_str());
		}
		shm_fd = -1;
		throw std::runtime_error("Mapping into process memory failed");
	}

	void* data = mapping;
	std::atomic_flag* lock = spinning_semaphore_cast<std::atomic_flag>(data);
	std::atomic_int32_t* counter = spinning_semaphore_cast<std::atomic_int32_t>(data);
	std::int32_t* ref = spinning_semaphore_cast<std::int32_t>(data);

	if (created)
	{
		new(lock) std::atomic_flag(false);
		new(counter) std::atomic_int32_t(count);
	}

	*ref += 1;
	this->lock = lock;
	this->count = counter;
	this->ref = ref;
}

SpinningSemaphore::~SpinningSemaphore()
{
	if (lock && count && shared)
    {
        void* data = static_cast<void*>(lock);
        std::atomic_flag* lock = spinning_semaphore_cast<std::atomic_flag>(data);
		std::atomic_int32_t* count = spinning_semaphore_cast<std::atomic_int32_t>(data);
        std::int32_t* ref = spinning_semaphore_cast<std::int32_t>(data);
        *ref -= 1;

        if (*ref == 0)
        {
			count->~atomic();
			lock->~atomic_flag();
            shm_unlink(name.c_str());
        }
        munmap(lock, sizeof(*this));
    }

	if (lock && count && !shared)
	{
		delete count;
		delete lock;
	}

    if (shm_fd != -1)
    {
        close(shm_fd);
    }

	count = nullptr;
    lock = nullptr;
    ref = nullptr;
    shm_fd = -1;
}

bool SpinningSemaphore::wait()
{
	int cnt = 0;
	while (lock->test_and_set(std::memory_order_acquire)) { spinning_semaphore_sleep(&cnt); }
	
	cnt = 0;
	while (count->load(std::memory_order_relaxed) <= 0) { spinning_semaphore_sleep(&cnt); }
	
	count->fetch_sub(1, std::memory_order_acq_rel);
	lock->clear(std::memory_order_release);
	return true;
}

bool SpinningSemaphore::try_wait()
{
	if (lock->test_and_set(std::memory_order_acquire))
	{
		if (count->load(std::memory_order_relaxed) <= 0)
		{
			count->fetch_sub(1, std::memory_order_acq_rel);
			return true;
		}
		lock->clear(std::memory_order_release);
	}
	return false;
}

bool SpinningSemaphore::timed_wait(std::uint32_t milliseconds)
{
	if(!milliseconds)
    {
        return wait();
    }
	
	auto duration = (std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(milliseconds)).time_since_epoch();
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

	struct timespec ts = { seconds.count(), nanoseconds.count() };
	return spinning_semaphore_timedlock(lock, count, &ts) == 1;
}

bool SpinningSemaphore::signal()
{
	count->fetch_add(1, std::memory_order_release);
	return true;
}


template<typename Rep, typename Period>
bool SpinningSemaphore::try_wait_for(const std::chrono::duration<Rep, Period>& relative_time)
{
	std::chrono::steady_clock::duration rtime = std::chrono::duration_cast<std::chrono::steady_clock::duration>(relative_time);
    if(std::ratio_greater<std::chrono::steady_clock::period, Period>())
    {
        ++rtime;
    }
    return try_wait_until(std::chrono::steady_clock::now() + rtime);
}

template<typename Duration>
bool SpinningSemaphore::try_wait_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time)
{
	std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::seconds> sec = std::chrono::time_point_cast<std::chrono::seconds>(absolute_time);
    std::chrono::nanoseconds nano = std::chrono::duration_cast<std::chrono::nanoseconds>(absolute_time - sec);

	struct timespec ts = { sec.time_since_epoch().count(), nano.count() };
	return spinning_semaphore_timedlock(lock, count, &ts) == 1;
}

template<typename Clock, typename Duration>
bool SpinningSemaphore::try_wait_until(const std::chrono::time_point<Clock, Duration>& absolute_time)
{
	return try_wait_until(std::chrono::high_resolution_clock::now() + (absolute_time - Clock::now()));
}








void spinning_signal_sleep(int* count)
{
	if (*count++ >= 500)
	{
		auto start = std::chrono::high_resolution_clock::now();
		auto end = start + std::chrono::nanoseconds(1);
		do {
			std::this_thread::yield();
		} while (std::chrono::high_resolution_clock::now() < end);
		*count = 0;
	}
	else
	{
		std::this_thread::yield();
	}
}

bool spinning_signal_timedlock(std::atomic_bool* lock, const struct timespec* timeout)
{
	struct timespec sleeptime;
	sleeptime.tv_sec = 0;
	sleeptime.tv_nsec = 1000000; //1ms
	sleeptime.tv_nsec *= 5; //5ms

    while (lock->load(std::memory_order_acquire) != true)
    {
		auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
		auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

		struct timespec now = { seconds.count(), nanoseconds.count() };
		if (now.tv_sec >= timeout->tv_sec && now.tv_nsec >= timeout->tv_nsec)
		{
			errno = EAGAIN;
			return false;
		}

		nanosleep (&sleeptime, nullptr);
    }
	return true;
}

template<typename T>
T* spinning_variable_cast(void* &ptr)
{
	std::uint8_t* it = static_cast<std::uint8_t*>(ptr);
	T* result = static_cast<T*>(ptr);
	ptr = (it + sizeof(T));
	return result;
}

SpinningSignal::SpinningSignal(std::int32_t count) : shared(false), lock(new std::atomic_bool(false)), ref(nullptr), name("\0"), shm_fd(-1)
{
	static_assert(ATOMIC_BOOL_LOCK_FREE == 2, "Atomic Bool is NOT Lock-Free");
}

SpinningSignal::SpinningSignal(std::string name, std::int32_t count) : shared(true), lock(nullptr), ref(nullptr), name(name), shm_fd(-1)
{
	static_assert(ATOMIC_BOOL_LOCK_FREE == 2, "Atomic Bool is NOT Lock-Free");
	
	bool created = true;
	shm_fd = shm_open(name.c_str(), O_CREAT | O_EXCL | O_RDWR, S_IRWXU); //0660 -> S_IRWXU
	if (shm_fd == -1)
	{
		if (errno == EEXIST)
		{
			created = false;
			shm_fd = shm_open(name.c_str(), O_RDWR, S_IRWXU);
		}
	}

	if (shm_fd == -1)
	{
		perror(strerror(errno));
		throw std::runtime_error("Cannot create or open memory map");
	}

	if (created && ftruncate(shm_fd, sizeof(*this)) == -1)
	{
		shm_unlink(name.c_str());
		shm_fd = -1;
		throw std::runtime_error("Cannot truncate memory map");
	}

	void* mapping = mmap(nullptr, sizeof(*this), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (mapping == MAP_FAILED)
	{
		if (created)
		{
			shm_unlink(name.c_str());
		}
		shm_fd = -1;
		throw std::runtime_error("Mapping into process memory failed");
	}

	void* data = mapping;
	std::atomic_bool* lock = spinning_variable_cast<std::atomic_bool>(data);
	std::int32_t* ref = spinning_variable_cast<std::int32_t>(data);

	if (created)
	{
		new(lock) std::atomic_bool(false);
	}

	*ref += 1;
	this->lock = lock;
	this->ref = ref;
	msync(ref, sizeof(*ref), MS_SYNC);
}

SpinningSignal::~SpinningSignal()
{
	if (lock && shared)
    {
        void* data = static_cast<void*>(lock);
        std::atomic_bool* lock = spinning_variable_cast<std::atomic_bool>(data);
        std::int32_t* ref = spinning_variable_cast<std::int32_t>(data);
        *ref -= 1;
		msync(ref, sizeof(*ref), MS_SYNC);

        if (*ref == 0)
        {
			lock->~atomic();
            shm_unlink(name.c_str());
        }
        munmap(lock, sizeof(*this));
    }

	if (lock && !shared)
	{
		delete lock;
	}

    if (shm_fd != -1)
    {
        close(shm_fd);
    }

    lock = nullptr;
    ref = nullptr;
    shm_fd = -1;
}

bool SpinningSignal::wait()
{
	static int cnt = 0;
	while (!lock->load(std::memory_order_acquire)) { spinning_signal_sleep(&cnt); }
	lock->store(false, std::memory_order_release);
	return true;
}

bool SpinningSignal::try_wait()
{
	if (!lock->load(std::memory_order_acquire))
	{
		lock->store(false, std::memory_order_release);
		return true;
	}
	return false;
}

bool SpinningSignal::timed_wait(std::uint32_t milliseconds)
{
	if(!milliseconds)
    {
        return wait();
    }
	
	auto duration = (std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(milliseconds)).time_since_epoch();
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

	struct timespec ts = { seconds.count(), nanoseconds.count() };
	return spinning_signal_timedlock(lock, &ts);
}

bool SpinningSignal::signal()
{
	lock->store(true, std::memory_order_release);
	return true;
}


template<typename Rep, typename Period>
bool SpinningSignal::try_wait_for(const std::chrono::duration<Rep, Period>& relative_time)
{
	std::chrono::steady_clock::duration rtime = std::chrono::duration_cast<std::chrono::steady_clock::duration>(relative_time);
    if(std::ratio_greater<std::chrono::steady_clock::period, Period>())
    {
        ++rtime;
    }
    return try_wait_until(std::chrono::steady_clock::now() + rtime);
}

template<typename Duration>
bool SpinningSignal::try_wait_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time)
{
	std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::seconds> sec = std::chrono::time_point_cast<std::chrono::seconds>(absolute_time);
    std::chrono::nanoseconds nano = std::chrono::duration_cast<std::chrono::nanoseconds>(absolute_time - sec);

	struct timespec ts = { sec.time_since_epoch().count(), nano.count() };
	return spinning_signal_timedlock(lock, &ts);
}

template<typename Clock, typename Duration>
bool SpinningSignal::try_wait_until(const std::chrono::time_point<Clock, Duration>& absolute_time)
{
	return try_wait_until(std::chrono::high_resolution_clock::now() + (absolute_time - Clock::now()));
}
