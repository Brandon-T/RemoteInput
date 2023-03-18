//
//  Signal.hxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-15.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef SIGNAL_HXX_INCLUDED
#define SIGNAL_HXX_INCLUDED

#include "SpinLock.hxx"
#include "SharedEvent.hxx"

template<typename T>
class Signal
{
private:
    using ValidationType = typename std::enable_if<std::is_same<T, Semaphore>::value || std::is_same<T, Mutex>::value || std::is_same<T, SpinLock>::value || std::is_same<T, SpinningSemaphore>::value || std::is_same<T, SpinningSignal>::value>::type;
    void* underlying_type;

public:
    template<typename U = T, typename = ValidationType>
    Signal() noexcept;

    template<typename U = T, typename = ValidationType>
    Signal(std::string name) noexcept;

    ~Signal() noexcept;

    Signal(const Signal &other) = delete;
    Signal& operator = (const Signal &other) = delete;

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

template<typename T>
template<typename U,  typename>
Signal<T>::Signal() noexcept
{
    underlying_type = new T();
}

template<typename T>
template<typename U, typename>
Signal<T>::Signal(std::string name) noexcept
{
    underlying_type = new T(name);
}

template<typename T>
Signal<T>::~Signal() noexcept
{
    delete static_cast<T*>(underlying_type);
    underlying_type = nullptr;
}


template<typename T>
bool Signal<T>::wait() const noexcept
{
    if constexpr (std::is_same<T, Semaphore>::value)
    {
        return static_cast<Semaphore*>(underlying_type)->wait();
    }
    else if constexpr (std::is_same<T, SpinLock>::value)
    {
        return static_cast<SpinLock*>(underlying_type)->lock();
    }
    else if constexpr (std::is_same<T, Mutex>::value)
    {
        return static_cast<Mutex*>(underlying_type)->lock();
    }
    else if constexpr (std::is_same<T, SpinningSemaphore>::value)
    {
        return static_cast<SpinningSemaphore*>(underlying_type)->wait();
    }
    else if constexpr (std::is_same<T, SpinningSignal>::value)
    {
        return static_cast<SpinningSignal*>(underlying_type)->wait();
    }
    return false;
}

template<typename T>
bool Signal<T>::try_wait() const noexcept
{
    if constexpr (std::is_same<T, Semaphore>::value)
    {
        return static_cast<Semaphore*>(underlying_type)->try_wait();
    }
    else if constexpr (std::is_same<T, SpinLock>::value)
    {
        return static_cast<SpinLock*>(underlying_type)->try_lock();
    }
    else if constexpr (std::is_same<T, Mutex>::value)
    {
        return static_cast<Mutex*>(underlying_type)->try_lock();
    }
    else if constexpr (std::is_same<T, SpinningSemaphore>::value)
    {
        return static_cast<SpinningSemaphore*>(underlying_type)->try_wait();
    }
    else if constexpr (std::is_same<T, SpinningSignal>::value)
    {
        return static_cast<SpinningSignal*>(underlying_type)->try_wait();
    }
    return false;
}

template<typename T>
bool Signal<T>::timed_wait(std::uint32_t milliseconds) const noexcept
{
    if constexpr (std::is_same<T, Semaphore>::value)
    {
        return static_cast<Semaphore*>(underlying_type)->timed_wait(milliseconds);
    }
    else if constexpr (std::is_same<T, SpinLock>::value)
    {
        return static_cast<SpinLock*>(underlying_type)->timed_lock(milliseconds);
    }
    else if constexpr (std::is_same<T, Mutex>::value)
    {
        return static_cast<Mutex*>(underlying_type)->timed_lock(milliseconds);
    }
    else if constexpr (std::is_same<T, SpinningSemaphore>::value)
    {
        return static_cast<SpinningSemaphore*>(underlying_type)->timed_wait(milliseconds);
    }
    else if constexpr (std::is_same<T, SpinningSignal>::value)
    {
        return static_cast<SpinningSignal*>(underlying_type)->timed_wait(milliseconds);
    }
    return false;
}

template<typename T>
bool Signal<T>::signal() const noexcept
{
    if constexpr (std::is_same<T, Semaphore>::value)
    {
        return static_cast<Semaphore*>(underlying_type)->signal();
    }
    else if constexpr (std::is_same<T, SpinLock>::value)
    {
        return static_cast<SpinLock*>(underlying_type)->unlock();
    }
    else if constexpr (std::is_same<T, Mutex>::value)
    {
        return static_cast<Mutex*>(underlying_type)->unlock();
    }
    else if constexpr (std::is_same<T, SpinningSemaphore>::value)
    {
        return static_cast<SpinningSemaphore*>(underlying_type)->signal();
    }
    else if constexpr (std::is_same<T, SpinningSignal>::value)
    {
        return static_cast<SpinningSignal*>(underlying_type)->signal();
    }
    return false;
}


template<typename T>
template<typename Rep, typename Period>
bool Signal<T>::try_wait_for(const std::chrono::duration<Rep, Period>& relative_time) const noexcept
{
    if constexpr (std::is_same<T, Semaphore>::value)
    {
        return static_cast<Semaphore*>(underlying_type)->try_wait_for(relative_time);
    }
    else if constexpr (std::is_same<T, SpinLock>::value)
    {
        return static_cast<SpinLock*>(underlying_type)->try_lock_for(relative_time);
    }
    else if constexpr (std::is_same<T, Mutex>::value)
    {
        return static_cast<Mutex*>(underlying_type)->try_lock_for(relative_time);
    }
    else if constexpr (std::is_same<T, SpinningSemaphore>::value)
    {
        return static_cast<SpinningSemaphore*>(underlying_type)->try_wait_for(relative_time);
    }
    else if constexpr (std::is_same<T, SpinningSignal>::value)
    {
        return static_cast<SpinningSignal*>(underlying_type)->try_wait_for(relative_time);
    }
    return false;
}

template<typename T>
template<typename Duration>
bool Signal<T>::try_wait_until(const std::chrono::time_point<std::chrono::high_resolution_clock, Duration>& absolute_time) const noexcept
{
    if constexpr (std::is_same<T, Semaphore>::value)
    {
        return static_cast<Semaphore*>(underlying_type)->try_wait_until(absolute_time);
    }
    else if constexpr (std::is_same<T, SpinLock>::value)
    {
        return static_cast<SpinLock*>(underlying_type)->try_lock_until(absolute_time);
    }
    else if constexpr (std::is_same<T, Mutex>::value)
    {
        return static_cast<Mutex*>(underlying_type)->try_lock_until(absolute_time);
    }
    else if constexpr (std::is_same<T, SpinningSemaphore>::value)
    {
        return static_cast<SpinningSemaphore*>(underlying_type)->try_wait_until(absolute_time);
    }
    else if constexpr (std::is_same<T, SpinningSignal>::value)
    {
        return static_cast<SpinningSignal*>(underlying_type)->try_wait_until(absolute_time);
    }
    return false;
}

template<typename T>
template<typename Clock, typename Duration>
bool Signal<T>::try_wait_until(const std::chrono::time_point<Clock, Duration>& absolute_time) const noexcept
{
    if constexpr (std::is_same<T, Semaphore>::value)
    {
        return static_cast<Semaphore*>(underlying_type)->try_wait_until(absolute_time);
    }
    else if constexpr (std::is_same<T, SpinLock>::value)
    {
        return static_cast<SpinLock*>(underlying_type)->try_lock_until(absolute_time);
    }
    else if constexpr (std::is_same<T, Mutex>::value)
    {
        return static_cast<Mutex*>(underlying_type)->try_lock_until(absolute_time);
    }
    else if constexpr (std::is_same<T, SpinningSemaphore>::value)
    {
        return static_cast<SpinningSemaphore*>(underlying_type)->try_wait_until(absolute_time);
    }
    else if constexpr (std::is_same<T, SpinningSignal>::value)
    {
        return static_cast<SpinningSignal*>(underlying_type)->try_wait_until(absolute_time);
    }
    return false;
}

#endif /* SIGNAL_HXX_INCLUDED */
