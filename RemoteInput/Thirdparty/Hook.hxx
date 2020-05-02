//
// Created by brandon on 2020-04-06.
//

#ifndef HOOK_HXX
#define HOOK_HXX

#include <utility>
#include <cstdint>
#include <string>

class Hook
{
private:
    void* original;
    void* detour;
    void* trampoline;
    std::uint8_t data[32];

public:
    Hook(void* original, void* detour);
    Hook(const Hook& other) = delete;
    Hook(Hook&& other);
    ~Hook();

    Hook& operator = (const Hook& other) = delete;
    Hook& operator = (Hook&& other) = delete;

    void apply();
    void remove();

    template<typename R, typename FuncPtr, typename... Args>
    R call(Args... args);
};

template<typename R, typename FuncPtr, typename... Args>
R Hook::call( Args... args)
{
#if defined(_WIN32) || defined(_WIN64)
    return reinterpret_cast<FuncPtr*>(trampoline ?: original)(std::forward<Args>(args)...);
#elif defined(__APPLE__)
    return reinterpret_cast<FuncPtr*>(trampoline ?: original)(std::forward<Args>(args)...);
#else
    return reinterpret_cast<FuncPtr*>(original)(std::forward<Args>(args)...);
#endif
}

#endif //HOOK_HXX
