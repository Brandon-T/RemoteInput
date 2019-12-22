#ifndef SIMBAPLUGIN_HXX_INCLUDED
#define SIMBAPLUGIN_HXX_INCLUDED

#include <cstdint>
#include <cstdio>

#include "EIOS.hxx"
#include "TMemoryManager.hxx"


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
	"Reflect_Pascal_String", "Function RIGetString2(eios: Pointer; instance: Pointer; cls: String; field: String; desc: String): String; native;",
    "Reflect_Array", "Function RIGetArray(eios: Pointer; instance: Pointer; cls: String; field: String; desc: String): Pointer;",
    "Reflect_Array_With_Size", "Function RIGetArray(eios: Pointer; instance: Pointer; output_size: ^PtrUInt; cls: String; field: String; desc: String): Pointer; overload;",
    "Reflect_Array_Size", "Function RIGetArraySize(eios: Pointer; arr: Pointer): PtrUInt;",
    "Reflect_Array_Index", "Procedure RIGetArrayElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; index: PtrUInt; length: PtrUInt; buffer: Pointer);",
    "Reflect_Array_Index2D", "Procedure RIGetArrayElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; length: PtrUInt; x: Int32; y: Int32; buffer: Pointer); overload;",
    "Reflect_Array_Index3D", "Procedure RIGetArrayElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; length: PtrUInt; x: Int32; y: Int32; z: Int32; buffer: Pointer); overload;",
    "Reflect_Array_Index4D", "Procedure RIGetArrayElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; length: PtrUInt; x: Int32; y: Int32; z: Int32; w: Int32; buffer: Pointer); overload;",

	"Pascal_GetDebugImageBuffer", "Function EIOS_GetDebugImageBuffer(eios: Pointer): ^UInt8",
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
EXPORT char* Reflect_Pascal_String(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc);
EXPORT std::uint8_t* Pascal_GetDebugImageBuffer(EIOS* eios);

#ifdef __cplusplus
}
#endif

#endif // SIMBAPLUGIN_HXX_INCLUDED
