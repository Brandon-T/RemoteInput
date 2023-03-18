//
//  Plugin.cxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-21.
//  Copyright © 2019 XIO. All rights reserved.
//

#include "Plugin.hxx"
#include <memory>
#include <atomic>

#include "MemoryMap.hxx"
#include "Platform.hxx"
#include "NativeHooks.hxx"
#include "ControlCenter.hxx"
#include "EIOS.hxx"

#if defined(_WIN32) || defined(_WIN64)
HMODULE module = nullptr;
#endif

std::unique_ptr<ControlCenter> control_center;

// MARK: - MAIN

#if defined(_WIN32) || defined(_WIN64)
[[gnu::stdcall]] void __load()
{
    #if defined(DEBUG)
    printf("ATTACHED TO: %d\n", getpid());
    #endif

    //Increase our reference count by 1..
    //So that if someone calls `FreeLibrary` before the thread exists, we won't get a crash.
    //Later on we will call `FreeLibraryAndExitThread`
    HMODULE this_module = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCTSTR>(module), &this_module);

    std::thread([](HMODULE module){
        auto reflector = std::unique_ptr<Reflection>(GetNativeReflector());
        if (reflector)
        {
            control_center = std::make_unique<ControlCenter>(getpid(), false, std::move(reflector));
            StartHook();
        }

        //Decrease our reference count by 1..
        //So if `FreeLibrary` was called previous, our count reaches 0 and we'll be freed.
        FreeLibraryAndExitThread(module, 0);
    }, this_module).detach();
}

[[gnu::stdcall]] void __unload()
{
    control_center.reset();
}
#elif defined(__APPLE__)
[[gnu::constructor]] void __load()
{
    printf("ATTACHED TO: %d\n", getpid());

    extern void disable_app_nap();
    std::thread([&] {
        disable_app_nap();

        auto reflector = std::unique_ptr<Reflection>(GetNativeReflector());
        if (reflector)
        {
            control_center = std::make_unique<ControlCenter>(getpid(), false, std::move(reflector));
            StartHook();
        }
    }).detach();
}

[[gnu::destructor]] void __unload()
{
    control_center.reset();
}
#else
[[gnu::constructor]] void __load()
{
    printf("ATTACHED TO: %d\n", getpid());

    //Increase our reference count by 1..
    //So that if someone calls `dlclose` before the thread exists, we won't get a crash.
    //Later on we will call `dlclose` on a detached thread.

    Dl_info this_info = {0};
    dladdr(reinterpret_cast<void*>(&__load), &this_info);
    void* this_module = dlopen(this_info.dli_fname, RTLD_LAZY);

    std::thread([&](void* this_module) {

        auto reflector = std::unique_ptr<Reflection>(GetNativeReflector());
        if (reflector)
        {
            control_center = std::make_unique<ControlCenter>(getpid(), false, std::move(reflector));
            StartHook();
        }

        //std::thread([this_module]{ if (this_module) { dlclose(this_module); } }).detach();
        dlclose(this_module);
    }, this_module).detach();
}

[[gnu::destructor]] void __unload()
{
    control_center.reset();
}
#endif // defined


#if defined(_WIN32) || defined(_WIN64)
extern "C" EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            module = hinstDLL;
            DisableThreadLibraryCalls(hinstDLL);

            __load();
        }
            break;

        case DLL_PROCESS_DETACH:
        {
            __unload();
        }
            break;

        case DLL_THREAD_ATTACH:
            //printf("ATTACHED\n");
            break;

        case DLL_THREAD_DETACH:
            //printf("DETACHED\n");
            break;
    }
    return TRUE; // succesful
}
#endif // defined
