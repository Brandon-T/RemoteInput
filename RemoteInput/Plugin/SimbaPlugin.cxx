#include "SimbaPlugin.hxx"
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

TMemoryManager PLUGIN_MEMORY_MANAGER = {0};
std::unique_ptr<ControlCenter> control_center;

int GetPluginABIVersion()
{
    return 2;
}

int GetFunctionCount()
{
    return PascalExportCount;
}

int GetTypeCount()
{
	return PascalTypeCount;
}

int GetFunctionInfo(int Index, void** Address, char** Definition)
{
    if (Index < PascalExportCount)
    {
        #if defined(_WIN32) || defined(_WIN64)
        *Address = (void*)GetProcAddress(module, PascalExports[Index * 2]);
        #else
        *Address = (void*)dlsym(RTLD_DEFAULT, PascalExports[Index * 2]);
        #endif
        strcpy(*Definition, PascalExports[Index * 2 + 1]);
        return Index;
    }
    return -1;
}

int GetTypeInfo(int Index, char** Type, char** Definition)
{
	if (Index < PascalTypeCount)
	{
		strcpy(*Type, PascalTypes[Index * 2 + 0]);
		strcpy(*Definition, PascalTypes[Index * 2 + 1]);
		return Index;
	}
	return -1;
}

void SetPluginMemManager(TMemoryManager MemMgr)
{
    PLUGIN_MEMORY_MANAGER = MemMgr;
}

void OnAttach(void* info)
{
    if (control_center)
	{
		control_center.reset();
	}
}

void OnDetach()
{
	control_center.reset();
}

EIOS* Reflect_GetEIOS(std::int32_t pid)
{
    extern std::unordered_map<std::int32_t, EIOS*> clients;
    if (clients.count(pid))
    {
        return clients[pid];
    }
    return nullptr;
}

jobject Reflect_Object(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc)
{
	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		return eios->control_center->reflect_object(hook);
	}
	return nullptr;
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




template<typename T>
int PascalHigh(T* Arr)
{
    return reinterpret_cast<int*>(Arr)[-1];
}

template<typename T>
int PascalLength(T* Arr)
{
    return PascalHigh<T>(Arr) + 1;
}

template<typename T>
T* AllocateArray(std::size_t size, std::size_t element_size = sizeof(T))
{
    typedef struct
    {
        std::int32_t refCount;
        std::int32_t length;
        char data[];
    } FPCArray;

    std::size_t new_size = (size * element_size) + sizeof(FPCArray);
    FPCArray* ptr = static_cast<FPCArray*>(PLUGIN_MEMORY_MANAGER.AllocMem(new_size));
    ptr->refCount = 1;
    ptr->length = size - 1;
    return reinterpret_cast<T*>(++ptr);
}

template<typename T, bool isFPC3 = false>
T* AllocateString(std::size_t size, std::size_t element_size = sizeof(T))
{
    if (isFPC3)
    {
        typedef struct
        {
            std::uint16_t codePage;
            std::uint16_t elementSize;
            #if defined(__x86_64__)
            std::uint32_t dummy;
            #endif
            std::int32_t refCount;
            std::int32_t length;
            char data[];
        } __attribute__((__packed__)) FPCAnsiString;

        std::size_t new_size = (size * element_size) + sizeof(FPCAnsiString);
        FPCAnsiString* ptr = static_cast<FPCAnsiString*>(PLUGIN_MEMORY_MANAGER.AllocMem(new_size + 1));
        ptr->codePage = 0; //CP_ACP
        ptr->elementSize = element_size;
        ptr->refCount = 1;
        ptr->length = size;
        return reinterpret_cast<T*>(++ptr);
    }

    typedef struct
    {
        std::int32_t refCount;
        std::int32_t length;
        char data[];
    } __attribute__((__packed__)) FPCAnsiString;

    std::size_t new_size = (size * element_size) + sizeof(FPCAnsiString);
    FPCAnsiString* ptr = static_cast<FPCAnsiString*>(PLUGIN_MEMORY_MANAGER.AllocMem(new_size + 1));
    ptr->refCount = 1;
    ptr->length = size;
    return reinterpret_cast<T*>(++ptr);
}

#if defined(_WIN32) || defined(_WIN64)
[[gnu::stdcall]] void __load()
{
    printf("ATTACHED TO: %d\n", getpid());

    std::thread([&]{
        auto reflector = std::unique_ptr<Reflection>(GetNativeReflector());
        if (reflector)
        {
            control_center = std::make_unique<ControlCenter>(getpid(), false, std::move(reflector));
            if (control_center && control_center->hasReflector())
            {
                StartHook();

                /*globalLock->lock();
                ImageData* info = reinterpret_cast<ImageData*>(globalMap->data());
                info->parentId = -1;
                info->width = 765;
                info->height = 553;
                info->imgoff = sizeof(ImageData);
                globalLock->unlock();*/
            }
        }
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

    std::thread([&]{
		auto reflector = std::unique_ptr<Reflection>(GetNativeReflector());
        if (reflector)
        {
            control_center = std::make_unique<ControlCenter>(getpid(), false, std::move(reflector));
			if (control_center && control_center->hasReflector() && dlopen("libawt_lwawt.so", RTLD_NOLOAD))
			{
				StartHook();
			}
        }
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
