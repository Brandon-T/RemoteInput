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

jobject Reflect_Object(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc)
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_object(hook);
	}
	return nullptr;
}

jboolean Reflect_IsSame_Object(EIOS* eios, jobject first, jobject second)
{
    if (eios)
    {
        return eios->control_center->reflect_is_objects_equal(first, second);
    }
    return false;
}

jboolean Reflect_InstanceOf(EIOS* eios, jobject object, const char* cls)
{
	if (eios)
	{
		return eios->control_center->reflect_instance_of(object, cls);
	}
	return false;
}

void Reflect_Release_Object(EIOS* eios, jobject object)
{
	if (eios)
	{
		eios->control_center->reflect_release_object(object);
	}
}

void Reflect_Release_Objects(EIOS* eios, jobject* objects, std::size_t amount)
{
	if (eios)
	{
		eios->control_center->reflect_release_objects(objects, amount);
	}
}

char Reflect_Char(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc)
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_char(hook);
	}
	return '\0';
}

std::uint8_t Reflect_Byte(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc)
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_byte(hook);
	}
	return 0;
}

std::int16_t Reflect_Short(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc)
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_short(hook);
	}
	return -1;
}

std::int32_t Reflect_Int(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc)
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_int(hook);
	}
	return -1;
}

std::int64_t Reflect_Long(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc)
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_long(hook);
	}
	return -1;
}

float Reflect_Float(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc)
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_float(hook);
	}
	return -1.0;
}

double Reflect_Double(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc)
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_double(hook);
	}
	return -1.0;
}

void Reflect_String(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc, char* output, std::size_t output_size)
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		std::string result = eios->control_center->reflect_string(hook);
		std::size_t length = std::min(result.length(), output_size);
		std::memcpy(output, &result[0], length);
	}
}

jarray Reflect_Array(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc)
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_array(hook);
	}
	return nullptr;
}

jarray Reflect_Array_With_Size(EIOS* eios, jobject object, std::size_t* output_size, const char* cls, const char* field, const char* desc)
{
    if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_array_with_size(hook, output_size);
	}
	return nullptr;
}

std::size_t Reflect_Array_Size(EIOS* eios, jarray array)
{
	if (eios)
	{
		return eios->control_center->reflect_array_size(array);
	}
	return 0;
}

void Reflect_Array_Index(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t index, std::size_t length, void* buffer)
{
	if (eios)
	{
		void* result = eios->control_center->reflect_array_index(array, type, index, length);
		std::memcpy(buffer, result, ControlCenter::reflect_size_for_type(type) * length);
	}
}

void Reflect_Array_Index2D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, void* buffer)
{
	if (eios)
	{
		void* result = eios->control_center->reflect_array_index2d(array, type, length, x, y);
		std::memcpy(buffer, result, ControlCenter::reflect_size_for_type(type) * length);
	}
}

void Reflect_Array_Index3D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z, void* buffer)
{
	if (eios)
	{
		void* result = eios->control_center->reflect_array_index3d(array, type, length, x, y, z);
		std::memcpy(buffer, result, ControlCenter::reflect_size_for_type(type) * length);
	}
}

void Reflect_Array_Index4D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t w, void* buffer)
{
	if (eios)
	{
		void* result = eios->control_center->reflect_array_index4d(array, type, length, x, y, z, w);
		std::memcpy(buffer, result, ControlCenter::reflect_size_for_type(type) * length);
	}
}


// MARK: - MAIN

#if defined(_WIN32) || defined(_WIN64)
[[gnu::stdcall]] void __load()
{
    printf("ATTACHED TO: %d\n", getpid());

	std::thread([&] {
		auto reflector = std::unique_ptr<Reflection>(GetNativeReflector());
        if (reflector)
        {
            control_center = std::make_unique<ControlCenter>(getpid(), false, std::move(reflector));
        }

		StartHook();
	}).detach();
}

[[gnu::stdcall]] void __unload()
{
    control_center.reset();
}
#elif defined(__APPLE__)
[[gnu::constructor]] void __load()
{
    printf("ATTACHED TO: %d\n", getpid());

	std::thread([&] {
		auto reflector = std::unique_ptr<Reflection>(GetNativeReflector());
        if (reflector)
        {
            control_center = std::make_unique<ControlCenter>(getpid(), false, std::move(reflector));
        }

		StartHook();
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

	std::thread([&] {
		auto reflector = std::unique_ptr<Reflection>(GetNativeReflector());
        if (reflector)
        {
            control_center = std::make_unique<ControlCenter>(getpid(), false, std::move(reflector));
        }

		StartHook();
	}).detach();
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
