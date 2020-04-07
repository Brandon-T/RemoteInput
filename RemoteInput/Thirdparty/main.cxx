//
// Created by brandon on 2020-04-06.
//

#include "main.hxx"
#include <cassert>
#include <cstring>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <MinHook.h>
#elif defined(__APPLE__)
#include <rd_route.h>
#include <mach_inject.h>
#include <mach/mach_vm.h>
#include <mach/mach_init.h>
#else
#include <detours.h>
#endif


#if defined(_WIN32) || defined(_WIN64)
Hook::Hook(void* original, void* detour) : original(original), detour(detour), trampoline(nullptr), data()
{
    std::memset(&data[0], 0, sizeof(data));

    MH_Initialize();
    MH_CreateHook(original, detour, &trampoline);
}

Hook::Hook(Hook&& other) : original(nullptr), detour(nullptr), trampoline(nullptr), data()
{
    other.original = nullptr;
    other.detour = nullptr;
    other.trampoline = nullptr;
    std::memset(&other.data[0], 0, sizeof(data));
}

Hook::~Hook()
{
    MH_Uninitialize();
}

void Hook::apply()
{
    MH_EnableHook(original);
}

void Hook::remove()
{
    MH_DisableHook(original);
}
#elif defined(__APPLE__)
Hook::Hook(void* original, void* detour) : original(original), detour(detour), trampoline(nullptr), data()
{
    std::memset(&data[0], 0, sizeof(data));

    #if defined (__x86_64__)
	mach_vm_size_t size_of_jump = (sizeof(uintptr_t) * 2);
    #else
	mach_vm_size_t size_of_jump = (sizeof(int) + 1);
    #endif

	mach_msg_type_number_t amount_read = 0;
	kern_return_t kr = mach_vm_protect(mach_task_self(), reinterpret_cast<mach_vm_address_t>(original), size_of_jump, FALSE, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE | VM_PROT_COPY);
	if (kr == KERN_SUCCESS)
	{
	    kr = mach_vm_read(mach_task_self(), reinterpret_cast<mach_vm_address_t>(original), size_of_jump, reinterpret_cast<vm_offset_t*>(&data[0]), &amount_read);
	    kr = mach_vm_protect(mach_task_self(), reinterpret_cast<mach_vm_address_t>(original), size_of_jump, FALSE, VM_PROT_READ | VM_PROT_EXECUTE);
	}
}

Hook::Hook(Hook&& other) : original(nullptr), detour(nullptr), trampoline(nullptr), data()
{
    other.original = nullptr;
    other.detour = nullptr;
    other.trampoline = nullptr;
    std::memset(&other.data[0], 0, sizeof(data));
}

Hook::~Hook()
{
    trampoline = nullptr;
}

void Hook::apply()
{
    rd_route(original, detour, &trampoline);
}

void Hook::remove()
{
	#if defined (__x86_64__)
	mach_msg_type_number_t size_of_jump = (sizeof(uintptr_t) * 2);
	#else
	mach_msg_type_number_t size_of_jump = (sizeof(int) + 1);
	#endif

	kern_return_t kr = mach_vm_protect(mach_task_self(), reinterpret_cast<mach_vm_address_t>(original), size_of_jump, FALSE, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE | VM_PROT_COPY);
	if (kr == KERN_SUCCESS)
	{
		kr = mach_vm_write(mach_task_self(), reinterpret_cast<mach_vm_address_t>(original), reinterpret_cast<vm_offset_t>(&data[0]), size_of_jump);
		kr = mach_vm_protect(mach_task_self(), reinterpret_cast<mach_vm_address_t>(original), size_of_jump, FALSE, VM_PROT_READ | VM_PROT_EXECUTE);
	}

	std::memset(&data[0], 0, sizeof(data));
}
#else

Hook::Hook(void *original, void *detour) : original(original), detour(detour), trampoline(new HOOK_TRACE_INFO()), data()
{
    std::memset(&data[0], 0, sizeof(data));
}

Hook::Hook(Hook&& other) : original(other.original), detour(other.detour), trampoline(other.trampoline), data()
{
    other.original = nullptr;
    other.detour = nullptr;
    other.trampoline = nullptr;
    std::memset(&other.data[0], 0, sizeof(data));
}

Hook::~Hook()
{
    trampoline = nullptr;
}

void Hook::apply()
{
    LONG callback = 0;
    ULONG acl_list = 0;
    DetourInstallHook(original, detour, &callback, reinterpret_cast<HOOK_TRACE_INFO*>(&data[0]));
    DetourSetExclusiveACL(&acl_list, 1, reinterpret_cast<HOOK_TRACE_INFO*>(&data[0]));
}

void Hook::remove()
{
    DetourUninstallHook(reinterpret_cast<HOOK_TRACE_INFO*>(&data[0]));
}
#endif


#if defined(_WIN32) || defined(_WIN64)
bool Injector::Inject(std::string module_path, pid_t pid, void* bootstrap)
{
    auto IsProcessAlive = [](pid_t pid) -> bool {
        HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
        if (process)
        {
            DWORD ret = WaitForSingleObject(process, 0);
            CloseHandle(process);
            return ret == WAIT_TIMEOUT;
        }
        return false;
    };

    if (IsProcessAlive(pid))
    {
        HMODULE hKernel32 = nullptr;
        void* RemoteAddress = nullptr;
        HANDLE ProcessHandle, hThread = nullptr;
        LPTHREAD_START_ROUTINE LoadLibraryHandle = nullptr;

        if ((ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid)))
        {
            if ((hKernel32 = GetModuleHandle("Kernel32.dll")))
            {
                LoadLibraryHandle = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(hKernel32, "LoadLibraryA"));
                if (LoadLibraryHandle)
                {
                    RemoteAddress = VirtualAllocEx(ProcessHandle, nullptr, module_path.size() * sizeof(char), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                    if (RemoteAddress)
                    {
                        if (WriteProcessMemory(ProcessHandle, RemoteAddress, &module_path[0], module_path.size() * sizeof(char), nullptr))
                        {
                            hThread = CreateRemoteThread(ProcessHandle, nullptr, 0, LoadLibraryHandle, RemoteAddress, 0, nullptr);
                            if (hThread)
                            {
                                //More than enough time to wait for a proccess to create a thread..
                                WaitForSingleObject(hThread, 5000);
                                CloseHandle(hThread);

								VirtualFreeEx(ProcessHandle, RemoteAddress, module_path.size() * sizeof(char), MEM_RELEASE);
								CloseHandle(ProcessHandle);
								return true;
                            }
                        }

                        VirtualFreeEx(ProcessHandle, RemoteAddress, module_path.size() * sizeof(char), MEM_RELEASE);
                    }
                }
            }
            CloseHandle(ProcessHandle);
        }
    }
    return false;
}
#elif defined(__APPLE__)
bool Injector::Inject(std::string module_path, pid_t pid, void* bootstrap)
{
    return !mach_inject(reinterpret_cast<mach_inject_entry>(bootstrap), &module_path[0], module_path.size() + 1, pid, 0);
}
#else
bool Injector::Inject(std::string module_path, pid_t pid, void* bootstrap)
{
    #warning No Injection Yet
    return false;
}
#endif
