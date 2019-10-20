#include "Platform.hxx"

#if defined(__APPLE__)
#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>
#endif // defined

#if defined(__APPLE__)
void GetDesktopResolution(int &width, int &height)
{
    NSRect frame = [[NSScreen mainScreen] frame];
    width = reinterpret_cast<int>(frame.size.width);
    height = reinterpret_cast<int>(frame.size.height);
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

    delete Reflection;
    return nullptr;
}
#endif // defined
