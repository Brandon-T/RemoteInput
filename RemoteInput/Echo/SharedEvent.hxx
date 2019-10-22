//
//  SharedEvent.hxx
//  RemoteInput
//
//  Created by Brandon on 2019-09-14.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef SHAREDEVENT_HXX_INCLUDED
#define SHAREDEVENT_HXX_INCLUDED

#if defined(_WIN32) || defined(_WIN64)
#include <pthread.h>
#include <windows.h>
#else
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#endif

#include <ctime>
#include <cstdint>
#include <string>
#include <chrono>

#include "Time.hxx"

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
    #else
	bool shared;
    pthread_mutex_t* mutex;
    pthread_cond_t* condition;
    std::int32_t* sem_count;
    std::int32_t* ref;
    std::string name;
    int shm_fd;

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

#endif /* SHAREDEVENT_HXX_INCLUDED */
