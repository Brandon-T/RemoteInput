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
#include <cstring>
#include <unordered_map>
#include <unordered_set>

#include "MemoryMap.hxx"
#include "SharedEvent.hxx"
#include "Platform.hxx"
#include "NativeHooks.hxx"
#include "ControlCenter.hxx"

#if defined(_WIN32) || defined(_WIN64)
HMODULE module = nullptr;
#endif

std::unique_ptr<ControlCenter> control_center;

// MARK: - EXPORTS

void EIOS_Inject(const char* process_name) noexcept
{
    if (process_name)
    {
        std::vector<pid_t> result = InjectProcesses(process_name);
		for (pid_t pid : result)
		{
			ControlCenter::wait_for_sync(pid);
		}
    }
}

void EIOS_Inject_PID(std::int32_t pid) noexcept
{
    if (pid != -1)
    {
        if (InjectProcess(pid) != -1)
		{
			ControlCenter::wait_for_sync(pid);
		}
    }
}

EIOS* Reflect_GetEIOS(std::int32_t pid) noexcept
{
	extern std::unordered_map<std::int32_t, EIOS*> clients;
	if (clients.count(pid))
	{
		return clients[pid];
	}
	return nullptr;
}

jobject Reflect_Object(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_object(hook);
	}
	return nullptr;
}

jboolean Reflect_IsSame_Object(EIOS* eios, jobject first, jobject second) noexcept
{
    if (eios)
    {
        return eios->control_center->reflect_is_objects_equal(first, second);
    }
    return false;
}

jboolean Reflect_InstanceOf(EIOS* eios, jobject object, const char* cls) noexcept
{
	if (eios)
	{
		return eios->control_center->reflect_instance_of(object, cls);
	}
	return false;
}

void Reflect_Release_Object(EIOS* eios, jobject object) noexcept
{
	if (eios)
	{
	    //Make sure we're not freeing null objects..
	    if (object)
	    {
            eios->control_center->reflect_release_object(object);
        }
	}
}

void Reflect_Release_Objects(EIOS* eios, jobject* objects, std::size_t amount) noexcept
{
	if (eios)
	{
        //Make sure we're not freeing null objects..
	    if (objects && amount > 0)
        {
            //This is the fastest possible way to remove duplicates..
            //Do NOT use `unordered_set` constructor instead of the for-loop.
            //It is slower than emplace/insert.
            std::unordered_set<jobject> set;
            for (std::size_t i = 0; i < amount; ++i)
            {
                set.insert(objects[i]);
            }

            //Remove null objects
            set.erase(nullptr);

            //Create a contiguous array of objects for the client.
            std::vector<jobject> objects;
            objects.assign(set.begin(), set.end());

            //Make sure we're not freeing null objects..
            if (objects.size())
            {
                eios->control_center->reflect_release_objects(objects.data(), objects.size());
            }
        }
	}
}

bool Reflect_Bool(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
    if (eios)
    {
        ReflectionHook hook{object, cls, field, desc};
        return eios->control_center->reflect_boolean(hook);
    }
    return false;
}

char Reflect_Char(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_char(hook);
	}
	return '\0';
}

std::uint8_t Reflect_Byte(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_byte(hook);
	}
	return 0;
}

std::int16_t Reflect_Short(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_short(hook);
	}
	return -1;
}

std::int32_t Reflect_Int(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_int(hook);
	}
	return -1;
}

std::int64_t Reflect_Long(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_long(hook);
	}
	return -1;
}

float Reflect_Float(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_float(hook);
	}
	return -1.0;
}

double Reflect_Double(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_double(hook);
	}
	return -1.0;
}

void Reflect_String(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc, char* output, std::size_t output_size) noexcept
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		std::string result = eios->control_center->reflect_string(hook);
		std::size_t length = std::min(result.length(), output_size);
		std::memcpy(output, &result[0], length);
	}
}

jarray Reflect_Array(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_array(hook);
	}
	return nullptr;
}

jarray Reflect_Array_With_Size(EIOS* eios, jobject object, std::size_t* output_size, const char* cls, const char* field, const char* desc) noexcept
{
    if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_array_with_size(hook, output_size);
	}
	return nullptr;
}

std::size_t Reflect_Array_Size(EIOS* eios, jarray array) noexcept
{
	if (eios)
	{
		return eios->control_center->reflect_array_size(array);
	}
	return 0;
}

void* Reflect_Array_Index(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t index, std::size_t length) noexcept
{
	if (eios)
	{
		return eios->control_center->reflect_array_index(array, type, index, length);
	}
	return nullptr;
}

void* Reflect_Array_Index2D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y) noexcept
{
	if (eios)
	{
		return eios->control_center->reflect_array_index2d(array, type, length, x, y);
	}
	return nullptr;
}

void* Reflect_Array_Index3D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z) noexcept
{
	if (eios)
	{
		return eios->control_center->reflect_array_index3d(array, type, length, x, y, z);
	}
	return nullptr;
}

void* Reflect_Array_Index4D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t w) noexcept
{
	if (eios)
	{
		return eios->control_center->reflect_array_index4d(array, type, length, x, y, z, w);
	}
	return nullptr;
}

void* Reflect_Array_Indices(EIOS* eios, jarray array, ReflectionArrayType type, std::int32_t* indices, std::size_t length) noexcept
{
    if (eios)
    {
        return eios->control_center->reflect_array_indices(array, type, indices, length);
    }
    return nullptr;
}


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
		//So if `FreeLibrary` was called previous, our count reaches 0 and we'll be fred.
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
