#ifndef SIMBAPLUGIN_HXX_INCLUDED
#define SIMBAPLUGIN_HXX_INCLUDED

#include <cstdint>
#include <cstdio>
#include <new>

#include "EIOS.hxx"
#include "TMemoryManager.hxx"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
extern HMODULE module;
#endif

//extern TMemoryManager PLUGIN_MEMORY_MANAGER;


static const char* PascalExports[] =
{
    "Reflect_GetEIOS", "Function RIGetEIOS(pid: Int32): Pointer;",
	"Reflect_Object", "Function RIGetObject(eios: Pointer; instance: Pointer; cls: String; field: String; desc: String): Pointer;",
    "Reflect_Release_Object", "Procedure RIReleaseObject(eios: Pointer; instance: Pointer);",
    "Reflect_Release_Objects", "Procedure RIReleaseObjects(eios: Pointer; instance: Pointer; amount: PtrUInt);",
    "Reflect_Boolean", "Function RIGetBool(eios: Pointer; instance: Pointer; cls: String; field: String; desc: String): Boolean;",
    "Reflect_Char", "Function RIGetChar(eios: Pointer; instance: Pointer; cls: String; field: String; desc: String): Char;",
    "Reflect_Byte", "Function RIGetByte(eios: Pointer; instance: Pointer; cls: String; field: String; desc: String): Byte;",
    "Reflect_Short", "Function RIGetShort(eios: Pointer; instance: Pointer; cls: String; field: String; desc: String): Int16;",
    "Reflect_Int", "Function RIGetInt(eios: Pointer; instance: Pointer; cls: String; field: String; desc: String): Int32;",
    "Reflect_Long", "Function RIGetLong(eios: Pointer; instance: Pointer; cls: String; field: String; desc: String): Int64;",
    "Reflect_Float", "Function RIGetFloat(eios: Pointer; instance: Pointer; cls: String; field: String; desc: String): Single;",
    "Reflect_Double", "Function RIGetDouble(eios: Pointer; instance: Pointer; cls: String; field: String; desc: String): Double;",
    "Reflect_String", "Procedure RIGetString(eios: Pointer; instance: Pointer; cls: String; field: String; desc: String; output: Pointer; size: PtrUInt);",
    "Reflect_Array", "Function RIGetArray(eios: Pointer; instance: Pointer; cls: String; field: String; desc: String): Pointer;",
    "Reflect_Array_Size", "Function RIGetArraySize(eios: Pointer; arr: Pointer): PtrUInt;",
    "Reflect_Array_Index", "Procedure RIGetArrayElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; index: PtrUInt; length: PtrUInt; buffer: Pointer);",
    "Reflect_Array_Index2D", "Procedure RIGetArrayElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; length: PtrUInt; x: Int32; y: Int32; buffer: Pointer); overload;",
    "Reflect_Array_Index3D", "Procedure RIGetArrayElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; length: PtrUInt; x: Int32; y: Int32; z: Int32; buffer: Pointer); overload;",
    "Reflect_Array_Index4D", "Procedure RIGetArrayElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; length: PtrUInt; x: Int32; y: Int32; z: Int32; w: Int32; buffer: Pointer); overload;",

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
	"EIOS_SendString", "Procedure EIOS_SendString(target: Pointer; text: String; KeyWait: Int32; KeyModWait: Int32);",
	"EIOS_HoldKey", "Procedure EIOS_HoldKey(target: Pointer; Key: Int32);",
	"EIOS_ReleaseKey", "Procedure EIOS_ReleaseKey(target: Pointer; Key: Int32);",
	"EIOS_IsKeyHeld", "Function EIOS_IsKeyHeld(target: Pointer; Key: Int32): Boolean;"*/

	"EIOS_GetDebugImageBufferEx", "Function EIOS_GetDebugImageBufferEx(eios: Pointer): ^UInt8",
};

static const char* PascalTypes[] =
{
	"EIOS_BUTTON", "Int32",
	"ReflectionArrayType", "(RI_CHAR = 0, RI_BYTE = 1, RI_BOOLEAN = 2, RI_SHORT = 3, RI_INT = 4, RI_LONG = 5, RI_FLOAT = 6, RI_DOUBLE = 7, RI_OBJECT = 8)"
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


#ifdef __cplusplus
extern "C"
{
#endif

EXPORT EIOS* Reflect_GetEIOS(std::int32_t pid);
EXPORT jobject Reflect_Object(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc);
EXPORT void Reflect_Release_Object(EIOS* eios, jobject object);
EXPORT void Reflect_Release_Objects(EIOS* eios, jobject* objects, std::size_t amount);
EXPORT char Reflect_Char(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc);
EXPORT std::uint8_t Reflect_Byte(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc);
EXPORT std::int16_t Reflect_Short(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc);
EXPORT std::int32_t Reflect_Int(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc);
EXPORT std::int64_t Reflect_Long(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc);
EXPORT float Reflect_Float(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc);
EXPORT double Reflect_Double(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc);
EXPORT void Reflect_String(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc, char* output, std::size_t output_size);
EXPORT jarray Reflect_Array(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc);
EXPORT std::size_t Reflect_Array_Size(EIOS* eios, jarray array);

EXPORT void Reflect_Array_Index(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t index, std::size_t length, void* buffer);

EXPORT void Reflect_Array_Index2D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, void* buffer);

EXPORT void Reflect_Array_Index3D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z, void* buffer);

EXPORT void Reflect_Array_Index4D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t w, void* buffer);

#ifdef __cplusplus
}
#endif

#endif // SIMBAPLUGIN_HXX_INCLUDED
