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

#include <signal.h>
#include <libproc.h>
#include <sys/syscall.h>
#endif // defined

#if defined(__APPLE__)
void GetDesktopResolution(int &width, int &height)
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

std::int32_t GetCurrentThreadID()
{
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"
	return syscall(SYS_thread_selfid);
	#pragma clang diagnostic pop
}

bool IsProcessAlive(pid_t pid)
{
	return !kill(pid, 0);
}

bool IsThreadAlive(std::int32_t tid)
{
    return !kill(tid, 0);
}

std::vector<pid_t> get_pids()
{
	std::vector<pid_t> pids(2048);
	pids.resize(proc_listpids(PROC_ALL_PIDS, 0, &pids[0], 2048 * sizeof(pid_t)) / sizeof(pid_t));
	std::vector<pid_t>(pids).swap(pids);
	return pids;
}

std::vector<pid_t> get_pids(const char* process_name)
{
	std::vector<pid_t> result;
	std::vector<pid_t> pids = get_pids();
	
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

pid_t PIDFromWindow(void* window)
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
#endif // defined


#if defined(__APPLE__)
@interface NSView (MDRecursiveSubviews)
- (jobject)awtComponent:(JNIEnv*)env;
@end

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
	
//	if (!TimeOut(20, []{ return dlopen("libawt_lwawt.dylib", RTLD_NOLOAD); })) {
//		return nullptr;
//	}
	
	auto IsValidFrame = [&](Reflection* reflection, jobject object) -> bool {
		return reflection->GetClassType(object) == "java.awt.Frame";
    };
	
	auto GetWindowViews = [&]() -> std::vector<NSView*> {
		__block std::vector<NSView*> views;
		dispatch_sync(dispatch_get_main_queue(), ^{
			for (NSWindow* window in NSApplication.sharedApplication.windows)
			{
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
		auto hasReflection = TimeOut(300, [&]{
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

void disable_app_nap()
{
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		dispatch_async(dispatch_get_main_queue(), ^{
			static NSObject* app_nap_token = [[NSProcessInfo processInfo] beginActivityWithOptions:NSActivityUserInitiatedAllowingIdleSystemSleep | NSActivityLatencyCritical reason:@"No nappy time"];
			printf("Disable App-Nap: %p\n", app_nap_token);
		});
	});
}
#endif // defined
