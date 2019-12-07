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
		auto hasReflection = TimeOut(60, [&]{
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
#endif // defined
