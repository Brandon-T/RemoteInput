#include "Platform.hxx"

#if defined(__APPLE__)
#if defined(__cplusplus)
extern "C" {
#endif

#include <Foundation/Foundation.h>
#include <Cocoa/Cocoa.h>

#if defined(__cplusplus)
}
#endif

#include <Thirdparty/Hook.hxx>
#include <Injection/Injector.hxx>
#include <signal.h>
#include <libproc.h>
#include <sys/syscall.h>
#include <mach-o/dyld.h>
#endif // defined

#if defined(__APPLE__)
void GetDesktopResolution(int &width, int &height) noexcept
{
    auto get_screen_resolution = [&]{
        NSRect frame = [[NSScreen mainScreen] frame];
        width = static_cast<int>(frame.size.width);
        height = static_cast<int>(frame.size.height);
    };

    if (!NSThread.isMainThread)
    {
        return dispatch_sync(dispatch_get_main_queue(), ^{
            get_screen_resolution();
        });
    }

    get_screen_resolution();
}

std::int32_t GetCurrentThreadID() noexcept
{
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    return syscall(SYS_thread_selfid);
    #pragma clang diagnostic pop
}

bool IsProcessAlive(std::int32_t pid) noexcept
{
    return !kill(pid, 0);
}

bool IsThreadAlive(std::int32_t tid) noexcept
{
    return !kill(tid, 0);
}

std::vector<std::int32_t> get_pids() noexcept
{
    std::vector<std::int32_t> pids(2048);
    pids.resize(proc_listpids(PROC_ALL_PIDS, 0, &pids[0], 2048 * sizeof(std::int32_t)) / sizeof(std::int32_t));
    std::vector<std::int32_t>(pids).swap(pids);
    return pids;
}

std::vector<std::int32_t> get_pids(const char* process_name) noexcept
{
    std::vector<std::int32_t> result;
    std::vector<std::int32_t> pids = get_pids();

    for (std::size_t i = 0; i < pids.size(); ++i)
    {
        struct proc_bsdinfo proc;
        if (proc_pidinfo(pids[i], PROC_PIDTBSDINFO, 0, &proc, PROC_PIDTBSDINFO_SIZE) == PROC_PIDTBSDINFO_SIZE)
        {
            if (!strcmp(process_name, proc.pbi_name))
            {
                result.push_back(pids[i]);
            }
        }
    }
    return result;
}

std::int32_t InjectProcess(std::int32_t pid) noexcept
{
    Dl_info info = {0};
    if (dladdr(reinterpret_cast<void*>(InjectProcess), &info))
    {
        std::string path = std::string(PATH_MAX, '\0');
        if (realpath(info.dli_fname, &path[0]))
        {
            if (Injector::Inject(info.dli_fname, pid, nullptr))
            {
                return pid;
            }
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
    NSArray *windowList = (NSArray *)CFBridgingRelease(CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID));

    for (int i = 0; i < windowList.count; ++i)
    {
        CGWindowID windowId = [[windowList[i] valueForKey:(NSString *)kCGWindowNumber] intValue];
        if (windowId == (CGWindowID)reinterpret_cast<std::uintptr_t>(window))
        {
            return ((NSNumber *)[windowList[i] valueForKey:(NSString *)kCGWindowOwnerPID]).intValue;
        }
    }
    return 0;
}

void* GetModuleHandle(const char* module_name) noexcept
{
    std::uint32_t count = _dyld_image_count();
    for (std::uint32_t i = 0; i < count; ++i)
    {
        const char* name = _dyld_get_image_name(i);
        if (strcasestr(name, module_name))
        {
            return dlopen(name, RTLD_NOLOAD);

            /*const struct mach_header* header = _dyld_get_image_header(i);
            //std::intptr_t offset = _dyld_get_image_vmaddr_slide(i);

            const struct load_command* cmd = reinterpret_cast<const struct load_command*>(reinterpret_cast<const char *>(header) + sizeof(struct mach_header));
            if (header->magic == MH_MAGIC_64)
            {
                cmd = reinterpret_cast<const struct load_command*>(reinterpret_cast<const char *>(header) + sizeof(struct mach_header_64));
            }

            for (std::uint32_t j = 0; j < header->ncmds; ++j)
            {
                if (cmd->cmd == LC_SEGMENT)
                {
                    const struct segment_command* seg = reinterpret_cast<const struct segment_command*>(cmd);
                    void* base_addr = reinterpret_cast<void*>(seg->vmaddr); //seg->vmaddr + offset;

                    Dl_info info = {0};
                    dladdr(base_addr, &info);
                    return dlopen(info.dli_fname, RTLD_NOLOAD);
                }

                if (cmd->cmd == LC_SEGMENT_64)
                {
                    const struct segment_command_64* seg = reinterpret_cast<const struct segment_command_64*>(cmd);
                    void* base_addr = reinterpret_cast<void*>(seg->vmaddr); //seg->vmaddr + offset;

                    Dl_info info = {0};
                    dladdr(base_addr, &info);
                    return dlopen(info.dli_fname, RTLD_NOLOAD);
                }

                cmd = reinterpret_cast<const struct load_command*>(reinterpret_cast<const char*>(cmd) + cmd->cmdsize);
            }*/
        }
    }
    return dlopen(module_name, RTLD_NOLOAD);
}
#endif // defined


#if defined(__APPLE__)
@interface NSView (MDRecursiveSubviews)
- (jobject)awtComponent:(JNIEnv*)env;
@end

Reflection* GetNativeReflector() noexcept
{
//	auto ModuleLoaded = [](std::string name) -> bool {
//		void* lib = dlopen(name.c_str(), RTLD_GLOBAL | RTLD_NOLOAD);
//		if (lib)
//		{
//			dlclose(lib);
//			return true;
//		}
//		return false;
//	};

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

//	if (!TimeOut(20, [&]{ return ModuleLoaded("libjvm.dylib"); })) {
//		return nullptr;
//	}

    auto IsValidFrame = [&](Reflection* reflection, jobject object) -> bool {
        return reflection->IsDecendentOf(object, "java/awt/Frame");
    };

    auto GetWindowViews = [&]() -> std::vector<NSView*> {
        __block std::vector<NSView*> views;
        dispatch_sync(dispatch_get_main_queue(), ^{
            for (NSWindow* window in NSApplication.sharedApplication.windows)
            {
                //NSLog(@"Window: %@ -- %@", NSStringFromClass([window class]), NSStringFromClass([window.contentView class]));
                if ([window isKindOfClass:NSClassFromString(@"AWTWindow_Normal")] && [window.contentView isKindOfClass:NSClassFromString(@"AWTView")])
                {
                    NSView* view = window.contentView;
                    views.push_back(view);
                }
            }
        });
        return views;
    };

    if (!TimeOut(5, [&]{ return JVM().getVM() != nullptr; }))
    {
        return nullptr;
    }

    Reflection* reflection = new Reflection();

    if (reflection->Attach())
    {
        auto hasReflection = TimeOut(20, [&]{
            for (auto&& view : GetWindowViews())
            {
                //TODO: Check if we can call "awt_GetComponent" from the JDK like on Linux
                jobject object = [view awtComponent:reflection->getEnv()];  //java.awt.Frame
                if (IsValidFrame(reflection, object) && reflection->Initialize(object))
                {
                    return true;
                }
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

void disable_app_nap() noexcept
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        dispatch_async(dispatch_get_main_queue(), ^{
            static NSObject* app_nap_token __attribute__((used)) = [[NSProcessInfo processInfo] beginActivityWithOptions:NSActivityUserInitiatedAllowingIdleSystemSleep | NSActivityLatencyCritical reason:@"No nappy time"];

            #if defined(DEBUG)
            printf("Disable App-Nap: %p\n", app_nap_token);
            #else
            (void)app_nap_token;
            #endif
        });
    });
}
#endif // defined
