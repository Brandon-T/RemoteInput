#include "Platform.hxx"

#if defined(__linux__)
#include <X11/Xlib.h>
#endif // defined

#if defined(__linux__)
void GetDesktopResolution(int &width, int &height)
{
    Display* display = XOpenDisplay(nullptr);
    Screen* screen = DefaultScreenOfDisplay(display);
    width = screen->width;
    height = screen->height;
    XCloseDisplay(display);
}
#endif // defined
