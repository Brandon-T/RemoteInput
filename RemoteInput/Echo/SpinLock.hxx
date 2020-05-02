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

public:
	SpinLock();
    SpinLock(std::string name);
    ~SpinLock();

    SpinLock(const SpinLock &other) = delete;
    SpinLock& operator = (const SpinLock &other) = delete;

    bool lock() const noexcept;
    bool try_lock() const noexcept;
    bool timed_lock(std::uint32_t milliseconds) const noexcept;
    bool unlock() const noexcept;


    template<typename Rep, typename Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& relative_time) const noexcept;

    template<typename Duration>
    bool try_lock_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time) const noexcept;

    template<typename Clock, typename Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& absolute_time) const noexcept;
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

public:
	SpinningSemaphore(std::int32_t count = 0);
    SpinningSemaphore(std::string name, std::int32_t count = 0);
    ~SpinningSemaphore();

    SpinningSemaphore(const SpinningSemaphore &other) = delete;
    SpinningSemaphore& operator = (const SpinningSemaphore &other) = delete;

    bool wait() const noexcept;
    bool try_wait() const noexcept;
    bool timed_wait(std::uint32_t milliseconds) const noexcept;
    bool signal() const noexcept;


    template<typename Rep, typename Period>
    bool try_wait_for(const std::chrono::duration<Rep, Period>& relative_time) const noexcept;

    template<typename Duration>
    bool try_wait_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time) const noexcept;

    template<typename Clock, typename Duration>
    bool try_wait_until(const std::chrono::time_point<Clock, Duration>& absolute_time) const noexcept;
};





class SpinningSignal
{
private:
	bool shared;
	int shm_fd;
	std::string name;
	std::atomic_bool* lock;
	std::int32_t* ref;

public:
	SpinningSignal(std::int32_t count = 0);
    SpinningSignal(std::string name, std::int32_t count = 0);
    ~SpinningSignal();

    SpinningSignal(const SpinningSignal &other) = delete;
    SpinningSignal& operator = (const SpinningSignal &other) = delete;

    bool wait() const noexcept;
    bool try_wait() const noexcept;
    bool timed_wait(std::uint32_t milliseconds) const noexcept;
    bool signal() const noexcept;


    template<typename Rep, typename Period>
    bool try_wait_for(const std::chrono::duration<Rep, Period>& relative_time) const noexcept;

    template<typename Duration>
    bool try_wait_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time) const noexcept;

    template<typename Clock, typename Duration>
    bool try_wait_until(const std::chrono::time_point<Clock, Duration>& absolute_time) const noexcept;
};

#endif /* SPINLOCK_HXX_INCLUDED */
