#include "Platform.hxx"
#include <string>
#include <chrono>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <tlhelp32.h>
#endif // defined

#if defined(_WIN32) || defined(_WIN64)
void GetDesktopResolution(int &width, int &height)
{
    RECT desktop = {0};
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    width = desktop.right;
    height = desktop.bottom;
}

std::int32_t GetCurrentThreadID()
{
	return GetCurrentThreadId();
}

bool IsProcessAlive(pid_t pid)
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

bool IsThreadAlive(std::int32_t tid)
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

std::vector<pid_t> get_pids()
{
	std::vector<pid_t> result;
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

std::vector<pid_t> get_pids(const char* process_name)
{
	std::vector<pid_t> result;
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

PROCESSENTRY32 GetProcessInfo(pid_t pid)
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

MODULEENTRY32 GetModuleInfo(pid_t pid, const char* module_name)
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

MODULEENTRY32 GetModuleInfo(pid_t pid, HMODULE module_handle)
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

void PrintProcessInfo(pid_t pid)
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

bool InjectSelf(pid_t pid)
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

        HMODULE hKernel32 = nullptr;
        void* RemoteAddress = nullptr;
        HANDLE ProcessHandle, hThread = nullptr;
        LPTHREAD_START_ROUTINE LoadLibraryHandle = nullptr;

        if ((ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid)))
        {
            if ((hKernel32 = GetModuleHandle("Kernel32.dll")))
            {
                PrintProcessInfo(pid);

                LoadLibraryHandle = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(hKernel32, "LoadLibraryA"));
                if (LoadLibraryHandle)
                {
                    RemoteAddress = VirtualAllocEx(ProcessHandle, nullptr, File.size() * sizeof(char), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                    if (RemoteAddress)
                    {
                        if (WriteProcessMemory(ProcessHandle, RemoteAddress, &File[0], File.size() * sizeof(char), nullptr))
                        {
                            hThread = CreateRemoteThread(ProcessHandle, nullptr, 0, LoadLibraryHandle, RemoteAddress, 0, nullptr);
                            if (hThread)
                            {
                                //More than enough time to wait for a proccess to create a thread..
                                WaitForSingleObject(hThread, 5000);
                                CloseHandle(hThread);
                            }

                            VirtualFreeEx(ProcessHandle, RemoteAddress, File.size() * sizeof(char), MEM_RELEASE);
                            CloseHandle(ProcessHandle);
                            return true;
                        }

                        VirtualFreeEx(ProcessHandle, RemoteAddress, File.size() * sizeof(char), MEM_RELEASE);
                    }
                }
            }
            CloseHandle(ProcessHandle);
        }
    }
    return false;
}

void InjectProcesses(const char* process_name)
{
    extern HMODULE module;

    std::vector<pid_t> pids = get_pids(process_name);
    for (pid_t pid : pids)
    {
        MODULEENTRY32 info = GetModuleInfo(pid, module);
        if (info.dwSize == 0)
        {
            InjectSelf(pid);
        }
    }
}

pid_t PIDFromWindow(void* window)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(static_cast<HWND>(window), &pid);
    return static_cast<pid_t>(pid);
}
#endif // defined

#if defined(_WIN32) || defined(_WIN64)
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
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

Reflection* GetNativeReflector()
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
		return reflection->GetClassType(object) == "java.awt.Frame";
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

    auto DSGetComponent = reinterpret_cast<jobject __stdcall (*)(JNIEnv*, void*)>(GetProcAddress(awt, "DSGetComponent"));
    if (!DSGetComponent)
    {
        DSGetComponent = reinterpret_cast<jobject __stdcall (*)(JNIEnv*, void*)>(GetProcAddress(awt, "_DSGetComponent@8"));
    }

    if (!DSGetComponent)
    {
        int ordinal = 146;
        char* procName = reinterpret_cast<char*>(static_cast<std::uint64_t>(static_cast<std::uint16_t>(ordinal)));
        DSGetComponent = reinterpret_cast<jobject __stdcall (*)(JNIEnv*, void*)>(GetProcAddress(awt, procName));
    }

    if (!DSGetComponent)
    {
        return nullptr;
    }

    Reflection* reflection = new Reflection();

    if (reflection->Attach())
    {
		auto hasReflection = TimeOut(20, [&]{
			HWND windowFrame = GetMainWindow();
			if (windowFrame)
			{
				jobject object = DSGetComponent(reflection->getEnv(), windowFrame);  //java.awt.Frame
				return IsValidFrame(reflection, object) && reflection->Initialize(object);
			}
			return false;
		});

		if (hasReflection)
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
