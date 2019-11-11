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


#include "ControlCenter.hxx"


enum class EIOSCommand: std::uint32_t
{
    COMMAND_NONE,
	GET_TARGET_DIMENSIONS,
	GET_MOUSE,
    MOVE_MOUSE,
    HOLD_MOUSE,
    RELEASE_MOUSE,
    IS_MOUSE_HELD,
	SEND_STRING,
	HOLD_KEY,
	RELEASE_KEY,
	IS_KEY_HELD,

	REFLECT
};

typedef struct ImageData
{
    std::int32_t parentId;
    std::int32_t width;
    std::int32_t height;
    EIOSCommand command;
    std::uint8_t args[4096];
} ImageData;

typedef struct EIOS
{
    std::int32_t pid;
	std::int32_t width;
    std::int32_t height;
    std::unique_ptr<ControlCenter> control_center;
} EIOS;

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
[[gnu::stdcall]] EIOS* EIOS_RequestTarget(const char* initargs);
[[gnu::stdcall]] void EIOS_ReleaseTarget(EIOS* eios);
[[gnu::stdcall]] void EIOS_GetTargetDimensions(EIOS* eios, std::int32_t* width, std::int32_t* height);
[[gnu::stdcall]] std::uint8_t* EIOS_GetImageBuffer(EIOS* eios);
[[gnu::stdcall]] void EIOS_UpdateImageBuffer(EIOS* eios);
[[gnu::stdcall]] void EIOS_GetMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y);
[[gnu::stdcall]] void EIOS_MoveMouse(EIOS* eios, std::int32_t x, std::int32_t y);
[[gnu::stdcall]] void EIOS_HoldMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button);
[[gnu::stdcall]] void EIOS_ReleaseMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button);
[[gnu::stdcall]] bool EIOS_IsMouseHeld(EIOS* eios, std::int32_t button);
[[gnu::stdcall]] void EIOS_SendString(EIOS* eios, const char* string, std::int32_t keywait, std::int32_t keymodwait);
[[gnu::stdcall]] void EIOS_HoldKey(EIOS* eios, std::int32_t key);
[[gnu::stdcall]] void EIOS_ReleaseKey(EIOS* eios, std::int32_t key);
[[gnu::stdcall]] bool EIOS_IsKeyHeld(EIOS* eios, std::int32_t key);
#else
EIOS* EIOS_RequestTarget(const char* initargs);
void EIOS_ReleaseTarget(EIOS* eios);
void EIOS_GetTargetDimensions(EIOS* eios, std::int32_t* width, std::int32_t* height);
std::uint8_t* EIOS_GetImageBuffer(EIOS* eios);
void EIOS_UpdateImageBuffer(EIOS* eios);
void EIOS_GetMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y);
void EIOS_MoveMouse(EIOS* eios, std::int32_t x, std::int32_t y);
void EIOS_HoldMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button);
void EIOS_ReleaseMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button);
bool EIOS_IsMouseHeld(EIOS* eios, std::int32_t button);
void EIOS_SendString(EIOS* eios, const char* string, std::int32_t keywait, std::int32_t keymodwait);
void EIOS_HoldKey(EIOS* eios, std::int32_t key);
void EIOS_ReleaseKey(EIOS* eios, std::int32_t key);
bool EIOS_IsKeyHeld(EIOS* eios, std::int32_t key);
#endif

#ifdef __cplusplus
}
#endif

#endif /* EIOS_HXX_INCLUDED */
