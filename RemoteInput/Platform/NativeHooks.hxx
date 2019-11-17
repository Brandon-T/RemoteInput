#ifndef NATIVEHOOKS_HXX_INCLUDED
#define NATIVEHOOKS_HXX_INCLUDED

#include <chrono>
#include <ratio>
#include <thread>
#include <cstdint>

void StartHook();

template<typename T>
void yield_thread(std::chrono::duration<std::int64_t, T> time)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + time;
    do {
        std::this_thread::yield();
    } while (std::chrono::high_resolution_clock::now() < end);
}

template<typename T>
std::int64_t elapsed_time(std::chrono::high_resolution_clock::time_point start)
{
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<T>(end - start).count();
}

#endif // NATIVEHOOKS_HXX_INCLUDED
