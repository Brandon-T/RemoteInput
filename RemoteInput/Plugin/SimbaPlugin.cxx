#include "SimbaPlugin.hxx"
#include <memory>
#include <atomic>

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
extern "C" EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            module = hinstDLL;
            DisableThreadLibraryCalls(hinstDLL);

			control_center = std::make_unique<ControlCenter>(getpid(), false, std::unique_ptr<Reflection>(GetNativeReflector()));
			if (control_center && control_center->hasReflector())
			{
				StartHook();
			}

			/*globalLock->lock();
			ImageData* info = reinterpret_cast<ImageData*>(globalMap->data());
			info->parentId = -1;
			info->width = 765;
			info->height = 553;
			info->imgoff = sizeof(ImageData);
			globalLock->unlock();*/
        }
            break;

        case DLL_PROCESS_DETACH:
        {
			control_center.reset();
        }
            break;

        case DLL_THREAD_ATTACH:
            printf("ATTACHED\n");
            break;

        case DLL_THREAD_DETACH:
            printf("DETACHED\n");
            break;
    }
    return TRUE; // succesful
}
#elif defined(__APPLE__)
[[gnu::constructor]] void __load()
{
	control_center = std::make_unique<ControlCenter>(getpid(), false, std::unique_ptr<Reflection>(GetNativeReflector()));
	if (control_center && control_center->hasReflector() && dlopen("libawt_lwawt.dylib", RTLD_NOLOAD))
	{
		StartHook();
	}
}

[[gnu::destructor]] void __unload()
{
	control_center.reset();
}
#else
[[gnu::constructor]] void __load()
{
	control_center = std::make_unique<ControlCenter>(getpid(), false, std::unique_ptr<Reflection>(GetNativeReflector()));
	if (control_center && control_center->hasReflector() && dlopen("libawt_lwawt.so", RTLD_NOLOAD))
	{
		StartHook();
	}
}

[[gnu::destructor]] void __unload()
{
	control_center.reset();
}
#endif // defined
