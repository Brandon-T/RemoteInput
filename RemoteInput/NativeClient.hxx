//
// Created by Brandon on 2025-05-17.
//

#ifndef REMOTEINPUT_NATIVECLIENT_HXX
#define REMOTEINPUT_NATIVECLIENT_HXX

#include <windows.h>

class NativeClient
{
private:
    HWND window_handle;
    HWND renderer_window_handle;

public:
    NativeClient(HWND window_handle, HWND renderer_window_handle);
};

#endif //REMOTEINPUT_NATIVECLIENT_HXX
