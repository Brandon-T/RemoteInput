#include "SimbaPlugin.hxx"
#include <memory>
#include <atomic>

#include "../Echo/MemoryMap.hxx"
#include "../Echo/SharedEvent.hxx"
#include "../Platform/Platform.hxx"
#include "../Platform/NativeHooks.hxx"
#include "CommandThread.hxx"

#if defined(_WIN32) || defined(_WIN64)
HMODULE module = nullptr;
#endif

TMemoryManager PLUGIN_MEMORY_MANAGER = {0};
extern std::unique_ptr<MemoryMap<char>> globalMap;
extern std::unique_ptr<Mutex> globalLock;
extern std::unique_ptr<Semaphore> commandSignal;
extern std::unique_ptr<Semaphore> responseSignal;
extern std::unique_ptr<Reflection> globalReflector;
std::unique_ptr<ResponsiveThread> commandThread;

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
    //commandThread.reset();
}

void OnDetach()
{
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

bool InitializeSharedMemory(pid_t pid)
{
    int width = 0;
    int height = 0;
    GetDesktopResolution(width, height); //TODO: Change to Target Window size..

    int image_size = width * height * 4 * sizeof(std::uint8_t);

    char mapName[256] = {0};
    std::int32_t map_size = (1024 * sizeof(std::int32_t)) + image_size;
    sprintf(mapName, "Local\\RemoteInput_%d", pid);

    // Open first..
    globalMap.reset(new MemoryMap<char>{mapName, std::ios::in | std::ios::out});
    if (globalMap->open() && globalMap->map())
    {
        return true;
    }

    // Create otherwise..
    globalMap.reset(new MemoryMap<char>(mapName, map_size, std::ios::in | std::ios::out));
    if (globalMap->open() && globalMap->map())
    {
        return true;
    }

    return false;
}

bool InitializeLocks(pid_t pid)
{
    char lockName[256] = {0};
    sprintf(lockName, "Local\\RemoteInput_Lock_%d", pid);
    globalLock.reset(new Mutex(lockName));
    return true;
}

bool InitializeSignals(pid_t pid)
{
    char signalName[256] = {0};
    sprintf(signalName, "Local\\RemoteInput_EventRead_%d", pid);
    commandSignal.reset(new Semaphore(signalName));

    sprintf(signalName, "Local\\RemoteInput_EventWrite_%d", pid);
    responseSignal.reset(new Semaphore(signalName));
    return true;
}

template<typename T>
T EIOS_Read(void*& ptr)
{
    T result = *static_cast<T*>(ptr);
    ptr = static_cast<T*>(ptr) + 1;
    return result;
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

            if (InitializeSharedMemory(getpid()))
            {
                InitializeLocks(getpid());
                InitializeSignals(getpid());

                globalLock->lock();
                ImageData* info = reinterpret_cast<ImageData*>(globalMap->data());
                info->parentId = -1;
                info->width = 765;
                info->height = 553;
                info->imgoff = sizeof(ImageData);
                globalLock->unlock();
            }

            globalReflector.reset(GetNativeReflector());

            if (globalReflector && GetModuleHandle("awt.dll"))
            {
                StartHook();
            }

            commandThread.reset(new ResponsiveThread([&](ResponsiveThread* thread){
                while(thread && !thread->stopped()) {
                    if (!commandSignal || !globalLock || !responseSignal || !globalMap)
                    {
                        return;
                    }

                    commandSignal->wait();
                    if (!thread || thread->stopped())
                    {
                        return;
                    }

                    globalLock->lock();
                    ImageData* info = reinterpret_cast<ImageData*>(globalMap->data());
                    int command = info->command;
                    void* args = info->args;

                    if (command != EIOS_COMMAND_NONE)
                    {
                        switch(command)
                        {
                        case EIOS_MOVE_MOUSE:
                            std::int32_t x = EIOS_Read<std::int32_t>(args);
                            std::int32_t y = EIOS_Read<std::int32_t>(args);

                            char buff[48];
                            sprintf(buff, "(%d, %d)", x, y);
                            MessageBox(NULL, buff, NULL, 0);
                            break;
                        }
                    }
                    globalLock->unlock();
                    responseSignal->signal();
                }
            }, [&]{
                if (commandSignal)
                {
                    commandSignal->signal();
                }
            }));
        }
            break;

        case DLL_PROCESS_DETACH:
        {
            commandThread.reset();
            globalReflector.reset();

            if (globalLock)
            {
                globalLock->unlock();
            }
            globalLock.reset();

            if (globalMap)
            {
                globalMap->unmap();
                globalMap->close();
            }
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
#else
[[gnu::constructor]] void __load()
{
    if (InitializeSharedMemory(getpid()))
    {
        InitializeLocks(getpid());
        InitializeSignals(getpid());
    }
    globalReflector.reset(GetNativeReflector());
}

[[gnu::destructor]] void __unload()
{
    globalReflector.reset();

    globalLock->unlock();
    globalLock.reset();

    globalMap->unmap();
    globalMap->close();
}
#endif // defined
