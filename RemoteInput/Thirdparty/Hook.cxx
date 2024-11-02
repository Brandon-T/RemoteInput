//
// Created by brandon on 2020-04-06.
//

#include "Hook.hxx"
#include <cassert>
#include <cstring>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <MinHook.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <mach-o/loader.h>
#import <mach-o/nlist.h>
#include <rd_route.h>
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

Hook::Hook(Hook&& other) noexcept : original(other.original), detour(other.detour), trampoline(other.trampoline), data()
{
    other.original = nullptr;
    other.detour = nullptr;
    other.trampoline = nullptr;
    std::memcpy(&data[0], &other.data[0], sizeof(data));
    std::memset(&other.data[0], 0, sizeof(data));
}

Hook::~Hook()
{
    remove();
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

Hook::Hook(Hook&& other) noexcept : original(other.original), detour(other.detour), trampoline(other.trampoline), data()
{
    other.original = nullptr;
    other.detour = nullptr;
    other.trampoline = nullptr;
    std::memcpy(&data[0], &other.data[0], sizeof(data));
    std::memset(&other.data[0], 0, sizeof(data));
}

Hook::~Hook()
{
    remove();
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

Hook::Hook(Hook&& other)  noexcept : original(other.original), detour(other.detour), trampoline(other.trampoline), data()
{
    other.original = nullptr;
    other.detour = nullptr;
    other.trampoline = nullptr;
    std::memcpy(&data[0], &other.data[0], sizeof(data));
    std::memset(&other.data[0], 0, sizeof(data));
}

Hook::~Hook()
{
    remove();
    delete static_cast<HOOK_TRACE_INFO*>(trampoline);
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
    DetourUninstallHook(reinterpret_cast<HOOK_TRACE_INFO*>(trampoline));
}
#endif
