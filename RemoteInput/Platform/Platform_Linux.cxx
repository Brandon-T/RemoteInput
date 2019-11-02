#include "Platform.hxx"

#if defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
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

std::string GetClassName(Display* display, Window window)
{
    XClassHint hint;
    if (XGetClassHint(display, window, &hint))
    {
        if (hint.res_name)
        {
            XFree(hint.res_name);
        }

        if (hint.res_class)
        {
            std::string wm_class = hint.res_class;
            XFree(hint.res_class);
            return wm_class;
        }
    }

    return "";
}

std::string GetWindowName(Display* display, Window window)
{
    char* name = nullptr;
    if (XFetchName(display, window, &name) > 0)
    {
        return name;
        XFree(name);
    }
    return "";
}

bool EnumWindows(Display* display, bool (*EnumWindowsProc)(Display*, Window, void*), void* other)
{
    Window root_return;
    Window parent_return;
    Window* children_return;
    unsigned int nchildren_return;

    if (XQueryTree(display, XDefaultRootWindow(display), &root_return, &parent_return, &children_return, &nchildren_return))
    {
        for (unsigned int i = 0; i < nchildren_return; ++i)
        {
            if (!EnumWindowsProc(display, children_return[i], other))
            {
                break;
            }
        }
        XFree(children_return);
    }
}

void EnumChildWindows(Display* display, Window parentWindow, bool (*EnumChildProc)(Display*, Window, void*), void* other)
{
    if (!EnumChildProc)
    {
        return;
    }

    Window root_return;
    Window parent_return;
    Window* children_return;
    unsigned int nchildren_return;
    if (XQueryTree(display, parentWindow, &root_return, &parent_return, &children_return, &nchildren_return))
    {
        for (unsigned int i = 0; i < nchildren_return; ++i)
        {
            if (!EnumChildProc(display, children_return[i], other))
            {
                break;
            }

            // Recursive search children
            EnumChildWindows(display, children_return[i], EnumChildProc, other);
        }
        XFree(children_return);
    }
}

void GetWindowThreadProcessId(Display* display, Window window, pid_t* pid)
{
    if (!pid) { return; }
    *pid = 0;

    Atom atomPID = XInternAtom(display, "_NET_WM_PID", True);
    if (atomPID == None)
    {
        return;
    }

    Atom type = None;
    int format = 0;
    unsigned long nItems = 0;
    unsigned long bytesAfter = 0;
    unsigned char* property = nullptr;

    if (XGetWindowProperty(display, window, atomPID, 0L, 1L, False, XA_CARDINAL, &type, &format, &nItems, &bytesAfter, &property) == 0)
    {
        if (property)
        {
            *pid = *reinterpret_cast<pid_t*>(property);
            XFree(property);
        }
    }
}

std::vector<Window> GetWindowsFromProcessId(Display* display, pid_t pid)
{
    // Get Atom PID
    Atom atomPID = XInternAtom(display, "_NET_WM_PID", True);
    if (atomPID == None)
    {
        return std::vector<Window>();
    }

    std::function<void(Display*, Window, Atom, pid_t, std::vector<Window>&)> GetWindowsFromPIDInternal = [&](Display* display, Window window, Atom atomPID, pid_t pid, std::vector<Window>& results) {
        // Get Window PID
        Atom type = None;
        int format = 0;
        unsigned long nItems = 0;
        unsigned long bytesAfter = 0;
        unsigned char* property = nullptr;

        if (XGetWindowProperty(display, window, atomPID, 0L, 1L, False, XA_CARDINAL, &type, &format, &nItems, &bytesAfter, &property) == 0)
        {
            if (property)
            {
                if (pid == *reinterpret_cast<pid_t*>(property))
                {
                    results.emplace_back(window);
                }
                XFree(property);
            }
        }

        // Check all child windows
        Window root_return;
        Window parent_return;
        Window* children_return;
        unsigned int nchildren_return;
        if (XQueryTree(display, window, &root_return, &parent_return, &children_return, &nchildren_return))
        {
            for (unsigned int i = 0; i < nchildren_return; ++i)
            {
                // Recursive search children
                GetWindowsFromPIDInternal(display, children_return[i], atomPID, pid, results);
            }
            XFree(children_return);
        }
    };

    std::vector<Window> windows;
    GetWindowsFromPIDInternal(display, XDefaultRootWindow(display), atomPID, pid, windows);
    return windows;
}

bool EnumChildWindowsProc(Display* display, Window window, void* other)
{
    pid_t pid = 0;
    GetWindowThreadProcessId(display, window, &pid);
    if (pid == getpid() && other)
    {
        std::vector<Window>* results = reinterpret_cast<std;:vector<Window>*>(other);
        results->push_back(window);
    }
    return true;
}

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
