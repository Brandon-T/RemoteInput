#include "Platform.hxx"
#include <string>
#include <chrono>
#include "Thirdparty/Hook.hxx"
#include "Injection/Injector.hxx"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <tlhelp32.h>
#endif // defined

#if defined(_WIN32) || defined(_WIN64)
int GetDpi(HWND hwnd)
{
    HDC hDC = GetDC(hwnd);
    int ydpi = GetDeviceCaps(hDC, LOGPIXELSY);
    ReleaseDC(hwnd, hDC);
    return ydpi;
}

void ReverseScaleDPI(HWND hwnd, int &width, int &height)
{
    HDC hDC = GetDC(hwnd);
    int xdpi = GetDeviceCaps(hDC, LOGPIXELSX);
    int ydpi = GetDeviceCaps(hDC, LOGPIXELSY);
    ReleaseDC(hwnd, hDC);

    // Reverse DPI Scaling
    width = static_cast<int>(96.0 * width / xdpi);
    height = static_cast<int>(96.0 * height / ydpi);
}

void GetDesktopResolution(int &width, int &height) noexcept
{
    HMODULE user32 = GetModuleHandleA("User32.dll");
    DPI_AWARENESS_CONTEXT (*GetThreadDpiAwarenessContext)() = reinterpret_cast<decltype(GetThreadDpiAwarenessContext)>(GetProcAddress(user32, "GetThreadDpiAwarenessContext"));
    DPI_AWARENESS (*GetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = reinterpret_cast<decltype(GetAwarenessFromDpiAwarenessContext)>(GetProcAddress(user32, "GetAwarenessFromDpiAwarenessContext"));
    BOOL (*SetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = reinterpret_cast<decltype(SetThreadDpiAwarenessContext)>(GetProcAddress(user32, "SetThreadDpiAwarenessContext"));

    DPI_AWARENESS_CONTEXT context = nullptr;

    if (GetThreadDpiAwarenessContext && GetAwarenessFromDpiAwarenessContext && SetThreadDpiAwarenessContext)
    {
        context = GetThreadDpiAwarenessContext();
        DPI_AWARENESS awareness = GetAwarenessFromDpiAwarenessContext(context);

        if (awareness == DPI_AWARENESS_INVALID || awareness == DPI_AWARENESS_UNAWARE)
        {
            SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        }
    }

    RECT desktop = {0};
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);

    width = desktop.right;
    height = desktop.bottom;

    if (context)
    {
        SetThreadDpiAwarenessContext(context);
    }
}

std::int32_t GetCurrentThreadID() noexcept
{
    return GetCurrentThreadId();
}

bool IsProcessAlive(std::int32_t pid) noexcept
{
    HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if (process)
    {
        DWORD ret = WaitForSingleObject(process, 0);
        CloseHandle(process);
        return ret == WAIT_TIMEOUT;
    }
    return false;
}

bool IsThreadAlive(std::int32_t tid) noexcept
{
    HANDLE thread = OpenThread(SYNCHRONIZE, FALSE, tid);
    if (thread)
    {
        DWORD ret = WaitForSingleObject(thread, 0);
        CloseHandle(thread);
        return ret == WAIT_TIMEOUT;
    }
    return false;
}

std::vector<std::int32_t> get_pids() noexcept
{
    std::vector<std::int32_t> result;
    PROCESSENTRY32 processInfo = {0};
    processInfo.dwSize = sizeof(processInfo);

    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (processesSnapshot != INVALID_HANDLE_VALUE)
    {
        if (Process32First(processesSnapshot, &processInfo))
        {
            result.push_back(processInfo.th32ProcessID);

            while (Process32Next(processesSnapshot, &processInfo))
            {
                result.push_back(processInfo.th32ProcessID);
            }
        }
        CloseHandle(processesSnapshot);
    }
    return result;
}

std::vector<std::int32_t> get_pids(const char* process_name) noexcept
{
    std::vector<std::int32_t> result;
    PROCESSENTRY32 processInfo = {0};
    processInfo.dwSize = sizeof(processInfo);

    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (processesSnapshot != INVALID_HANDLE_VALUE)
    {
        if (Process32First(processesSnapshot, &processInfo))
        {
            if (!_stricmp(process_name, processInfo.szExeFile))
            {
                result.push_back(processInfo.th32ProcessID);
            }

            while (Process32Next(processesSnapshot, &processInfo))
            {
                if (!_stricmp(process_name, processInfo.szExeFile))
                {
                    result.push_back(processInfo.th32ProcessID);
                }
            }
        }

        CloseHandle(processesSnapshot);
    }
    return result;
}

PROCESSENTRY32 GetProcessInfo(std::int32_t pid) noexcept
{
    HANDLE processesSnapshot = nullptr;
    if((processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE)
    {
        return {0};
    }

    PROCESSENTRY32 processInfo = {0};
    processInfo.dwSize = sizeof(PROCESSENTRY32);
    while(Process32Next(processesSnapshot, &processInfo))
    {
        if(processInfo.th32ProcessID == static_cast<DWORD>(pid))
        {
            CloseHandle(processesSnapshot);
            return processInfo;
        }
    }
    CloseHandle(processesSnapshot);
    return {0};
}

MODULEENTRY32 GetModuleInfo(std::int32_t pid, const char* module_name) noexcept
{
    HANDLE modulesSnapshot = nullptr;
    if ((modulesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid)) == INVALID_HANDLE_VALUE)
    {
        return {0};
    }

    MODULEENTRY32 moduleInfo = {0};
    moduleInfo.dwSize = sizeof(MODULEENTRY32);
    while (Module32Next(modulesSnapshot, &moduleInfo))
    {
        if (!_stricmp(module_name, moduleInfo.szModule))
        {
            CloseHandle(modulesSnapshot);
            return moduleInfo;
        }
    }

    CloseHandle(modulesSnapshot);
    return {0};
}

MODULEENTRY32 GetModuleInfo(std::int32_t pid, HMODULE module_handle) noexcept
{
    HANDLE modulesSnapshot = nullptr;
    if ((modulesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid)) == INVALID_HANDLE_VALUE)
    {
        return {0};
    }

    MODULEENTRY32 moduleInfo = {0};
    moduleInfo.dwSize = sizeof(MODULEENTRY32);
    while (Module32Next(modulesSnapshot, &moduleInfo))
    {
        if (module_handle == moduleInfo.hModule)
        {
            CloseHandle(modulesSnapshot);
            return moduleInfo;
        }
    }

    CloseHandle(modulesSnapshot);
    return {0};
}

void PrintProcessInfo(std::int32_t pid) noexcept
{
    PROCESSENTRY32 Proc32 = GetProcessInfo(pid);
    if (Proc32.th32ProcessID != 0)
    {
        printf("  =======================================================\n");
        printf("  Process Name:            %s\n", Proc32.szExeFile);
        printf("  =======================================================\n\n");
        printf("  Process ID:              %lu\n", Proc32.th32ProcessID);
        printf("  Thread Count:            %lu\n", Proc32.cntThreads);
        printf("  Priority Base:           %lu\n", Proc32.pcPriClassBase);
        printf("  Parent Process ID:       %lu\n\n", Proc32.th32ParentProcessID);
        printf("  =======================================================\n");
    }
}

bool InjectSelf(std::int32_t pid) noexcept
{
    if (IsProcessAlive(pid))
    {
        std::string File;
        File.resize(MAX_PATH);
        extern HMODULE module;

        if (GetModuleFileName(module, &File[0], MAX_PATH) == 0)
        {
            return false;
        }

        return Injector::Inject(File, pid, nullptr);
    }
    return false;
}

std::int32_t InjectProcess(std::int32_t pid) noexcept
{
    extern HMODULE module;
    MODULEENTRY32 info = GetModuleInfo(pid, module);
    if (info.dwSize == 0)
    {
        if (InjectSelf(pid))
        {
            return pid;
        }
    }
    return -1;
}

std::vector<std::int32_t> InjectProcesses(const char* process_name) noexcept
{
    std::vector<std::int32_t> result;
    std::vector<std::int32_t> pids = get_pids(process_name);
    for (std::int32_t pid : pids)
    {
        if (InjectProcess(pid) != -1)
        {
            result.push_back(pid);
        }
    }
    return result;
}

std::int32_t PIDFromWindow(void* window) noexcept
{
    DWORD pid = 0;
    GetWindowThreadProcessId(static_cast<HWND>(window), &pid);
    return static_cast<std::int32_t>(pid);
}
#endif // defined

#if defined(_WIN32) || defined(_WIN64)
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) noexcept
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId() && lParam)
    {
        char className[MAX_PATH] = {0};
        int result = GetClassName(hwnd, className, sizeof(className));
        if (result != 0 && std::string(className) == "SunAwtFrame")
        {
            *reinterpret_cast<HWND*>(lParam) = hwnd;
            return FALSE;
        }
    }
    return TRUE;
}

Reflection* GetNativeReflector() noexcept
{
    auto TimeOut = [&](std::uint32_t time, std::function<bool()> &&run) -> bool {
        auto start = std::chrono::high_resolution_clock::now();
        while(!run())
        {
            if (elapsed_time<std::chrono::seconds>(start) >= time)
            {
                return false;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        return true;
    };

    auto IsValidFrame = [&](Reflection* reflection, jobject object) -> bool {
        return reflection->IsDecendentOf(object, "java/awt/Frame");
    };

    auto GetMainWindow = [&] {
        HWND windowFrame = nullptr;
        EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windowFrame));
        return windowFrame;
    };

    HMODULE awt = nullptr;
    if (!TimeOut(20, [&]{ return (awt = GetModuleHandle("awt.dll")); }))
    {
        return nullptr;
    }

    if (!TimeOut(5, [&]{ return JVM().getVM() != nullptr; }))
    {
        return nullptr;
    }

    Reflection* reflection = new Reflection();

    if (reflection->Attach())
    {
        auto hasReflection = TimeOut(20, [&]{
            JNIEnv* env = reflection->getEnv();
            jclass cls = env->FindClass("java/awt/Frame");
            if (!cls)
            {
                return false;
            }

            jmethodID method = env->GetStaticMethodID(cls, "getFrames", "()[Ljava/awt/Frame;");
            if (!method)
            {
                return false;
            }

            jobjectArray frames = static_cast<jobjectArray>(env->CallStaticObjectMethod(cls, method));
            if (!frames)
            {
                return false;
            }

            jsize size = env->GetArrayLength(frames);
            for (jsize i = 0; i < size; ++i)
            {
                jobject frame = env->GetObjectArrayElement(frames, i);
                if (IsValidFrame(reflection, frame) && reflection->Initialize(frame))
                {
                    return true;
                }
            }

            return false;
        });

        auto hasReflection2 = TimeOut(20, [&]{
            auto DSGetComponent = reinterpret_cast<jobject (__stdcall *)(JNIEnv*, void*)>(GetProcAddress(awt, "_DSGetComponent@8"));
            if (!DSGetComponent)
            {
                DSGetComponent = reinterpret_cast<jobject (__stdcall* )(JNIEnv*, void*)>(GetProcAddress(awt, "DSGetComponent"));
            }

            if (!DSGetComponent)
            {
                int ordinal = 146;
                char* procName = reinterpret_cast<char*>(static_cast<std::uint64_t>(static_cast<std::uint16_t>(ordinal)));
                DSGetComponent = reinterpret_cast<jobject (__stdcall *)(JNIEnv*, void*)>(GetProcAddress(awt, procName));
            }

            if (!DSGetComponent)
            {
                return false;
            }

            return TimeOut(20, [&]{
                HWND windowFrame = GetMainWindow();
                if (windowFrame)
                {
                    jobject object = DSGetComponent(reflection->getEnv(), windowFrame);  //java.awt.Frame
                    return IsValidFrame(reflection, object) && reflection->Initialize(object);
                }
                return false;
            });
        });

        if (hasReflection || hasReflection2)
        {
            reflection->Detach();
            return reflection;
        }

        reflection->Detach();
    }

    delete reflection;
    return nullptr;
}
#endif // defined
