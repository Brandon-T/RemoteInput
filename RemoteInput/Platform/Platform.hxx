#ifndef PLATFORM_HXX_INCLUDED
#define PLATFORM_HXX_INCLUDED

#include <chrono>
#include <ratio>
#include <thread>
#include <cstdint>
#include <vector>
#include "Reflection.hxx"

void GetDesktopResolution(int &width, int &height);
Reflection* GetNativeReflector();

std::int32_t GetCurrentThreadID();

bool IsProcessAlive(pid_t pid);
bool IsThreadAlive(std::int32_t tid);
std::vector<pid_t> get_pids();
std::vector<pid_t> get_pids(const char* process_name);
pid_t PIDFromWindow(void* window);

template<typename T>
void yield_thread(std::chrono::duration<std::int64_t, T> time)
{
    /*auto start = std::chrono::high_resolution_clock::now();
    auto end = start + time;
    do {
        //std::this_thread::yield();
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    } while (std::chrono::high_resolution_clock::now() < end);*/
	std::this_thread::sleep_for(time);
}

template<typename T>
std::int64_t elapsed_time(std::chrono::high_resolution_clock::time_point start)
{
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<T>(end - start).count();
}

#endif // PLATFORM_HXX_INCLUDED
