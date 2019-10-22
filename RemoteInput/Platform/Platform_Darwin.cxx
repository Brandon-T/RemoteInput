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
    NSRect frame = [[NSScreen mainScreen] frame];
    width = static_cast<int>(frame.size.width);
    height = static_cast<int>(frame.size.height);
}
#endif // defined


#if defined(__APPLE__)
@interface NSView (MDRecursiveSubviews)
- (jobject)awtComponent:(JNIEnv*)env;
@end

Reflection* GetNativeReflector()
{
    Reflection* reflection = new Reflection();

    for (NSWindow* window in NSApplication.sharedApplication.windows)
    {
        if ([window isKindOfClass:NSClassFromString(@"AWTWindow_Normal")] && [window.contentView isKindOfClass:NSClassFromString(@"AWTView")])
		{
			NSView* view = window.contentView;
			if (reflection->Attach())
			{
				jobject object = [view awtComponent:reflection->getEnv()];  //java.awt.Frame
				if (reflection->Initialize(object))
				{
				    reflection->Detach();
				    return reflection;
				}
				reflection->Detach();
			}
		}
    }

    delete reflection;
    return nullptr;
}
#endif // defined
