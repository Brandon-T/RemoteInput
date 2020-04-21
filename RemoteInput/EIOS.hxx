//
//  EIOS.hxx
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
#define EXPORT [[gnu::visibility("default")]]
#endif


#include "ControlCenter.hxx"


enum class EIOSCommand: std::uint32_t
{
    COMMAND_NONE,
	KILL_APPLICATION,
	
	GET_TARGET_DIMENSIONS,
	HAS_FOCUS,
	GAIN_FOCUS,
	LOSE_FOCUS,
	IS_INPUT_ENABLED,
	SET_INPUT_ENABLED,
	GET_MOUSE,
	GET_REAL_MOUSE,
    MOVE_MOUSE,
    HOLD_MOUSE,
    RELEASE_MOUSE,
	SCROLL_MOUSE,
    IS_MOUSE_HELD,
	SEND_STRING,
	HOLD_KEY,
	RELEASE_KEY,
	IS_KEY_HELD,

    GET_KEYBOARD_SPEED,
    SET_KEYBOARD_SPEED,
    GET_KEYBOARD_REPEAT_DELAY,
    SET_KEYBOARD_REPEAT_DELAY,

	REFLECT_OBJECT,
    REFLECT_COMPARE_OBJECTS,
	REFLECT_INSTANCE_OF,
	REFLECT_RELEASE_OBJECT,
	REFLECT_RELEASE_OBJECTS,
	REFLECT_CHAR,
	REFLECT_BYTE,
	REFLECT_BOOLEAN,
	REFLECT_SHORT,
	REFLECT_INT,
	REFLECT_LONG,
	REFLECT_FLOAT,
	REFLECT_DOUBLE,
	REFLECT_STRING,
	REFLECT_ARRAY,
	REFLECT_ARRAY_WITH_SIZE,
	REFLECT_ARRAY_SIZE,
	REFLECT_ARRAY_INDEX_SIZE,
	REFLECT_ARRAY_INDEX,
	REFLECT_ARRAY_INDEX2D,
	REFLECT_ARRAY_INDEX3D,
	REFLECT_ARRAY_INDEX4D,
    REFLECT_ARRAY_INDICES
};

typedef struct ImageData
{
	std::int32_t parent_process_id;
    std::int32_t parent_thread_id;
    std::int32_t width;
    std::int32_t height;
	bool debug_graphics;
    EIOSCommand command;
    std::uint8_t args[4096 * 8];
} ImageData;

typedef struct EIOS
{
    std::int32_t pid;
	std::int32_t width;
    std::int32_t height;
	std::intptr_t local_storage;
    std::unique_ptr<ControlCenter> control_center;
} EIOS;

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
EXPORT [[gnu::stdcall]] EIOS* EIOS_RequestTarget(const char* initargs);
EXPORT [[gnu::stdcall]] void EIOS_ReleaseTarget(EIOS* eios);
EXPORT [[gnu::stdcall]] void EIOS_GetTargetDimensions(EIOS* eios, std::int32_t* width, std::int32_t* height);
EXPORT [[gnu::stdcall]] std::uint8_t* EIOS_GetImageBuffer(EIOS* eios);
EXPORT [[gnu::stdcall]] std::uint8_t* EIOS_GetDebugImageBuffer(EIOS* eios);
EXPORT [[gnu::stdcall]] void EIOS_SetGraphicsDebugging(EIOS* eios, bool enabled);
EXPORT [[gnu::stdcall]] void EIOS_UpdateImageBuffer(EIOS* eios);
EXPORT [[gnu::stdcall]] bool EIOS_HasFocus(EIOS* eios);
EXPORT [[gnu::stdcall]] void EIOS_GainFocus(EIOS* eios);
EXPORT [[gnu::stdcall]] void EIOS_LoseFocus(EIOS* eios);
EXPORT [[gnu::stdcall]] bool EIOS_IsInputEnabled(EIOS* eios);
EXPORT [[gnu::stdcall]] void EIOS_SetInputEnabled(EIOS* eios, bool enabled);
EXPORT [[gnu::stdcall]] void EIOS_GetMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y);
EXPORT [[gnu::stdcall]] void EIOS_GetRealMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y);
EXPORT [[gnu::stdcall]] void EIOS_MoveMouse(EIOS* eios, std::int32_t x, std::int32_t y);
EXPORT [[gnu::stdcall]] void EIOS_HoldMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button);
EXPORT [[gnu::stdcall]] void EIOS_ReleaseMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button);
EXPORT [[gnu::stdcall]] void EIOS_ScrollMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t lines);
EXPORT [[gnu::stdcall]] bool EIOS_IsMouseHeld(EIOS* eios, std::int32_t button);
EXPORT [[gnu::stdcall]] void EIOS_SendString(EIOS* eios, const char* string, std::int32_t keywait, std::int32_t keymodwait);
EXPORT [[gnu::stdcall]] void EIOS_HoldKey(EIOS* eios, std::int32_t key);
EXPORT [[gnu::stdcall]] void EIOS_ReleaseKey(EIOS* eios, std::int32_t key);
EXPORT [[gnu::stdcall]] bool EIOS_IsKeyHeld(EIOS* eios, std::int32_t key);
EXPORT [[gnu::stdcall]] EIOS* EIOS_PairClient(pid_t pid);
EXPORT [[gnu::stdcall]] void EIOS_KillClientPID(pid_t pid);
EXPORT [[gnu::stdcall]] void EIOS_KillClient(EIOS* eios);
EXPORT [[gnu::stdcall]] void EIOS_KillZombieClients();
EXPORT [[gnu::stdcall]] std::size_t EIOS_GetClients(bool unpaired_only);
EXPORT [[gnu::stdcall]] pid_t EIOS_GetClientPID(std::size_t index);
#else
EXPORT EIOS* EIOS_RequestTarget(const char* initargs);
EXPORT void EIOS_ReleaseTarget(EIOS* eios);
EXPORT void EIOS_GetTargetDimensions(EIOS* eios, std::int32_t* width, std::int32_t* height);
EXPORT std::uint8_t* EIOS_GetImageBuffer(EIOS* eios);
EXPORT std::uint8_t* EIOS_GetDebugImageBuffer(EIOS* eios);
EXPORT void EIOS_SetGraphicsDebugging(EIOS* eios, bool enabled);
EXPORT void EIOS_UpdateImageBuffer(EIOS* eios);
EXPORT bool EIOS_HasFocus(EIOS* eios);
EXPORT void EIOS_GainFocus(EIOS* eios);
EXPORT void EIOS_LoseFocus(EIOS* eios);
EXPORT bool EIOS_IsInputEnabled(EIOS* eios);
EXPORT void EIOS_SetInputEnabled(EIOS* eios, bool enabled);
EXPORT void EIOS_GetMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y);
EXPORT void EIOS_GetRealMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y);
EXPORT void EIOS_MoveMouse(EIOS* eios, std::int32_t x, std::int32_t y);
EXPORT void EIOS_HoldMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button);
EXPORT void EIOS_ReleaseMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button);
EXPORT void EIOS_ScrollMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t lines);
EXPORT bool EIOS_IsMouseHeld(EIOS* eios, std::int32_t button);
EXPORT void EIOS_SendString(EIOS* eios, const char* string, std::int32_t keywait, std::int32_t keymodwait);
EXPORT void EIOS_HoldKey(EIOS* eios, std::int32_t key);
EXPORT void EIOS_ReleaseKey(EIOS* eios, std::int32_t key);
EXPORT bool EIOS_IsKeyHeld(EIOS* eios, std::int32_t key);
EXPORT std::int32_t EIOS_GetKeyboardSpeed(EIOS* eios);
EXPORT void EIOS_SetKeyboardSpeed(EIOS* eios, std::int32_t speed);
EXPORT std::int32_t EIOS_GetKeyboardRepeatDelay(EIOS* eios);
EXPORT void EIOS_SetKeyboardRepeatDelay(EIOS* eios, std::int32_t delay);
EXPORT EIOS* EIOS_PairClient(pid_t pid);
EXPORT void EIOS_KillClientPID(pid_t pid);
EXPORT void EIOS_KillClient(EIOS* eios);
EXPORT void EIOS_KillZombieClients();
EXPORT std::size_t EIOS_GetClients(bool unpaired_only);
EXPORT pid_t EIOS_GetClientPID(std::size_t index);
#endif

#ifdef __cplusplus
}
#endif

#endif /* EIOS_HXX_INCLUDED */
