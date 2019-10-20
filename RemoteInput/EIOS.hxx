//
//  EIOS.hpp
//  RemoteInput
//
//  Created by Brandon on 2019-09-14.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef EIOS_HXX_INCLUDED
#define EIOS_HXX_INCLUDED

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#if defined(_WIN32) || defined (_WIN64)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include "Echo/SharedEvent.hxx"
#include "SHMAllocator.hxx"

typedef struct ImageData {
    std::uintptr_t mutex;

    int parentId;
    int width;
    int height;
    int imgoff;
    uint8_t args[4096];
} ImageData;

typedef struct EIOS {
    int pid;
    void* memoryMap;
    int width;
    int height;
    ImageData* imageData;
} EIOS;

#ifdef __cplusplus
extern "C" {
#endif

EIOS* __stdcall EIOS_RequestTarget(const char* initargs);
void __stdcall EIOS_ReleaseTarget(EIOS* eios);
void __stdcall EIOS_GetTargetDimensions(EIOS* eios, std::int32_t* width, std::int32_t* height);
std::uint8_t* __stdcall EIOS_GetImageBuffer(EIOS* eios);
void __stdcall EIOS_UpdateImageBuffer(EIOS* eios);
void __stdcall EIOS_GetMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y);
void __stdcall EIOS_MoveMouse(EIOS* eios, std::int32_t x, std::int32_t y);
void __stdcall EIOS_HoldMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button);
void __stdcall EIOS_ReleaseMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button);
bool __stdcall EIOS_IsMouseHeld(EIOS* eios, std::int32_t button);
void __stdcall EIOS_SendString(EIOS* eios, const char* string, std::int32_t keywait, std::int32_t keymodwait);
void __stdcall EIOS_HoldKey(EIOS* eios, std::int32_t key);
void __stdcall EIOS_ReleaseKey(EIOS* eios, std::int32_t key);
bool __stdcall EIOS_IsKeyHeld(EIOS* eios, std::int32_t key);

#ifdef __cplusplus
}
#endif

#endif /* EIOS_HXX_INCLUDED */
