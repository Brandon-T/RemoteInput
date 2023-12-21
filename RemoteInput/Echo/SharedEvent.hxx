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

#if !defined(_MSC_VER)
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
#endif  // _MSC_VER

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

    bool timed_lock(std::uint64_t nanoseconds) const noexcept;

public:
    Mutex();
    Mutex(std::string name);
    ~Mutex();

    Mutex(const Mutex &other) = delete;
    Mutex& operator = (const Mutex &other) = delete;

    bool lock() const noexcept;
    bool try_lock() const noexcept;
    bool unlock() const noexcept;

    template<typename Rep, typename Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& relative_time) const noexcept
    {
        std::chrono::steady_clock::duration rtime = std::chrono::duration_cast<std::chrono::steady_clock::duration>(relative_time);
        if(std::ratio_greater<std::chrono::steady_clock::period, Period>())
        {
            ++rtime;
        }
        return try_lock_until(std::chrono::steady_clock::now() + rtime);
    }

    template<typename Duration>
    bool try_lock_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time) const noexcept
    {
        return timed_lock(std::chrono::duration_cast<std::chrono::nanoseconds>(absolute_time - std::chrono::high_resolution_clock::now()).count());
    }

    template<typename Clock, typename Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& absolute_time) const noexcept
    {
        return try_lock_until(std::chrono::high_resolution_clock::now() + (absolute_time - Clock::now()));
    }
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

    bool timed_wait(std::uint64_t nanoseconds) const noexcept;

public:
    Semaphore(std::int32_t count = 0);
    Semaphore(std::string name, std::int32_t count = 0);
    ~Semaphore();

    Semaphore(const Semaphore &other) = delete;
    Semaphore& operator = (const Semaphore &other) = delete;

    bool wait() const noexcept;
    bool try_wait() const noexcept;
    bool signal() const noexcept;


    template<typename Rep, typename Period>
    bool try_wait_for(const std::chrono::duration<Rep, Period>& relative_time) const noexcept
    {
        std::chrono::steady_clock::duration rtime = std::chrono::duration_cast<std::chrono::steady_clock::duration>(relative_time);
        if(std::ratio_greater<std::chrono::steady_clock::period, Period>())
        {
            ++rtime;
        }
        return try_wait_until(std::chrono::steady_clock::now() + rtime);
    }

    template<typename Duration>
    bool try_wait_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time) const noexcept
    {
        return timed_wait(std::chrono::duration_cast<std::chrono::nanoseconds>(absolute_time - std::chrono::high_resolution_clock::now()).count());
    }

    template<typename Clock, typename Duration>
    bool try_wait_until(const std::chrono::time_point<Clock, Duration>& absolute_time) const noexcept
    {
        return try_wait_until(std::chrono::high_resolution_clock::now() + (absolute_time - Clock::now()));
    }
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

    bool timed_wait(std::uint64_t nanoseconds) const noexcept;

public:
    AtomicSignal();
    AtomicSignal(std::string name);
    ~AtomicSignal();

    bool wait() const noexcept;
    bool try_wait() const noexcept;
    bool signal() const noexcept;
    bool is_signalled() const noexcept;


    template<typename Rep, typename Period>
    bool try_wait_for(const std::chrono::duration<Rep, Period>& relative_time) const noexcept
    {
        std::chrono::steady_clock::duration rtime = std::chrono::duration_cast<std::chrono::steady_clock::duration>(relative_time);
        if(std::ratio_greater<std::chrono::steady_clock::period, Period>())
        {
            ++rtime;
        }
        return try_wait_until(std::chrono::steady_clock::now() + rtime);
    }

    template<typename Duration>
    bool try_wait_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time) const noexcept
    {
        return timed_wait(std::chrono::duration_cast<std::chrono::nanoseconds>(absolute_time - std::chrono::high_resolution_clock::now()).count());
    }

    template<typename Clock, typename Duration>
    bool try_wait_until(const std::chrono::time_point<Clock, Duration>& absolute_time) const noexcept
    {
        return try_wait_until(std::chrono::high_resolution_clock::now() + (absolute_time - Clock::now()));
    }
};

#endif /* SHAREDEVENT_HXX_INCLUDED */
