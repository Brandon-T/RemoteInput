#include "Platform.hxx"

#if defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <functional>
#include <cstring>
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
#endif

#if defined(__linux__)
int GetClassName(Display* display, Window window, char* lpClassName, int nMaxCount)
{
    if (!lpClassName || !nMaxCount)
    {
        return 0;
    }

    XClassHint hint;
    if (XGetClassHint(display, window, &hint))
    {
        if (hint.res_name)
        {
            XFree(hint.res_name);
        }

        if (hint.res_class)
        {
            memset(lpClassName, 0, nMaxCount * sizeof(char));

            int length = strlen(hint.res_class);
            int max_copy = (nMaxCount < length ? nMaxCount - 1 : length);
            strncpy(lpClassName, hint.res_class, max_copy);
            XFree(hint.res_class);
            return max_copy;
        }
    }
    return 0;
}

int GetWindowText(Display* display, Window window, char* lpString, int nMaxCount)
{
    if (!lpString || !nMaxCount)
    {
        return 0;
    }

    char* name = nullptr;
    if (XFetchName(display, window, &name) > 0)
    {
        memset(lpString, 0, nMaxCount * sizeof(char));

        int length = strlen(name);
        int max_copy = (nMaxCount < length ? nMaxCount - 1 : length);
        strncpy(lpString, name, max_copy);
        XFree(name);
        return max_copy;
    }
    return 0;
}

void EnumWindows(Display* display, bool (*EnumWindowsProc)(Display*, Window, void*), void* other)
{
    if (!EnumWindowsProc)
    {
        return;
    }

    Atom atomList = XInternAtom(display, "_NET_CLIENT_LIST", true);
    Atom type = None;
    int format = 0;
    unsigned long nItems = 0;
    unsigned long bytesAfter = 0;
    unsigned char* property = nullptr;
    if (XGetWindowProperty(display, XDefaultRootWindow(display), atomList, 0L, ~0L, false, AnyPropertyType, &type, &format, &nItems, &bytesAfter, &property) == Success)
    {
        if (property)
        {
            for (unsigned int i = 0; i < nItems; ++i)
            {
                Window* windows = reinterpret_cast<Window*>(property);
                if (!EnumWindowsProc(display, windows[i], other))
                {
                    break;
                }
            }
            XFree(property);
        }
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
#endif

#if defined(__linux__)
bool EnumWindowsProc(Display* display, Window window, void* other)
{
    pid_t pid = 0;
    GetWindowThreadProcessId(display, window, &pid);
    if (pid == getpid() && other)
    {
        char className[256] = {0};
        int result = GetClassName(display, window, className, sizeof(className));
        if (result != 0 && std::string(className) == "jagexappletviewer")  //Need a better way to check the class..
        {
            *reinterpret_cast<Window*>(other) = window;
            return false;
        }
    }
    return true;
}

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
	
    auto awt_GetComponent = reinterpret_cast<jobject (*)(JNIEnv*, void*)>(dlsym(RTLD_NEXT, "awt_GetComponent"));
    if (!awt_GetComponent)
    {
        return nullptr;
    }

    Display* display = XOpenDisplay(nullptr);
    Window windowFrame = 0;
    EnumWindows(display, EnumWindowsProc, &windowFrame);
    XCloseDisplay(display);

    if (!windowFrame)
    {
        return nullptr;
    }

    Reflection* reflection = new Reflection();
    if (reflection->Attach())
    {
        jobject object = awt_GetComponent(reflection->getEnv(), reinterpret_cast<void*>(windowFrame));  //java.awt.Frame
        if (IsValidFrame(reflection, object) && reflection->Initialize(object))
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
