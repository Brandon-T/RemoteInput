//
//  SharedEvent.cxx
//  RemoteInput
//
//  Created by Brandon on 2019-09-14.
//  Copyright © 2019 XIO. All rights reserved.
//

#include "SharedEvent.hxx"
#include <tuple>

#if !defined(_WIN32) && !defined(_WIN64)
int pthread_mutex_timedlock(pthread_mutex_t* mutex, const struct timespec* timeout)
{
    int retcode = 0;
	struct timespec sleeptime;
	sleeptime.tv_sec = 0;
	sleeptime.tv_nsec = 1000000; //1ms
	sleeptime.tv_nsec *= 5; //5ms

    while ((retcode = pthread_mutex_trylock (mutex)) == EBUSY)
    {
        //clock_gettime(CLOCK_REALTIME, &now); gettimeofday(&now, nullptr); timespec_get(&now, UTC_TIME)
		auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
		auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

		struct timespec now = { seconds.count(), nanoseconds.count() };
		if (now.tv_sec >= timeout->tv_sec && now.tv_nsec >= timeout->tv_nsec)
		{
			errno = EAGAIN;
			return -1;
		}

		nanosleep (&sleeptime, nullptr);
    }

    return retcode;
}
#endif


#if defined (_WIN32) || defined(_WIN64)
Mutex::Mutex() : hMutex(INVALID_HANDLE_VALUE)
{
    hMutex = CreateMutex(0, false, nullptr);
}

Mutex::Mutex(std::string name) : hMutex(INVALID_HANDLE_VALUE)
{
    hMutex = OpenMutex(0, false, name.c_str());
    if (!hMutex)
    {
        hMutex = CreateMutex(0, false, name.c_str());
    }
}

Mutex::~Mutex()
{
    //ReleaseMutex(hMutex);
    CloseHandle(hMutex);
}
#else
template<typename T>
T* Mutex::mutex_cast(void* &ptr)
{
	std::uint8_t* it = static_cast<std::uint8_t*>(ptr);
	T* result = static_cast<T*>(ptr);
	ptr = (it + sizeof(T));
	return result;
}

Mutex::Mutex() : shared(false), mutex(nullptr), ref(nullptr), name("\0"), shm_fd(-1)
{
    pthread_mutexattr_t attributes;
    if (pthread_mutexattr_init(&attributes))
    {
        return;
    }

    if (pthread_mutexattr_setpshared(&attributes, PTHREAD_PROCESS_PRIVATE))
    {
        pthread_mutexattr_destroy(&attributes);
        return;
    }

	mutex = new pthread_mutex_t();
    if (pthread_mutex_init(mutex, &attributes))
    {
        pthread_mutexattr_destroy(&attributes);
		delete mutex;
        return;
    }

    pthread_mutexattr_destroy(&attributes);
}

Mutex::Mutex(std::string name) : shared(true), mutex(nullptr), ref(nullptr), name(name), shm_fd(-1)
{
    bool created = false;
	shm_fd = shm_open(name.c_str(), O_RDWR, S_IRWXU); //0660 -> S_IRWXU
	if (errno == ENOENT)
	{
		shm_fd = shm_open(name.c_str(), O_CREAT | O_RDWR, S_IRWXU);
		created = true;
	}

	if (shm_fd == -1)
	{
		return;
	}

	if (ftruncate(shm_fd, sizeof(*this)))
	{
		if (created)
		{
			shm_unlink(name.c_str());
		}
		shm_fd = -1;
		return;
	}

	void* mapping = mmap(nullptr, sizeof(*this), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (mapping == MAP_FAILED)
	{
		if (created)
		{
			shm_unlink(name.c_str());
		}
		shm_fd = -1;
		return;
	}

	void* data = mapping;
	pthread_mutex_t* mutex = mutex_cast<pthread_mutex_t>(data);
	std::int32_t* ref = mutex_cast<std::int32_t>(data);

	if (created)
	{
		pthread_mutexattr_t attributes;
		if (pthread_mutexattr_init(&attributes))
		{
			shm_unlink(name.c_str());
			shm_fd = -1;
			return;
		}

		if (pthread_mutexattr_setpshared(&attributes, PTHREAD_PROCESS_SHARED))
		{
			pthread_mutexattr_destroy(&attributes);
			shm_unlink(name.c_str());
			shm_fd = -1;
			return;
		}

		if (pthread_mutex_init(mutex, &attributes))
		{
			pthread_mutexattr_destroy(&attributes);
			shm_unlink(name.c_str());
			shm_fd = -1;
			return;
		}

		pthread_mutexattr_destroy(&attributes);
	}

	*ref += 1;
	this->mutex = mutex;
	this->ref = ref;
}

Mutex::~Mutex()
{
    if (mutex && shared)
    {
        void* data = static_cast<void*>(mutex);
        pthread_mutex_t* mutex = mutex_cast<pthread_mutex_t>(data);
        std::int32_t* ref = mutex_cast<std::int32_t>(data);
        *ref -= 1;

        if (*ref == 0)
        {
            pthread_mutex_destroy(mutex);
            shm_unlink(name.c_str());
        }
        munmap(mutex, sizeof(*this));
    }

	if (mutex && !shared)
	{
		pthread_mutex_destroy(mutex);
		delete mutex;
	}

    if (shm_fd != -1)
    {
        close(shm_fd);
    }

    mutex = nullptr;
    ref = nullptr;
    shm_fd = -1;
}
#endif

bool Mutex::lock()
{
    #if defined(_WIN32) || defined(_WIN64)
    return WaitForSingleObject(hMutex, INFINITE) == WAIT_OBJECT_0;
    #else
    return mutex && !pthread_mutex_lock(mutex);
    #endif
}

bool Mutex::try_lock()
{
    #if defined(_WIN32) || defined(_WIN64)
    return WaitForSingleObject(hMutex, 0) == WAIT_OBJECT_0;
    #else
    return mutex && !pthread_mutex_trylock(mutex);
    #endif
}


template<typename Rep, typename Period>
bool Mutex::try_lock_for(const std::chrono::duration<Rep, Period>& relative_time)
{
    std::chrono::steady_clock::duration rtime = std::chrono::duration_cast<std::chrono::steady_clock::duration>(relative_time);
    if(std::ratio_greater<std::chrono::steady_clock::period, Period>())
    {
        ++rtime;
    }
    return try_lock_until(std::chrono::steady_clock::now() + rtime);
}

template<typename Duration>
bool Mutex::try_lock_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time)
{
    #if defined (_WIN32) || defined(_WIN64)
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::milliseconds> msec = std::chrono::time_point_cast<std::chrono::milliseconds>(absolute_time);
    return WaitForSingleObject(hMutex, msec.time_since_epoch().count()) == WAIT_OBJECT_0;
    #else
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::seconds> sec = std::chrono::time_point_cast<std::chrono::seconds>(absolute_time);
    std::chrono::nanoseconds nano = std::chrono::duration_cast<std::chrono::nanoseconds>(absolute_time - sec);

	struct timespec ts = { sec.time_since_epoch().count(), nano.count() };
	return mutex && !pthread_mutex_timedlock(mutex, &ts);
    #endif
}

template<typename Clock, typename Duration>
bool Mutex::try_lock_until(const std::chrono::time_point<Clock, Duration>& absolute_time)
{
    return try_lock_until(std::chrono::high_resolution_clock::now() + (absolute_time - Clock::now()));
}

bool Mutex::timed_lock(std::uint32_t milliseconds)
{
    if(!milliseconds)
    {
        return lock();
    }

    #if defined(_WIN32) || defined(_WIN64)
    return WaitForSingleObject(hMutex, milliseconds) == WAIT_OBJECT_0;
    #else
    auto duration = (std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(milliseconds)).time_since_epoch();
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

	struct timespec ts = { seconds.count(), nanoseconds.count() };
	return mutex && !pthread_mutex_timedlock(mutex, &ts);
    #endif
}

bool Mutex::unlock()
{
    #if defined(_WIN32) || defined(_WIN64)
    return ReleaseMutex(hMutex);
    #else
    return mutex && !pthread_mutex_unlock(mutex);
    #endif
}

#if defined(_WIN32) || defined(_WIN64)
Semaphore::Semaphore(std::int32_t count)
{
    hSemaphore = OpenSemaphore(0, false, nullptr);
    if (!hSemaphore)
    {
        hSemaphore = CreateSemaphore(nullptr, count, count + 1, nullptr);
    }
}

Semaphore::Semaphore(std::string name, std::int32_t count)
{
    hSemaphore = OpenSemaphore(0, false, name.c_str());
    if (!hSemaphore)
    {
        hSemaphore = CreateSemaphore(nullptr, count, count + 1, name.c_str());
    }
}

Semaphore::~Semaphore()
{
    //ReleaseSemaphore(hSemaphore, 1, nullptr);
    CloseHandle(hSemaphore);
}
#else
template<typename T>
T* Semaphore::semaphore_cast(void* &ptr)
{
	std::uint8_t* it = static_cast<std::uint8_t*>(ptr);
	T* result = static_cast<T*>(ptr);
	ptr = (it + sizeof(T));
	return result;
}

Semaphore::Semaphore(std::int32_t count) : shared(false), mutex(nullptr), condition(nullptr), sem_count(nullptr), ref(nullptr), name("\0"), shm_fd(-1)
{
	pthread_mutexattr_t mutex_attributes;
	if (pthread_mutexattr_init(&mutex_attributes))
	{
		return;
	}

	if (pthread_mutexattr_setpshared(&mutex_attributes, PTHREAD_PROCESS_PRIVATE))
	{
		pthread_mutexattr_destroy(&mutex_attributes);
		return;
	}

	mutex = new pthread_mutex_t();
	if (pthread_mutex_init(mutex, &mutex_attributes))
	{
		pthread_mutexattr_destroy(&mutex_attributes);
		delete mutex;
		return;
	}

	pthread_condattr_t condition_attributes;
	if (pthread_condattr_init(&condition_attributes))
	{
		pthread_mutexattr_destroy(&mutex_attributes);
		pthread_mutex_destroy(mutex);
		delete mutex;
		return;
	}

	if (pthread_condattr_setpshared(&condition_attributes, PTHREAD_PROCESS_PRIVATE))
	{
		pthread_mutexattr_destroy(&mutex_attributes);
		pthread_mutex_destroy(mutex);
		pthread_condattr_destroy(&condition_attributes);
		delete mutex;
		return;
	}

	condition = new pthread_cond_t();
	if (pthread_cond_init(condition, &condition_attributes))
	{
		pthread_mutexattr_destroy(&mutex_attributes);
		pthread_mutex_destroy(mutex);
		pthread_condattr_destroy(&condition_attributes);
		delete condition;
		delete mutex;
		return;
	}

	pthread_mutexattr_destroy(&mutex_attributes);
	pthread_condattr_destroy(&condition_attributes);

	sem_count = new std::int32_t();
	*sem_count = count;
}

Semaphore::Semaphore(std::string name, std::int32_t count) : shared(true), mutex(nullptr), condition(nullptr), sem_count(nullptr), ref(nullptr), name("\0"), shm_fd(-1)
{
    bool created = false;
	shm_fd = shm_open(name.c_str(), O_RDWR, S_IRWXU); //0660 -> S_IRWXU
	if (errno == ENOENT)
	{
		shm_fd = shm_open(name.c_str(), O_CREAT | O_RDWR, S_IRWXU);
		created = true;
	}

	if (shm_fd == -1)
	{
		return;
	}

	if (ftruncate(shm_fd, sizeof(*this)))
	{
		perror(strerror(errno));
		if (created)
		{
			shm_unlink(name.c_str());
		}
		shm_fd = -1;
		return;
	}

	void* mapping = mmap(nullptr, sizeof(*this), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (mapping == MAP_FAILED)
	{
		if (created)
		{
			shm_unlink(name.c_str());
		}
		shm_fd = -1;
		return;
	}

	void* data = mapping;
	pthread_mutex_t* mutex = semaphore_cast<pthread_mutex_t>(data);
	pthread_cond_t* condition = semaphore_cast<pthread_cond_t>(data);
	std::int32_t* sem_count = semaphore_cast<std::int32_t>(data);
	std::int32_t* ref = semaphore_cast<std::int32_t>(data);

	if (created)
	{
		pthread_mutexattr_t mutex_attributes;
		if (pthread_mutexattr_init(&mutex_attributes))
		{
			shm_unlink(name.c_str());
			shm_fd = -1;
			return;
		}

		if (pthread_mutexattr_setpshared(&mutex_attributes, PTHREAD_PROCESS_SHARED))
		{
			pthread_mutexattr_destroy(&mutex_attributes);
			shm_unlink(name.c_str());
			shm_fd = -1;
			return;
		}

		if (pthread_mutex_init(mutex, &mutex_attributes))
		{
			pthread_mutexattr_destroy(&mutex_attributes);
			shm_unlink(name.c_str());
			shm_fd = -1;
			return;
		}

		pthread_condattr_t condition_attributes;
		if (pthread_condattr_init(&condition_attributes))
		{
			pthread_mutexattr_destroy(&mutex_attributes);
			pthread_mutex_destroy(mutex);
			shm_unlink(name.c_str());
			shm_fd = -1;
			return;
		}

		if (pthread_condattr_setpshared(&condition_attributes, PTHREAD_PROCESS_SHARED))
		{
			pthread_mutexattr_destroy(&mutex_attributes);
			pthread_mutex_destroy(mutex);
			pthread_condattr_destroy(&condition_attributes);
			shm_unlink(name.c_str());
			shm_fd = -1;
			return;
		}

		if (pthread_cond_init(condition, &condition_attributes))
		{
			pthread_mutexattr_destroy(&mutex_attributes);
			pthread_mutex_destroy(mutex);
			pthread_condattr_destroy(&condition_attributes);
			shm_unlink(name.c_str());
			shm_fd = -1;
			return;
		}

		pthread_mutexattr_destroy(&mutex_attributes);
		pthread_condattr_destroy(&condition_attributes);

		*sem_count = count;
	}

	*ref += 1;
	this->mutex = mutex;
	this->condition = condition;
	this->sem_count = sem_count;
	this->ref = ref;
}

Semaphore::~Semaphore()
{
    if (mutex && condition && shared)
	{
		void* data = static_cast<void*>(mutex);
		pthread_mutex_t* mutex = semaphore_cast<pthread_mutex_t>(data);
		pthread_cond_t* condition = semaphore_cast<pthread_cond_t>(data);
		std::ignore = semaphore_cast<std::int32_t>(data);
		std::int32_t* ref = semaphore_cast<std::int32_t>(data);
		*ref -= 1;

		if (*ref == 0)
		{
			pthread_cond_destroy(condition);
			pthread_mutex_destroy(mutex);
			shm_unlink(name.c_str());
		}
		munmap(mutex, sizeof(*this));
	}

	if (mutex && condition && !shared)
	{
		pthread_cond_destroy(condition);
		pthread_mutex_destroy(mutex);
		delete condition;
		delete mutex;
		delete sem_count;
	}

	if (shm_fd != -1)
	{
		close(shm_fd);
	}

	mutex = nullptr;
	condition = nullptr;
	sem_count = nullptr;
	ref = nullptr;
}
#endif

bool Semaphore::wait()
{
    #if defined(_WIN32) || defined(_WIN64)
    return WaitForSingleObject(hSemaphore, INFINITE) == WAIT_OBJECT_0;
    #else
    if (!mutex || !condition) { return false; }

    int res = pthread_mutex_lock(mutex);
    if (!res && res != EBUSY && res != EDEADLK)
    {
        while (*sem_count <= 0)
        {
            pthread_cond_wait(condition, mutex);
        }

		*sem_count -= 1;
        pthread_mutex_unlock(mutex);
        return true;
    }
    return false;
    #endif
}

bool Semaphore::try_wait()
{
    #if defined(_WIN32) || defined(_WIN64)
    return WaitForSingleObject(hSemaphore, 0) == WAIT_OBJECT_0;
    #else
    if (!mutex || !condition) { return false; }

    int res = 0;
    do
    {
        res = pthread_mutex_trylock(mutex);
    }
    while(res == EINTR);

    if (!res && res != EBUSY && res != EDEADLK)
    {
        while (*sem_count <= 0)
        {
            pthread_cond_wait(condition, mutex);
        }

        *sem_count -= 1;
        pthread_mutex_unlock(mutex);
        return true;
    }

    return false;
    #endif
}


template<typename Rep, typename Period>
bool Semaphore::try_wait_for(const std::chrono::duration<Rep, Period>& relative_time)
{
    std::chrono::steady_clock::duration rtime = std::chrono::duration_cast<std::chrono::steady_clock::duration>(relative_time);
    if(std::ratio_greater<std::chrono::steady_clock::period, Period>())
    {
        ++rtime;
    }
    return try_wait_until(std::chrono::steady_clock::now() + rtime);
}

template<typename Duration>
bool Semaphore::try_wait_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time)
{
    #if defined(_WIN32) || defined(_WIN64)
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::milliseconds> msec = std::chrono::time_point_cast<std::chrono::milliseconds>(absolute_time);
    return WaitForSingleObject(hSemaphore, msec.time_since_epoch().count()) == WAIT_OBJECT_0;
    #else
    if (!mutex || !condition) { return false; }

    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::seconds> sec = std::chrono::time_point_cast<std::chrono::seconds>(absolute_time);
    std::chrono::nanoseconds nano = std::chrono::duration_cast<std::chrono::nanoseconds>(absolute_time - sec);

	struct timespec ts = { sec.time_since_epoch().count(), nano.count() };

    int res = pthread_mutex_lock(mutex);
    if (!res && res != EBUSY && res != EDEADLK)
    {
        while (*sem_count == 0)
        {
            int res2 = pthread_cond_timedwait(condition, mutex, &ts);
            if (res2 == ETIMEDOUT || res2 == EINVAL)
            {
                pthread_mutex_unlock(mutex);
                return false;
            }

            *sem_count -= 1;
            pthread_mutex_unlock(mutex);
            return true;
        }
    }
    return false;
    #endif
}

template<typename Clock, typename Duration>
bool Semaphore::try_wait_until(const std::chrono::time_point<Clock, Duration>& absolute_time)
{
    return try_wait_until(std::chrono::high_resolution_clock::now() + (absolute_time - Clock::now()));
}

bool Semaphore::timed_wait(std::uint32_t milliseconds)
{
    if(!milliseconds)
    {
        return wait();
    }

    #if defined(_WIN32) || defined(_WIN64)
    return WaitForSingleObject(hSemaphore, milliseconds) == WAIT_OBJECT_0;
    #else
    auto duration = (std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(milliseconds)).time_since_epoch();
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

	struct timespec ts = { seconds.count(), nanoseconds.count() };

    int res = pthread_mutex_lock(mutex);
    if (!res && res != EBUSY && res != EDEADLK)
    {
        while (*sem_count == 0)
        {
            int res2 = pthread_cond_timedwait(condition, mutex, &ts);
            if (res2 == ETIMEDOUT || res2 == EINVAL)
            {
                pthread_mutex_unlock(mutex);
                return false;
            }

            *sem_count -= 1;
            pthread_mutex_unlock(mutex);
            return true;
        }
    }
    return false;
    #endif
}

bool Semaphore::signal()
{
    #if defined(_WIN32) || defined(_WIN64)
    return ReleaseSemaphore(hSemaphore, 1, nullptr);
    #else
    if (!mutex || !condition) { return false; }

    int res = pthread_mutex_lock(mutex);
    if (!res && res != EBUSY && res != EDEADLK)
    {
		*sem_count += 1;
        pthread_cond_signal(condition);
        pthread_mutex_unlock(mutex);
        return true;
    }
    return false;
    #endif
}
