//
//  SharedEvent.cxx
//  RemoteInput
//
//  Created by Brandon on 2019-09-14.
//  Copyright © 2019 XIO. All rights reserved.
//

#include "SharedEvent.hxx"
#include <tuple>

#if !(_POSIX_TIMEOUTS >= 200112L)
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

#if defined(_SYSTEM_V_SEMAPHORES) && !(_POSIX_TIMEOUTS >= 200112L) && !(_POSIX_C_SOURCE  >= 200112L)
int semtimedop(int handle, struct sembuf* operations, size_t operation_count, struct timespec* timeout)
{
	if (!timeout)
	{
		return semop(handle, operations, operation_count);
	}

	int retcode = 0;
	struct timespec sleeptime;
	sleeptime.tv_sec = 0;
	sleeptime.tv_nsec = 1000000; //1ms
	sleeptime.tv_nsec *= 5; //5ms

	while ((retcode = semop(handle, operations, operation_count)) != 0)
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

#if defined(_USE_POSIX_SEMAPHORES) && !(_POSIX_TIMEOUTS >= 200112L) && !(_POSIX_C_SOURCE  >= 200112L)
int sem_timedwait(sem_t* sem, const struct timespec* abs_timeout)
{
	if (!abs_timeout)
	{
		return sem_wait(sem);
	}

	int retcode = 0;
	struct timespec sleeptime;
	sleeptime.tv_sec = 0;
	sleeptime.tv_nsec = 1000000; //1ms
	sleeptime.tv_nsec *= 5; //5ms

	while ((retcode = sem_trywait(sem)) != 0)
	{
		//clock_gettime(CLOCK_REALTIME, &now); gettimeofday(&now, nullptr); timespec_get(&now, UTC_TIME)
		auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
		auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

		struct timespec now = { seconds.count(), nanoseconds.count() };
		if (now.tv_sec >= abs_timeout->tv_sec && now.tv_nsec >= abs_timeout->tv_nsec)
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

	if (!hMutex)
	{
		throw std::runtime_error("Cannot create mutex");
	}
}

Mutex::Mutex(std::string name) : hMutex(INVALID_HANDLE_VALUE)
{
    hMutex = OpenMutex(0, false, name.c_str());
    if (!hMutex)
    {
        hMutex = CreateMutex(0, false, name.c_str());
    }

	if (!hMutex)
	{
		throw std::runtime_error("Cannot create mutex");
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
		throw std::runtime_error("Cannot allocated mutex");
    }

    if (pthread_mutexattr_setpshared(&attributes, PTHREAD_PROCESS_PRIVATE))
    {
        pthread_mutexattr_destroy(&attributes);
        throw std::runtime_error("Cannot set mutex private attributes");
    }

	mutex = new pthread_mutex_t();
    if (pthread_mutex_init(mutex, &attributes))
    {
        pthread_mutexattr_destroy(&attributes);
		delete mutex;
        throw std::runtime_error("Cannot create mutex");
    }

    pthread_mutexattr_destroy(&attributes);
}

Mutex::Mutex(std::string name) : shared(true), mutex(nullptr), ref(nullptr), name(name), shm_fd(-1)
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
	pthread_mutex_t* mutex = mutex_cast<pthread_mutex_t>(data);
	std::int32_t* ref = mutex_cast<std::int32_t>(data);

	if (created)
	{
		pthread_mutexattr_t attributes;
		if (pthread_mutexattr_init(&attributes))
		{
			shm_unlink(name.c_str());
			shm_fd = -1;
			throw std::runtime_error("Cannot create mutex attributes");
		}

		if (pthread_mutexattr_setpshared(&attributes, PTHREAD_PROCESS_SHARED))
		{
			pthread_mutexattr_destroy(&attributes);
			shm_unlink(name.c_str());
			shm_fd = -1;
			throw std::runtime_error("Cannot set mutex shared attributes");
		}

		if (pthread_mutex_init(mutex, &attributes))
		{
			pthread_mutexattr_destroy(&attributes);
			shm_unlink(name.c_str());
			shm_fd = -1;
			throw std::runtime_error("Cannot create mutex");
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

	if (!hSemaphore)
	{
		throw std::runtime_error("Cannot create semaphore");
	}
}

Semaphore::Semaphore(std::string name, std::int32_t count)
{
    hSemaphore = OpenSemaphore(0, false, name.c_str());
    if (!hSemaphore)
    {
        hSemaphore = CreateSemaphore(nullptr, count, count + 1, name.c_str());
    }

	if (!hSemaphore)
	{
		throw std::runtime_error("Cannot create semaphore");
	}
}

Semaphore::~Semaphore()
{
    //ReleaseSemaphore(hSemaphore, 1, nullptr);
    CloseHandle(hSemaphore);
}
#elif defined(_USE_POSIX_SEMAPHORES)
template<typename T>
T* Semaphore::semaphore_cast(void* &ptr)
{
	std::uint8_t* it = static_cast<std::uint8_t*>(ptr);
	T* result = static_cast<T*>(ptr);
	ptr = (it + sizeof(T));
	return result;
}

Semaphore::Semaphore(std::int32_t count) : shared(false), owned(true), hSem(SEM_FAILED), name("\0")
{
	#if defined(__APPLE__)
	hSem = dispatch_semaphore_create(count);
	if (hSem == nullptr)
	{
		throw std::runtime_error("Cannot create or open semaphore");
	}
	#else
	hSem = sem_open(nullptr, O_CREAT | O_EXCL | O_RDWR, S_IRWXU, count); //sem_init with `pshared = 0`
	if (hSem == SEM_FAILED && errno == EEXIST)
	{
		owned = false;
		hSem = sem_open(nullptr, O_RDWR, S_IRWXU, count);
	}

	if (hSem != SEM_FAILED)
	{
		if (sem_init(static_cast<sem_t*>(hSem), false, count) == -1)
		{
			throw std::runtime_error("Cannot initialize semaphore");
		}
	}

	if (hSem == SEM_FAILED)
	{
		throw std::runtime_error("Cannot create or open semaphore");
	}
	#endif
}

Semaphore::Semaphore(std::string name, std::int32_t count) : shared(true), owned(true), hSem(SEM_FAILED), name(name)
{
	hSem = sem_open(name.c_str(), O_CREAT | O_EXCL | O_RDWR, S_IRWXU, count);
	if (hSem == SEM_FAILED && errno == EEXIST)
	{
		owned = false;
		hSem = sem_open(name.c_str(), O_RDWR, S_IRWXU, count);
	}

	if (hSem == SEM_FAILED)
	{
		throw std::runtime_error("Cannot create or open semaphore");
	}
}

Semaphore::~Semaphore()
{
	#if defined(__APPLE__)
	if (owned && !shared && hSem != nullptr)
	{
		dispatch_release(static_cast<dispatch_semaphore_t>(hSem));
	}
	else if (hSem != SEM_FAILED)
	{
		if (shared && name != "\0")
		{
			sem_close(static_cast<sem_t*>(hSem));
			hSem = SEM_FAILED;
		}

		if (owned && shared && name != "\0")
		{
			sem_unlink(name.c_str());
			name = "\0";
		}
	}
	#else
	if (hSem != SEM_FAILED)
	{
		if (!shared && name == "\0")
		{
			sem_destroy(static_cast<sem_t*>(hSem));
			hSem = SEM_FAILED;
		}
		else if (shared && name != "\0")
		{
			sem_close(static_cast<sem_t*>(hSem));
			hSem = SEM_FAILED;
		}

		if (owned && shared && name != "\0")
		{
			sem_unlink(name.c_str());
			name = "\0";
		}
	}
	#endif
}
#elif defined(_SYSTEM_V_SEMAPHORES)
template<typename T>
T* Semaphore::semaphore_cast(void* &ptr)
{
	std::uint8_t* it = static_cast<std::uint8_t*>(ptr);
	T* result = static_cast<T*>(ptr);
	ptr = (it + sizeof(T));
	return result;
}

Semaphore::Semaphore(std::int32_t count) : shared(false), id(IPC_PRIVATE), handle(-1), name("\0")
{
	union semun {
		int val;
		struct semid_ds* buf;
		unsigned short* array;

		#if defined(__linux__)
		struct seminfo* info;
		#endif
	} argument;

	handle = semget(id, 1, IPC_CREAT | IPC_EXCL | IPC_R | IPC_W | IPC_M);
	if (handle == -1 && errno == EEXIST)
	{
		handle = semget(id, 1, IPC_R | IPC_W | IPC_M);
	}

	if (handle == -1)
	{
		throw std::runtime_error("Cannot create or open semaphore");
	}

	argument.val = count;
	if (semctl(handle, 0, SETVAL, argument) == -1)
	{
		semctl(handle, 0, IPC_RMID);
		handle = -1;

		throw std::runtime_error("Cannot set semaphore attributes");
	}
}

Semaphore::Semaphore(std::string name, std::int32_t count) : shared(false), id(IPC_PRIVATE), handle(-1), name(name)
{
	union semun {
		int val;
		struct semid_ds* buf;
		unsigned short* array;

		#if defined(__linux__)
		struct seminfo* info;
		#endif
	} argument;

	id = std::hash<std::string>{}(name);
	handle = semget(id, 1, IPC_CREAT | IPC_EXCL | IPC_R | IPC_W | IPC_M);
	if (handle == -1 && errno == EEXIST)
	{
		handle = semget(id, 1, IPC_R | IPC_W | IPC_M);
	}

	if (handle == -1)
	{
		throw std::runtime_error("Cannot create or open semaphore");
	}

	argument.val = count;
	if (semctl(handle, 0, SETVAL, argument) == -1)
	{
		semctl(handle, 0, IPC_RMID);
		handle = -1;

		throw std::runtime_error("Cannot set semaphore attributes");
	}
}

Semaphore::~Semaphore()
{
	if (handle != -1)
	{
		semctl(handle, 0, IPC_RMID);
		handle = -1;
		id = -1;
	}
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
		throw std::runtime_error("Cannot create semaphore attributes");
	}

	if (pthread_mutexattr_setpshared(&mutex_attributes, PTHREAD_PROCESS_PRIVATE))
	{
		pthread_mutexattr_destroy(&mutex_attributes);
		throw std::runtime_error("Cannot set semaphore private attributes");
	}

	mutex = new pthread_mutex_t();
	if (pthread_mutex_init(mutex, &mutex_attributes))
	{
		pthread_mutexattr_destroy(&mutex_attributes);
		delete mutex;
		throw std::runtime_error("Cannot create semaphore");
	}

	pthread_condattr_t condition_attributes;
	if (pthread_condattr_init(&condition_attributes))
	{
		pthread_mutexattr_destroy(&mutex_attributes);
		pthread_mutex_destroy(mutex);
		delete mutex;
		throw std::runtime_error("Cannot create semaphore conditions");
	}

	if (pthread_condattr_setpshared(&condition_attributes, PTHREAD_PROCESS_PRIVATE))
	{
		pthread_mutexattr_destroy(&mutex_attributes);
		pthread_mutex_destroy(mutex);
		pthread_condattr_destroy(&condition_attributes);
		delete mutex;
		throw std::runtime_error("Cannot set semaphore conditions private attributes");
	}

	condition = new pthread_cond_t();
	if (pthread_cond_init(condition, &condition_attributes))
	{
		pthread_mutexattr_destroy(&mutex_attributes);
		pthread_mutex_destroy(mutex);
		pthread_condattr_destroy(&condition_attributes);
		delete condition;
		delete mutex;
		throw std::runtime_error("Cannot create semaphore conditions");
	}

	pthread_mutexattr_destroy(&mutex_attributes);
	pthread_condattr_destroy(&condition_attributes);

	sem_count = new std::int32_t();
	*sem_count = count;
}

Semaphore::Semaphore(std::string name, std::int32_t count) : shared(true), mutex(nullptr), condition(nullptr), sem_count(nullptr), ref(nullptr), name("\0"), shm_fd(-1)
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
		perror(strerror(errno));
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
			throw std::runtime_error("Cannot create semaphore attributes");
		}

		if (pthread_mutexattr_setpshared(&mutex_attributes, PTHREAD_PROCESS_SHARED))
		{
			pthread_mutexattr_destroy(&mutex_attributes);
			shm_unlink(name.c_str());
			shm_fd = -1;
			throw std::runtime_error("Cannot set semaphore shared attributes");
		}

		if (pthread_mutex_init(mutex, &mutex_attributes))
		{
			pthread_mutexattr_destroy(&mutex_attributes);
			shm_unlink(name.c_str());
			shm_fd = -1;
			throw std::runtime_error("Cannot create semaphore");
		}

		pthread_condattr_t condition_attributes;
		if (pthread_condattr_init(&condition_attributes))
		{
			pthread_mutexattr_destroy(&mutex_attributes);
			pthread_mutex_destroy(mutex);
			shm_unlink(name.c_str());
			shm_fd = -1;
			throw std::runtime_error("Cannot create semaphore condition attributes");
		}

		if (pthread_condattr_setpshared(&condition_attributes, PTHREAD_PROCESS_SHARED))
		{
			pthread_mutexattr_destroy(&mutex_attributes);
			pthread_mutex_destroy(mutex);
			pthread_condattr_destroy(&condition_attributes);
			shm_unlink(name.c_str());
			shm_fd = -1;
			throw std::runtime_error("Cannot set semaphore condition shared attributes");
		}

		if (pthread_cond_init(condition, &condition_attributes))
		{
			pthread_mutexattr_destroy(&mutex_attributes);
			pthread_mutex_destroy(mutex);
			pthread_condattr_destroy(&condition_attributes);
			shm_unlink(name.c_str());
			shm_fd = -1;
			throw std::runtime_error("Cannot create semaphore conditions");
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
	msync(mapping, sizeof(*this), MS_SYNC);
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

		msync(mutex, sizeof(*this), MS_SYNC);
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
	if (!hSemaphore) { return false; }
    return WaitForSingleObject(hSemaphore, INFINITE) == WAIT_OBJECT_0;
	#elif defined(_USE_POSIX_SEMAPHORES)
	#if defined(__APPLE__)
	if (owned && !shared)
	{
		if (hSem == nullptr) { return false; }
		return !dispatch_semaphore_wait(static_cast<dispatch_semaphore_t>(hSem), DISPATCH_TIME_FOREVER);
	}
	if (hSem == SEM_FAILED) { return false; }
	return !sem_wait(static_cast<sem_t*>(hSem));
	#else
	if (hSem == SEM_FAILED) { return false; }
	return !sem_wait(hSem);
	#endif
    #elif defined(_SYSTEM_V_SEMAPHORES)
	if (handle == -1) { return false; }
	struct sembuf operations[1];
	operations[0].sem_num = 0;
	operations[0].sem_op = -1;
	operations[0].sem_flg = 0;
	return !semop(handle, operations, 1);
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
		msync(mutex, sizeof(*this), MS_SYNC);
        pthread_mutex_unlock(mutex);
        return true;
    }
    return false;
    #endif
}

bool Semaphore::try_wait()
{
    #if defined(_WIN32) || defined(_WIN64)
	if (!hSemaphore) { return false; }
    return WaitForSingleObject(hSemaphore, 0) == WAIT_OBJECT_0;
    #elif defined(_USE_POSIX_SEMAPHORES)
	#if defined(__APPLE__)
	if (owned && !shared)
	{
		if (hSem == nullptr) { return false; }
		return !dispatch_semaphore_wait(static_cast<dispatch_semaphore_t>(hSem), DISPATCH_TIME_NOW);
	}
	if (hSem == SEM_FAILED) { return false; }
	return !sem_trywait(static_cast<sem_t*>(hSem));
	#else
	if (hSem == SEM_FAILED) { return false; }
	return !sem_trywait(hSem);
	#endif
    #elif defined(_SYSTEM_V_SEMAPHORES)
	if (handle == -1) { return false; }
	struct sembuf operations[1];
	operations[0].sem_num = 0;
	operations[0].sem_op = -1;
	operations[0].sem_flg = IPC_NOWAIT;
	return !semop(handle, operations, 1);
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
		msync(mutex, sizeof(*this), MS_SYNC);
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
	if (!hSemaphore) { return false; }

    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::milliseconds> msec = std::chrono::time_point_cast<std::chrono::milliseconds>(absolute_time);
    return WaitForSingleObject(hSemaphore, msec.time_since_epoch().count()) == WAIT_OBJECT_0;
    #elif defined(_USE_POSIX_SEMAPHORES)
	#if defined(__APPLE__)
	if (owned && !shared)
	{
		if (hSem == nullptr) { return false; }

		std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> now = std::chrono::high_resolution_clock::now();
		dispatch_time_t time_out = dispatch_time(DISPATCH_TIME_NOW, (absolute_time - now).count());
		return !dispatch_semaphore_wait(static_cast<dispatch_semaphore_t>(hSem), time_out);
	}

	if (hSem == SEM_FAILED) { return false; }
	std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::seconds> sec = std::chrono::time_point_cast<std::chrono::seconds>(absolute_time);
    std::chrono::nanoseconds nano = std::chrono::duration_cast<std::chrono::nanoseconds>(absolute_time - sec);

	struct timespec ts = { sec.time_since_epoch().count(), nano.count() };
	return !sem_timedwait(static_cast<sem_t*>(hSem), &ts);
	#else
	if (hSem == SEM_FAILED) { return false; }

	std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::seconds> sec = std::chrono::time_point_cast<std::chrono::seconds>(absolute_time);
    std::chrono::nanoseconds nano = std::chrono::duration_cast<std::chrono::nanoseconds>(absolute_time - sec);

	struct timespec ts = { sec.time_since_epoch().count(), nano.count() };
	return !sem_timedwait(hSem, &ts);
	#endif
    #elif defined(_SYSTEM_V_SEMAPHORES)
	std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::seconds> sec = std::chrono::time_point_cast<std::chrono::seconds>(absolute_time);
    std::chrono::nanoseconds nano = std::chrono::duration_cast<std::chrono::nanoseconds>(absolute_time - sec);

	struct timespec ts = { sec.time_since_epoch().count(), nano.count() };

	struct sembuf operations[1];
	operations[0].sem_num = 0;
	operations[0].sem_op = -1;
	operations[0].sem_flg = IPC_NOWAIT;
	return !semtimedop(handle, operations, 1, &ts);
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
			msync(mutex, sizeof(*this), MS_SYNC);
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
	if (!hSemaphore) { return false; }
    return WaitForSingleObject(hSemaphore, milliseconds) == WAIT_OBJECT_0;
    #elif defined(_USE_POSIX_SEMAPHORES)
	#if defined(__APPLE__)
	if (owned && !shared)
	{
		if (hSem == nullptr) { return false; }
		auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(milliseconds));
		dispatch_time_t time_out = dispatch_time(DISPATCH_TIME_NOW, duration.count());
		return !dispatch_semaphore_wait(static_cast<dispatch_semaphore_t>(hSem), time_out);
	}
	if (hSem == SEM_FAILED) { return false; }
	auto duration = (std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(milliseconds)).time_since_epoch();
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

	struct timespec ts = { seconds.count(), nanoseconds.count() };
	return !sem_timedwait(static_cast<sem_t*>(hSem), &ts);
	#endif
    #elif defined(_SYSTEM_V_SEMAPHORES)
	if (handle == -1) { return false; }

	auto duration = (std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(milliseconds)).time_since_epoch();
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

	struct timespec ts = { seconds.count(), nanoseconds.count() };

	struct sembuf operations[1];
	operations[0].sem_num = 0;
	operations[0].sem_op = -1;
	operations[0].sem_flg = IPC_NOWAIT;
	return !semtimedop(handle, operations, 1, &ts);
	#else
	if (!mutex || !condition) { return false; }

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
			msync(mutex, sizeof(*this), MS_SYNC);
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
	if (!hSemaphore) { return false; }
    return ReleaseSemaphore(hSemaphore, 1, nullptr);
    #elif defined(_USE_POSIX_SEMAPHORES)
	#if defined(__APPLE__)
	if (owned && !shared)
	{
		if (hSem == nullptr) { return false; }
		return !dispatch_semaphore_signal(static_cast<dispatch_semaphore_t>(hSem));
	}
	if (hSem == SEM_FAILED) { return false; }
	return !sem_post(static_cast<sem_t*>(hSem));
	#endif
    #elif defined(_SYSTEM_V_SEMAPHORES)
	if (handle == -1) { return false; }
	struct sembuf operations[1];
	operations[0].sem_num = 0;
	operations[0].sem_op = 1;
	operations[0].sem_flg = 0;
	return !semop(handle, operations, 1);
	#else
    if (!mutex || !condition) { return false; }

    int res = pthread_mutex_lock(mutex);
    if (!res && res != EBUSY && res != EDEADLK)
    {
		*sem_count += 1;
		msync(mutex, sizeof(*this), MS_SYNC);
        pthread_cond_signal(condition);
        pthread_mutex_unlock(mutex);
        return true;
    }
    return false;
    #endif
}
