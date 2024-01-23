//
//  Signal.hxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-15.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef SIGNAL_HXX_INCLUDED
#define SIGNAL_HXX_INCLUDED

#include "Synchronization.hxx"

template<typename T>
class Signal
{
private:
    T primitive;

public:
    Signal(std::string name) noexcept requires synchronization::lockable<T> || synchronization::acquirable<T>;
    ~Signal() = default;

    Signal(const Signal &other) = delete;
    Signal& operator = (const Signal &other) = delete;

    void signal() const noexcept;
    void wait() const noexcept;

    bool try_wait() const noexcept;

    template<typename Rep, typename Period>
    bool try_wait_for(const std::chrono::duration<Rep, Period>& relative_time) const noexcept;

    template<typename Clock, typename Duration>
    bool try_wait_until(const std::chrono::time_point<Clock, Duration>& absolute_time) const noexcept;
};

template<typename T>
Signal<T>::Signal(std::string name) noexcept requires synchronization::lockable<T> || synchronization::acquirable<T>
        : primitive(name)
{
}

template<typename T>
void Signal<T>::signal() const noexcept
{
    if constexpr(synchronization::lockable<T>)
    {
        primitive.unlock();
    }
    else
    {
        primitive.release();
    }
}

template<typename T>
void Signal<T>::wait() const noexcept
{
    if constexpr(synchronization::lockable<T>)
    {
        primitive.lock();
    }
    else
    {
        primitive.acquire();
    }
}

template<typename T>
bool Signal<T>::try_wait() const noexcept
{
    if constexpr(synchronization::lockable<T>)
    {
        return primitive.try_lock();
    }
    else
    {
        return primitive.try_acquire();
    }
}


template<typename T>
template<typename Rep, typename Period>
bool Signal<T>::try_wait_for(const std::chrono::duration<Rep, Period>& relative_time) const noexcept
{
    if constexpr(synchronization::lockable<T>)
    {
        return primitive.try_lock_for(relative_time);
    }
    else
    {
        return primitive.try_acquire_for(relative_time);
    }
}

template<typename T>
template<typename Clock, typename Duration>
bool Signal<T>::try_wait_until(const std::chrono::time_point<Clock, Duration>& absolute_time) const noexcept
{
    if constexpr(synchronization::lockable<T>)
    {
        return primitive.try_lock_until(absolute_time);
    }
    else
    {
        return primitive.try_acquire_until(absolute_time);
    }
}

#endif /* SIGNAL_HXX_INCLUDED */
