#ifndef SIMBAPLUGIN_HXX_INCLUDED
#define SIMBAPLUGIN_HXX_INCLUDED

#include <cstdint>
#include <cstdio>
#include <new>

#include "../EIOS.hxx"
#include "TMemoryManager.hxx"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
extern HMODULE module;
#endif

//extern TMemoryManager PLUGIN_MEMORY_MANAGER;


static const char* PascalExports[] =
{
    //"EIOS_RequestTarget", "Function EIOS_RequestTarget(initArgs: Pointer): Pointer;",
    //"EIOS_ReleaseTarget", "Procedure EIOS_ReleaseTarget(target: Pointer);",
	//"EIOS_GetTargetDimensions", "Procedure EIOS_GetTargetDimensions(target: Pointer; var width: Int32; var height: Int32);",
	//"EIOS_GetImageBuffer", "Function EIOS_GetImageBuffer(target: Pointer): Pointer;",
	/*"EIOS_UpdateImageBuffer", "Procedure EIOS_UpdateImageBuffer(target: Pointer);",
	"EIOS_GetMousePosition", "Procedure EIOS_GetMousePosition(target: Pointer; var width: Int32; var height: Int32);",
	"EIOS_MoveMouse", "Procedure EIOS_MoveMouse(target: Pointer; X: Int32; Y: Int32);",
	"EIOS_HoldMouse", "Procedure EIOS_HoldMouse(target: Pointer; X: Int32; Y: Int32; Button: EIOS_BUTTON);",
	"EIOS_ReleaseMouse", "Procedure EIOS_ReleaseMouse(target: Pointer; X: Int32; Y: Int32; Button: EIOS_BUTTON);",
	"EIOS_IsMouseHeld", "Function EIOS_IsMouseHeld(target: Pointer; Button: EIOS_BUTTON): Boolean;",
	"EIOS_SendString", "Procedure EIOS_SendString(target: Pointer; text: PChar; KeyWait: Int32; KeyModWait: Int32);",
	"EIOS_HoldKey", "Procedure EIOS_HoldKey(target: Pointer; Key: Int32);",
	"EIOS_ReleaseKey", "Procedure EIOS_ReleaseKey(target: Pointer; Key: Int32);",
	"EIOS_IsKeyHeld", "Function EIOS_IsKeyHeld(target: Pointer; Key: Int32): Boolean;"*/
};

static const char* PascalTypes[] =
{
	"EIOS_BUTTON", "Int32"
};

static const long int PascalExportCount = sizeof(PascalExports) / (sizeof(PascalExports[0]) * 2);
static const long int PascalTypeCount = sizeof(PascalTypes) / (sizeof(PascalTypes[0]) * 2);


#ifdef __cplusplus
extern "C"
{
#endif

EXPORT int GetPluginABIVersion();
EXPORT int GetFunctionCount();
EXPORT int GetTypeCount();
EXPORT int GetFunctionInfo(int Index, void** Address, char** Definition);
EXPORT int GetTypeInfo(int Index, char** Type, char** Definition);

EXPORT void SetPluginMemManager(TMemoryManager MemMgr);
EXPORT void OnAttach(void* info);
EXPORT void OnDetach();

#ifdef __cplusplus
}
#endif

#endif // SIMBAPLUGIN_HXX_INCLUDED
