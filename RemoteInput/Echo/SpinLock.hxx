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

class SpinningSemaphore
{
private:
	bool shared;
	int shm_fd;
	std::string name;
	std::atomic_flag* lock;
	std::atomic_int32_t* count;
	std::int32_t* ref;
	
public:
	SpinningSemaphore(std::int32_t count = 0);
    SpinningSemaphore(std::string name, std::int32_t count = 0);
    ~SpinningSemaphore();

    SpinningSemaphore(const SpinningSemaphore &other) = delete;
    SpinningSemaphore& operator = (const SpinningSemaphore &other) = delete;

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

#endif /* SPINLOCK_HXX_INCLUDED */
