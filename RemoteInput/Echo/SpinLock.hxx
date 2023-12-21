//
//  SpinLock.hxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-15.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef SPINLOCK_HXX_INCLUDED
#define SPINLOCK_HXX_INCLUDED

#include <cstdint>
#include <chrono>
#include <string>
#include <atomic>

class SpinLock
{
private:
    bool shared;
    int shm_fd;
    std::string name;
    std::atomic_flag* flag;
    std::int32_t* ref;

    bool timed_lock(std::uint64_t nanoseconds) const noexcept;

public:
    SpinLock();
    SpinLock(std::string name);
    ~SpinLock();

    SpinLock(const SpinLock &other) = delete;
    SpinLock& operator = (const SpinLock &other) = delete;

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

class SpinningSemaphore
{
private:
    bool shared;
    int shm_fd;
    std::string name;
    std::atomic_flag* lock;
    std::atomic<std::int32_t>* count;
    std::int32_t* ref;

    bool timed_wait(std::uint64_t nanoseconds) const noexcept;

public:
    SpinningSemaphore(std::int32_t count = 0);
    SpinningSemaphore(std::string name, std::int32_t count = 0);
    ~SpinningSemaphore();

    SpinningSemaphore(const SpinningSemaphore &other) = delete;
    SpinningSemaphore& operator = (const SpinningSemaphore &other) = delete;

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

class SpinningSignal
{
private:
    bool shared;
    int shm_fd;
    std::string name;
    std::atomic_bool* lock;
    std::int32_t* ref;

    bool timed_wait(std::uint64_t nanoseconds) const noexcept;

public:
    SpinningSignal(std::int32_t count = 0);
    SpinningSignal(std::string name, std::int32_t count = 0);
    ~SpinningSignal();

    SpinningSignal(const SpinningSignal &other) = delete;
    SpinningSignal& operator = (const SpinningSignal &other) = delete;

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

#endif /* SPINLOCK_HXX_INCLUDED */
