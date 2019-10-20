//
//  Module.cxx
//  RemoteInput
//
//  Created by Brandon on 2019-09-14.
//  Copyright © 2019 XIO. All rights reserved.
//

#include "Module.hxx"

#if defined _WIN32 || defined _WIN64
Module::Module(const char* path) : module(static_cast<void*>(LoadLibraryA(path))) {}
Module::Module(const wchar_t* path) : module(static_cast<void*>(LoadLibraryW(path))) {}

Module::~Module() {FreeLibrary(static_cast<HMODULE>(module));}
#else
Module::Module(const char* path) : module(dlopen(path, RTLD_LAZY | RTLD_GLOBAL)) {}
Module::Module(const wchar_t* path) : module(nullptr)
{
    std::string utf8 = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(path);
    module = dlopen(utf8.c_str(), RTLD_LAZY | RTLD_GLOBAL);
}

Module::~Module() {dlclose(module);}
#endif
