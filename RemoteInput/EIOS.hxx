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
    IS_KEYBOARD_INPUT_ENABLED,
    SET_KEYBOARD_INPUT_ENABLED,
    IS_MOUSE_INPUT_ENABLED,
    SET_MOUSE_INPUT_ENABLED,
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
    STOP_ALL_PROCESSING,

    GET_UI_SCALING,
    SET_UI_SCALING,

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
    REFLECT_ARRAY_INDICES,

    REMOTE_VM_INSTRUCTION
};

typedef struct ImageData
{
	std::int32_t parent_process_id;
    std::int32_t parent_thread_id;
    std::int32_t width;
    std::int32_t height;
	bool debug_graphics;
	bool ui_scaling;
    EIOSCommand command;
    std::uint8_t args[100000 * 8];
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
EXPORT [[gnu::stdcall]] EIOS* EIOS_RequestTarget(const char* initargs) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_ReleaseTarget(EIOS* eios) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_GetTargetDimensions(EIOS* eios, std::int32_t* width, std::int32_t* height) noexcept;
EXPORT [[gnu::stdcall]] std::uint8_t* EIOS_GetImageBuffer(EIOS* eios) noexcept;
EXPORT [[gnu::stdcall]] std::uint8_t* EIOS_GetDebugImageBuffer(EIOS* eios) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_SetGraphicsDebugging(EIOS* eios, bool enabled) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_UpdateImageBuffer(EIOS* eios) noexcept;
EXPORT [[gnu::stdcall]] bool EIOS_HasFocus(EIOS* eios) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_GainFocus(EIOS* eios) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_LoseFocus(EIOS* eios) noexcept;
EXPORT [[gnu::stdcall]] bool EIOS_IsKeyboardInputEnabled(EIOS* eios) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_SetKeyboardInputEnabled(EIOS* eios, bool enabled) noexcept;
EXPORT [[gnu::stdcall]] bool EIOS_IsMouseInputEnabled(EIOS* eios) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_SetMouseInputEnabled(EIOS* eios, bool enabled) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_GetMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_GetRealMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_MoveMouse(EIOS* eios, std::int32_t x, std::int32_t y) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_HoldMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_ReleaseMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_ScrollMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t lines) noexcept;
EXPORT [[gnu::stdcall]] bool EIOS_IsMouseButtonHeld(EIOS* eios, std::int32_t button) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_SendString(EIOS* eios, const char* string, std::int32_t keywait, std::int32_t keymodwait) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_HoldKey(EIOS* eios, std::int32_t key) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_ReleaseKey(EIOS* eios, std::int32_t key) noexcept;
EXPORT [[gnu::stdcall]] bool EIOS_IsKeyHeld(EIOS* eios, std::int32_t key) noexcept;
EXPORT [[gnu::stdcall]] std::int32_t EIOS_GetKeyboardSpeed(EIOS* eios) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_SetKeyboardSpeed(EIOS* eios, std::int32_t speed) noexcept;
EXPORT [[gnu::stdcall]] std::int32_t EIOS_GetKeyboardRepeatDelay(EIOS* eios) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_SetKeyboardRepeatDelay(EIOS* eios, std::int32_t delay) noexcept;
EXPORT [[gnu::stdcall]] bool EIOS_GetGraphicsScaling(EIOS* eios) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_SetGraphicsScaling(EIOS* eios, bool enabled) noexcept;
EXPORT [[gnu::stdcall]] EIOS* EIOS_PairClient(pid_t pid) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_KillClientPID(pid_t pid) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_KillClient(EIOS* eios) noexcept;
EXPORT [[gnu::stdcall]] void EIOS_KillZombieClients() noexcept;
EXPORT [[gnu::stdcall]] std::size_t EIOS_GetClients(bool unpaired_only) noexcept;
EXPORT [[gnu::stdcall]] pid_t EIOS_GetClientPID(std::size_t index) noexcept;
#else
EXPORT EIOS* EIOS_RequestTarget(const char* initargs) noexcept;
EXPORT void EIOS_ReleaseTarget(EIOS* eios) noexcept;
EXPORT void EIOS_GetTargetDimensions(EIOS* eios, std::int32_t* width, std::int32_t* height) noexcept;
EXPORT std::uint8_t* EIOS_GetImageBuffer(EIOS* eios) noexcept;
EXPORT std::uint8_t* EIOS_GetDebugImageBuffer(EIOS* eios) noexcept;
EXPORT void EIOS_SetGraphicsDebugging(EIOS* eios, bool enabled) noexcept;
EXPORT void EIOS_UpdateImageBuffer(EIOS* eios) noexcept;
EXPORT bool EIOS_HasFocus(EIOS* eios) noexcept;
EXPORT void EIOS_GainFocus(EIOS* eios) noexcept;
EXPORT void EIOS_LoseFocus(EIOS* eios) noexcept;
EXPORT bool EIOS_IsKeyboardInputEnabled(EIOS* eios) noexcept;
EXPORT void EIOS_SetKeyboardInputEnabled(EIOS* eios, bool enabled) noexcept;
EXPORT bool EIOS_IsMouseInputEnabled(EIOS* eios) noexcept;
EXPORT void EIOS_SetMouseInputEnabled(EIOS* eios, bool enabled) noexcept;
EXPORT void EIOS_GetMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y) noexcept;
EXPORT void EIOS_GetRealMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y) noexcept;
EXPORT void EIOS_MoveMouse(EIOS* eios, std::int32_t x, std::int32_t y) noexcept;
EXPORT void EIOS_HoldMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button) noexcept;
EXPORT void EIOS_ReleaseMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button) noexcept;
EXPORT void EIOS_ScrollMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t lines) noexcept;
EXPORT bool EIOS_IsMouseButtonHeld(EIOS* eios, std::int32_t button) noexcept;
EXPORT void EIOS_SendString(EIOS* eios, const char* string, std::int32_t keywait, std::int32_t keymodwait) noexcept;
EXPORT void EIOS_HoldKey(EIOS* eios, std::int32_t key) noexcept;
EXPORT void EIOS_ReleaseKey(EIOS* eios, std::int32_t key) noexcept;
EXPORT bool EIOS_IsKeyHeld(EIOS* eios, std::int32_t key) noexcept;
EXPORT std::int32_t EIOS_GetKeyboardSpeed(EIOS* eios) noexcept;
EXPORT void EIOS_SetKeyboardSpeed(EIOS* eios, std::int32_t speed) noexcept;
EXPORT std::int32_t EIOS_GetKeyboardRepeatDelay(EIOS* eios) noexcept;
EXPORT void EIOS_SetKeyboardRepeatDelay(EIOS* eios, std::int32_t delay) noexcept;
EXPORT bool EIOS_GetGraphicsScaling(EIOS* eios) noexcept;
EXPORT void EIOS_SetGraphicsScaling(EIOS* eios, bool enabled) noexcept;
EXPORT EIOS* EIOS_PairClient(pid_t pid) noexcept;
EXPORT void EIOS_KillClientPID(pid_t pid) noexcept;
EXPORT void EIOS_KillClient(EIOS* eios) noexcept;
EXPORT void EIOS_KillZombieClients() noexcept;
EXPORT std::size_t EIOS_GetClients(bool unpaired_only) noexcept;
EXPORT pid_t EIOS_GetClientPID(std::size_t index) noexcept;
#endif

#ifdef __cplusplus
}
#endif

#endif /* EIOS_HXX_INCLUDED */
