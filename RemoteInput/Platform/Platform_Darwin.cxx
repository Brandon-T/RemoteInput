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
	auto IsValidFrame = [&](Reflection* reflection, jobject object) -> Reflection* {
        auto start = std::chrono::high_resolution_clock::now();
        while(reflection && reflection->GetClassType(object) != "java.awt.Frame")
        {
            if (elapsed_time<std::chrono::seconds>(start) >= 20)
            {
                return nullptr;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        return reflection;
    };
	
	auto start = std::chrono::high_resolution_clock::now();
	while(!dlopen("libawt_lwawt.dylib", RTLD_NOLOAD))
	{
		if (elapsed_time<std::chrono::seconds>(start) >= 20)
		{
			return nullptr;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	
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
	
	Reflection* reflection = new Reflection();

	for (auto&& view : views)
	{
		if (reflection->Attach())
		{
			//TODO: Check if we can call "awt_GetComponent" from the JDK like on Linux
			jobject object = [view awtComponent:reflection->getEnv()];  //java.awt.Frame
			if (IsValidFrame(reflection, object) && reflection->Initialize(object))
			{
				reflection->Detach();
				return reflection;
			}
			reflection->Detach();
		}
	}

	delete reflection;
	return nullptr;
}
#endif // defined
