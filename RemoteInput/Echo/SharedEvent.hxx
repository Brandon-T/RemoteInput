//
//  SharedEvent.hxx
//  RemoteInput
//
//  Created by Brandon on 2019-09-14.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef SHAREDEVENT_HXX_INCLUDED
#define SHAREDEVENT_HXX_INCLUDED

#include <ctime>
#include <cstdint>
#include <string>
#include <chrono>
#include <atomic>
#include "Time.hxx"

#if !defined(_WIN32) && !defined(_WIN64)
#define _USE_POSIX_SEMAPHORES
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#if defined(_USE_POSIX_SEMAPHORES)
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#if !defined(_WIN32) && !defined(_WIN64)
#include <sys/mman.h>
#endif
#if defined(__APPLE__)
#include <dispatch/dispatch.h>
#endif
#elif defined(_USE_SYSTEM_V_SEMAPHORES)
#include <pthread.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#else
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#if !defined(_WIN32) && !defined(_WIN64)
#include <sys/mman.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#endif

class Mutex
{
private:
    #if defined(_WIN32) || defined(_WIN64)
    HANDLE hMutex;
    #else
	bool shared;
    pthread_mutex_t* mutex;
    std::int32_t* ref;
    std::string name;
    int shm_fd;

    template<typename T>
    T* mutex_cast(void* &ptr);
    #endif

public:
    Mutex();
    Mutex(std::string name);
    ~Mutex();

    Mutex(const Mutex &other) = delete;
    Mutex& operator = (const Mutex &other) = delete;

    bool lock();
    bool try_lock();
    bool timed_lock(std::uint32_t milliseconds);
    bool unlock();


    template<typename Rep, typename Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& relative_time);

    template<typename Duration>
    bool try_lock_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time);

    template<typename Clock, typename Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& absolute_time);
};

class Semaphore
{
private:
    #if defined(_WIN32) || defined(_WIN64)
    HANDLE hSemaphore;
    #elif defined(_USE_POSIX_SEMAPHORES)
	bool shared;
	bool owned;
	void* hSem;
	std::string name;
	#elif defined(_USE_SYSTEM_V_SEMAPHORES)
	bool shared;
	key_t id;
	int handle;
	std::string name;
	#else
	bool shared;
    pthread_mutex_t* mutex;
    pthread_cond_t* condition;
    std::int32_t* sem_count;
    std::int32_t* ref;
    std::string name;
    int shm_fd;
    #endif

	#if !defined(_WIN32) && !defined(_WIN64)
	template<typename T>
    T* semaphore_cast(void* &ptr);
	#endif

public:
    Semaphore(std::int32_t count = 0);
    Semaphore(std::string name, std::int32_t count = 0);
    ~Semaphore();

    Semaphore(const Semaphore &other) = delete;
    Semaphore& operator = (const Semaphore &other) = delete;

    bool wait();
    bool try_wait();
    bool timed_wait(std::uint32_t milliseconds);
    bool signal();


    template<typename Rep, typename Period>
    bool try_wait_for(const std::chrono::duration<Rep, Period>& relative_time);

    template<typename Duration>
    bool try_wait_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time);

    template<typename Clock, typename Duration>
    bool try_wait_until(const std::chrono::time_point<Clock, Duration>& absolute_time);
};

class AtomicSignal
{
private:
	#if defined(_WIN32) || defined(_WIN64)
	HANDLE hEvent;
	std::string name;
	#else
	bool shared;
	int shm_fd;
	std::string name;
	std::atomic_bool* lock;
	std::int32_t* ref;
	#endif

public:
	AtomicSignal();
	AtomicSignal(std::string name);
	~AtomicSignal();

	bool wait();
	bool try_wait();

	bool timed_wait(std::uint32_t milliseconds);
	bool signal();
	bool is_signalled();


	template<typename Rep, typename Period>
	bool try_wait_for(const std::chrono::duration<Rep, Period>& relative_time);

	template<typename Duration>
	bool try_wait_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time);

	template<typename Clock, typename Duration>
	bool try_wait_until(const std::chrono::time_point<Clock, Duration>& absolute_time);
};

#endif /* SHAREDEVENT_HXX_INCLUDED */
