#include "SimbaPlugin.hxx"
#include <memory>
#include <cstring>
#include <unordered_map>

#include "ControlCenter.hxx"

#if defined(_WIN32) || defined(_WIN64)
extern HMODULE module;
#endif

TMemoryManager PLUGIN_MEMORY_MANAGER = {0};
TSimbaMethods PLUGIN_SYNC_METHODS = {0};
TSimbaMemoryAllocators PLUGIN_MEMORY_ALLOCATORS = {0};
extern std::unique_ptr<ControlCenter> control_center;


// MARK: - DECLARATIONS

template<typename T>
int PascalHigh(T* Arr);

template<typename T>
int PascalLength(T* Arr);

template<typename T>
T* AllocateArray(std::size_t size, std::size_t element_size = sizeof(T));

template<typename T, bool isFPC3 = false>
T* AllocateString(std::size_t size, std::size_t element_size = sizeof(T));


// MARK: - EXPORTS

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

void SetPluginSimbaMethods(TSimbaMethods Methods)
{
	PLUGIN_SYNC_METHODS = Methods;
}

void SetPluginSimbaMemoryAllocators(TSimbaMemoryAllocators Allocators)
{
	PLUGIN_MEMORY_ALLOCATORS = Allocators;
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


void Reflect_Pascal_String(void** Params, void** Result)
{
	EIOS* eios = static_cast<EIOS*>(Params[0]);
	jobject object = static_cast<jobject>(Params[1]);
	const char* cls = static_cast<const char*>(Params[2]);
	const char* field = static_cast<const char*>(Params[3]);
	const char* desc = static_cast<const char*>(Params[4]);

	if (eios)
	{
		ReflectionHook hook{object, cls, field, desc};
		std::string result = eios->control_center->reflect_string(hook);

		char* output = AllocateString<char>(result.length());
		std::memcpy(output, &result[0], result.length());
		output[result.length() - 1] = '\0';
		*Result = output;
	}
}

std::uint8_t* Pascal_GetDebugImageBuffer(EIOS* eios)
{
    if (eios)
    {
		return eios->control_center->get_debug_image();
    }
    return nullptr;
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
T* AllocateArray(std::size_t size, std::size_t element_size)
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

template<typename T, bool isFPC3>
T* AllocateString(std::size_t size, std::size_t element_size)
{
    if constexpr (isFPC3)
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
