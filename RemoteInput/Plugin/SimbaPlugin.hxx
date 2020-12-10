#ifndef SIMBAPLUGIN_HXX_INCLUDED
#define SIMBAPLUGIN_HXX_INCLUDED

#include <cstdint>
#include <cstdio>

#include "EIOS.hxx"
#include "TMemoryManager.hxx"


static const char* PascalExports[] =
{
    "Pascal_Reflect_Equal", "Function RIObjectsEqual(eios: Pointer; A, B: Pointer): Boolean; native;",
	"Pascal_Reflect_InstanceOf", "Function RIObjectIsInstanceOf(eios: Pointer; instance: Pointer; cls: String): Boolean; native;",
	"Pascal_Reflect_Object", "Function RIGetObject(eios: Pointer; instance: Pointer; constref field: ^const RIField): Pointer; native;",
    "Pascal_Reflect_Release_Object", "Procedure RIReleaseObject(eios: Pointer; instance: Pointer); native;",
    "Pascal_Reflect_Release_Objects", "Procedure RIReleaseObjects(eios: Pointer; objects: Array of Pointer); native;",
    "Pascal_Reflect_Boolean", "Function RIGetBool(eios: Pointer; instance: Pointer; constref field: ^const RIField): Boolean; native;",
    "Pascal_Reflect_Char", "Function RIGetChar(eios: Pointer; instance: Pointer; constref field: ^const RIField): Char; native;",
    "Pascal_Reflect_Byte", "Function RIGetByte(eios: Pointer; instance: Pointer; constref field: ^const RIField): Byte; native;",
    "Pascal_Reflect_Short", "Function RIGetShort(eios: Pointer; instance: Pointer; constref field: ^const RIField): Int16; native;",
    "Pascal_Reflect_Int", "Function RIGetInt(eios: Pointer; instance: Pointer; constref field: ^const RIField): Int32; native;",
    "Pascal_Reflect_Long", "Function RIGetLong(eios: Pointer; instance: Pointer; constref field: ^const RIField): Int64; native;",
    "Pascal_Reflect_Float", "Function RIGetFloat(eios: Pointer; instance: Pointer; constref field: ^const RIField): Single; native;",
    "Pascal_Reflect_Double", "Function RIGetDouble(eios: Pointer; instance: Pointer; constref field: ^const RIField): Double; native;",
    "Pascal_Reflect_String", "Function RIGetString(eios: Pointer; instance: Pointer; constref field: ^const RIField): String; native;",
	"Pascal_Reflect_Array", "Function RIGetArray(eios: Pointer; instance: Pointer; constref field: ^const RIField): Pointer; native;",

	//Array size
    "Pascal_Reflect_Array_With_Size", "Function RIGetArray(eios: Pointer; instance: Pointer; output_size: ^SizeUInt; constref field: ^const RIField): Pointer; overload; native;",
    "Pascal_Reflect_Array_Size", "Function RIGetArraySize(eios: Pointer; arr: Pointer): SizeUInt; native;",
	"Pascal_Reflect_Array_Index_Size", "Function RIGetArraySize(eios: Pointer; arr: Pointer; index: SizeUInt): SizeUInt; overload; native;",
	"Pascal_Reflect_Array_Indices", "Function RIGetArrayElements(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; indices: Array of Int32): Pointer; overload; native;",

	//Array 1-D
	"Pascal_Reflect_Array_SingleIndex", "Function RIGetArraySingleElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; index: SizeUInt): Pointer; native;",
    "Pascal_Reflect_Array_Index", "Function RIGetArrayElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; index, length: SizeUInt): Pointer; native;",

	//Array 2-D
	"Pascal_Reflect_Array_SingleIndex2D", "Function RIGetArraySingleElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; x, y: Int32): Pointer; overload; native;",
    "Pascal_Reflect_Array_Index2D", "Function RIGetArrayElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; length: SizeUInt; x, y: Int32): Pointer; overload; native;",

	//Array 3-D
	"Pascal_Reflect_Array_SingleIndex3D", "Function RIGetArraySingleElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; x, y, z: Int32): Pointer; overload; native;",
    "Pascal_Reflect_Array_Index3D", "Function RIGetArrayElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; length: SizeUInt; x, y, z: Int32): Pointer; overload; native;",

	//Array 4-D
	"Pascal_Reflect_Array_SingleIndex4D", "Function RIGetArraySingleElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; x, y, z, w: Int32): Pointer; overload; native;",
    "Pascal_Reflect_Array_Index4D", "Function RIGetArrayElement(eios: Pointer; arr: Pointer; elementType: ReflectionArrayType; length: SizeUInt; x, y, z, w: Int32): Pointer; overload; native;",

	//Graphics
	"Pascal_GetDebugImageBuffer", "Function EIOS_GetDebugImageBuffer(eios: Pointer): ^UInt8; native;",
	"Pascal_SetGraphicsDebugging", "Procedure EIOS_SetGraphicsDebugging(eios: Pointer; enabled: Boolean); native;",

	//Pairing
	"Pascal_PairClient", "Function EIOS_PairClient(pid: Int32): Pointer; native;",
	"Pascal_KillClientPID", "Procedure EIOS_KillClient(pid: Int32); native;",
	"Pascal_KillClient", "Procedure EIOS_KillClient(eios: Pointer); overload; native;",
	"Pascal_GetClients", "Function EIOS_GetClients(unpaired_only: Boolean): SizeUInt; native;",
	"Pascal_GetClientPID", "Function EIOS_GetClientPID(index: SizeUInt): Int32; native;",

	//Injecting
	"Pascal_Inject", "Procedure RIInject(process_name: String); native;",
	"Pascal_Inject_PID", "Procedure RIInject(pid: Int32); overload; native;",
	
	//Other
	"Pascal_HasFocus", "Function EIOS_HasFocus(eios: Pointer): Boolean; native;",
	"Pascal_GainFocus", "Procedure EIOS_GainFocus(eios: Pointer); native;",
	"Pascal_LoseFocus", "Procedure EIOS_LoseFocus(eios: Pointer); native;",
	"Pascal_IsInputEnabled", "Function EIOS_IsInputEnabled(eios: Pointer): Boolean; native;",
	"Pascal_SetInputEnabled", "Function EIOS_SetInputEnabled(eios: Pointer; enabled: Boolean): Boolean; native;",
	"Pascal_GetRealMousePosition", "Procedure EIOS_GetRealMousePosition(eios: Pointer; var x, y: Int32); native;",
    "Pascal_GetKeyboardSpeed", "Function EIOS_GetKeyboardSpeed(eios: Pointer): Int32; native;",
    "Pascal_SetKeyboardSpeed", "Procedure EIOS_SetKeyboardSpeed(eios: Pointer; speed: Int32); native;",
    "Pascal_GetKeyboardRepeatDelay", "Function EIOS_GetKeyboardRepeatDelay(eios: Pointer): Int32; native;",
    "Pascal_SetKeyboardRepeatDelay", "Procedure EIOS_SetKeyboardRepeatDelay(eios: Pointer; delay: Int32); native;",

    //RemoteVM
    "RemoteVM_Init", "Function RemoteVM.Init(): Boolean; native;",
    "RemoteVM_InitEx", "Function RemoteVM.Init(argc: integer; args: ^string): Boolean; overload; native;",
    "Remote_Free", "Procedure RemoteVM.Free(); native;",

    "RemoteVM_MaxMemoryChunkSize", "Function RemoteVM.MaxMemoryChunkSize(): SizeUInt; native;",
    "RemoteVM_AllocateMemory", "Function RemoteVM.AllocateMemory(size: SizeUInt): Pointer; native;",
    "RemoteVM_ReadMemory", "Function RemoteVM.ReadMemory(destintation, source: Pointer; size: SizeUInt): Boolean; native;",
    "RemoteVM_WriteMemory", "Function RemoteVM.WriteMemory(destintation, source: Pointer; size: SizeUInt): Boolean; native;",
    "RemoteVM_FreeMemory", "Procedure RemoteVM.FreeMemory(memory: Pointer); native;",
    "RemoteVM_GetVersion", "Function RemoteVM.GetVersion: jint; native",
    "RemoteVM_DefineClass", "Function RemoteVM.DefineClass(name: String; loader: jobject; buf: Pointer; len: jsize): jclass; native;",
    "RemoteVM_FindClass", "Function RemoteVM.FindClass(name: String): jclass; native;",
    "RemoteVM_FromReflectedMethod", "Function RemoteVM.FromReflectedMethod(method: jobject): jmethodID; native;",
    "RemoteVM_FromReflectedField", "Function RemoteVM.FromReflectedField(field: jobject): jfieldID; native;",
    "RemoteVM_ToReflectedMethod", "Function RemoteVM.ToReflectedMethod(cls: jclass; methodID: jmethodID; isStatic: jboolean): jobject; native;",
    "RemoteVM_GetSuperclass", "Function RemoteVM.GetSuperclass(sub: jclass): jclass; native;",
    "RemoteVM_IsAssignableFrom", "Function RemoteVM.IsAssignableFrom(sub, sup: jclass): jboolean; native;",
    "RemoteVM_ToReflectedField", "Function RemoteVM.ToReflectedField(cls: jclass; fieldID: jfieldID; isStatic: jboolean): jobject; native;",
    "RemoteVM_Throw", "Function RemoteVM.Throw(obj: jthrowable): jint; native;",
    "RemoteVM_ThrowNew", "Function RemoteVM.ThrowNew(clazz: jclass; const msg: string): jint; native;",
    "RemoteVM_GetExceptionMessage", "Function RemoteVM.GetExceptionMessage(): String; native;",
    "RemoteVM_FatalError", "Procedure RemoteVM.FatalError(msg: String); native;",
    "RemoteVM_DeleteGlobalRef", "Procedure RemoteVM.DeleteGlobalRef(gref: jobject); native;",
    "RemoteVM_IsSameObject", "Function RemoteVM.IsSameObject(obj1, obj2: jobject): jboolean; native;",
    "RemoteVM_AllocObject", "Function RemoteVM.AllocObject(clazz: jclass): jobject; native;",
    "RemoteVM_NewObject", "Function RemoteVM.NewObject(clazz: jclass; methodID: jmethodID; args: array of jvalue): jobject; native;",
    "RemoteVM_GetObjectClass", "Function RemoteVM.GetObjectClass(obj: jobject): jclass; native;",
    "RemoteVM_IsInstanceOf", "Function RemoteVM.IsInstanceOf(obj: jobject; clazz: jclass): jboolean; native;",
    "RemoteVM_GetMethodID", "Function RemoteVM.GetMethodID(clazz: jclass; name, sig: String): jmethodID; native;",
    "RemoteVM_CallObjectMethod", "Function RemoteVM.CallObjectMethod(obj: jobject; methodID: jmethodID; args: array of jvalue): jobject; native;",
    "RemoteVM_CallBooleanMethod", "Function RemoteVM.CallBooleanMethod(obj: jobject; methodID: jmethodID; args: array of jvalue): jboolean; native;",
    "RemoteVM_CallByteMethod", "Function RemoteVM.CallByteMethod(obj: jobject; methodID: jmethodID; args: array of jvalue): jbyte; native;",
    "RemoteVM_CallCharMethod", "Function RemoteVM.CallCharMethod(obj: jobject; methodID: jmethodID; args: array of jvalue): jchar; native;",
    "RemoteVM_CallShortMethod", "Function RemoteVM.CallShortMethod(obj: jobject; methodID: jmethodID; args: array of jvalue): jshort; native;",
    "RemoteVM_CallIntMethod", "Function RemoteVM.CallIntMethod(obj: jobject; methodID: jmethodID; args: array of jvalue): jint; native;",
    "RemoteVM_CallLongMethod", "Function RemoteVM.CallLongMethod(obj: jobject; methodID: jmethodID; args: array of jvalue): jlong; native;",
    "RemoteVM_CallFloatMethod", "Function RemoteVM.CallObjectMethod(obj: jobject; methodID: jmethodID; args: array of jvalue): jfloat; native;",
    "RemoteVM_CallDoubleMethod", "Function RemoteVM.CallObjectMethod(obj: jobject; methodID: jmethodID; args: array of jvalue): jdouble; native;",
    "RemoteVM_CallVoidMethod", "Procedure RemoteVM.CallVoidMethod(obj: jobject; methodID: jmethodID; args: array of jvalue); native;",
    "RemoteVM_CallNonVirtualObjectMethod", "Function RemoteVM.CallNonVirtualObjectMethod(obj: jobject; clazz: jclass; methodID: jmethodID; args: array of jvalue): jobject; native;",
    "RemoteVM_CallNonVirtualBooleanMethod", "Function RemoteVM.CallNonVirtualBooleanMethod(obj: jobject; clazz: jclass; methodID: jmethodID; args: array of jvalue): jboolean; native;",
    "RemoteVM_CallNonVirtualByteMethod", "Function RemoteVM.CallNonVirtualByteMethod(obj: jobject; clazz: jclass; methodID: jmethodID; args: array of jvalue): jbyte; native;",
    "RemoteVM_CallNonVirtualCharMethod", "Function RemoteVM.CallNonVirtualCharMethod(obj: jobject; clazz: jclass; methodID: jmethodID; args: array of jvalue): jchar; native;",
    "RemoteVM_CallNonVirtualShortMethod", "Function RemoteVM.CallNonVirtualShortMethod(obj: jobject; clazz: jclass; methodID: jmethodID; args: array of jvalue): jshort; native;",
    "RemoteVM_CallNonVirtualIntMethod", "Function RemoteVM.CallNonVirtualIntMethod(obj: jobject; clazz: jclass; methodID: jmethodID; args: array of jvalue): jint; native;",
    "RemoteVM_CallNonVirtualLongMethod", "Function RemoteVM.CallNonVirtualLongMethod(obj: jobject; clazz: jclass; methodID: jmethodID; args: array of jvalue): jlong; native;",
    "RemoteVM_CallNonVirtualFloatMethod", "Function RemoteVM.CallNonVirtualObjectMethod(obj: jobject; clazz: jclass; methodID: jmethodID; args: array of jvalue): jfloat; native;",
    "RemoteVM_CallNonVirtualDoubleMethod", "Function RemoteVM.CallNonVirtualObjectMethod(obj: jobject; clazz: jclass; methodID: jmethodID; args: array of jvalue): jdouble; native;",
    "RemoteVM_CallNonVirtualVoidMethod", "Procedure RemoteVM.CallNonVirtualVoidMethod(obj: jobject; clazz: jclass; methodID: jmethodID; args: array of jvalue); native;",
    "RemoteVM_GetFieldID", "Function RemoteVM.GetFieldID(clazz: jclass; name, sig: string): jfieldID; native;",
    "RemoteVM_GetObjectField", "Function RemoteVM.GetObjectField(obj: jobject; fieldID: jfieldID): jobject; native;",
    "RemoteVM_GetBooleanField", "Function RemoteVM.GetBooleanField(obj: jobject; fieldID: jfieldID): jboolean; native;",
    "RemoteVM_GetByteField", "Function RemoteVM.GetByteField(obj: jobject; fieldID: jfieldID): jbyte; native;",
    "RemoteVM_GetCharField", "Function RemoteVM.GetCharField(obj: jobject; fieldID: jfieldID): jchar; native;",
    "RemoteVM_GetShortField", "Function RemoteVM.GetShortField(obj: jobject; fieldID: jfieldID): jshort; native;",
    "RemoteVM_GetIntField", "Function RemoteVM.GetIntField(obj: jobject; fieldID: jfieldID): jint; native;",
    "RemoteVM_GetFloatField", "Function RemoteVM.GetFloatField(obj: jobject; fieldID: jfieldID): jfloat; native;",
    "RemoteVM_GetDoubleField", "Function RemoteVM.GetDoubleField(obj: jobject; fieldID: jfieldID): jdouble; native;",
    "RemoteVM_SetObjectField", "Procedure RemoteVM.SetObjectField(obj: jobject; fieldID: jfieldID; value: jobject); native;",
    "RemoteVM_SetBooleanField", "Procedure RemoteVM.SetBooleanField(obj: jobject; fieldID: jfieldID; value: jboolean); native;",
    "RemoteVM_SetByteField", "Procedure RemoteVM.SetByteField(obj: jobject; fieldID: jfieldID; value: jbyte); native;",
    "RemoteVM_SetCharField", "Procedure RemoteVM.SetCharField(obj: jobject; fieldID: jfieldID; value: jchar); native;",
    "RemoteVM_SetShortField", "Procedure RemoteVM.SetShortField(obj: jobject; fieldID: jfieldID; value: jshort); native;",
    "RemoteVM_SetIntField", "Procedure RemoteVM.SetIntField(obj: jobject; fieldID: jfieldID; value: jint); native;",
    "RemoteVM_SetFloatField", "Procedure RemoteVM.SetFloatField(obj: jobject; fieldID: jfieldID; value: jfloat); native;",
    "RemoteVM_SetDoubleField", "Procedure RemoteVM.SetDoubleField(obj: jobject; fieldID: jfieldID; value: jdouble); native;",
    "RemoteVM_GetStaticMethodID", "Function RemoteVM.GetStaticMethodID(clazz: jclass; name, sig: string): jmethodID; native;",
    "RemoteVM_CallStaticObjectMethod", "Function RemoteVM.CallStaticObjectMethod(clazz: jclass; methodID: jmethodID; args: array of jvalue): jobject; native;",
    "RemoteVM_CallStaticBooleanMethod", "Function RemoteVM.CallStaticBooleanMethod(clazz: jclass; methodID: jmethodID; args: array of jvalue): jboolean; native;",
    "RemoteVM_CallStaticByteMethod", "Function RemoteVM.CallStaticByteMethod(clazz: jclass; methodID: jmethodID; args: array of jvalue): jbyte; native;",
    "RemoteVM_CallStaticCharMethod", "Function RemoteVM.CallStaticCharMethod(clazz: jclass; methodID: jmethodID; args: array of jvalue): jchar; native;",
    "RemoteVM_CallStaticShortMethod", "Function RemoteVM.CallStaticShortMethod(clazz: jclass; methodID: jmethodID; args: array of jvalue): jshort; native;",
    "RemoteVM_CallStaticIntMethod", "Function RemoteVM.CallIntStaticMethod(clazz: jclass; methodID: jmethodID; args: array of jvalue): jint; native;",
    "RemoteVM_CallStaticLongMethod", "Function RemoteVM.CallStaticLongMethod(clazz: jclass; methodID: jmethodID; args: array of jvalue): jlong; native;",
    "RemoteVM_CallStaticFloatMethod", "Function RemoteVM.CallStaticFloatMethod(clazz: jclass; methodID: jmethodID; args: array of jvalue): jfloat; native;",
    "RemoteVM_CallStaticDoubleMethod", "Function RemoteVM.CallStaticDoubleMethod(clazz: jclass; methodID: jmethodID; args: array of jvalue): jdouble; native;",
    "RemoteVM_CallStaticVoidMethod", "Procedure RemoteVM.CallStaticVoidMethod(cls: jclass; methodID: jmethodID; args: array of jvalue); native;",
    "RemoteVM_GetStaticFieldID", "Function RemoteVM.GetStaticFieldID(clazz: jclass; name: string; sign: string): jfieldID; native;",
    "RemoteVM_GetStaticObjectField", "Function RemoteVM.GetStaticObjectField(clazz: jclass; fieldID: jfieldID): jobject; native;",
    "RemoteVM_GetStaticBooleanField", "Function RemoteVM.GetStaticBooleanField(clazz: jclass; fieldID: jfieldID): jboolean; native;",
    "RemoteVM_GetStaticByteField", "Function RemoteVM.GetStaticByteField(clazz: jclass; fieldID: jfieldID): jbyte; native;",
    "RemoteVM_GetStaticCharField", "Function RemoteVM.GetStaticCharField(clazz: jclass; fieldID: jfieldID): jchar; native;",
    "RemoteVM_GetStaticShortField", "Function RemoteVM.GetStaticShortField(clazz: jclass; fieldID: jfieldID): jshort; native;",
    "RemoteVM_GetStaticIntField", "Function RemoteVM.GetStaticIntField(clazz: jclass; fieldID: jfieldID): jint; native;",
    "RemoteVM_GetStaticFloatField", "Function RemoteVM.GetStaticFloatField(clazz: jclass; fieldID: jfieldID): jfloat; native;",
    "RemoteVM_GetStaticDoubleField", "Function RemoteVM.GetStaticDoubleField(clazz: jclass; fieldID: jfieldID): jdouble; native;",
    "RemoteVM_SetStaticObjectField", "Procedure RemoteVM.SetStaticObjectField(clazz: jclass; fieldID: jfieldID; value: jobject); native;",
    "RemoteVM_SetStaticBooleanField", "Procedure RemoteVM.SetStaticBooleanField(clazz: jclass; fieldID: jfieldID; value: jboolean); native;",
    "RemoteVM_SetStaticByteField", "Procedure RemoteVM.SetStaticByteField(clazz: jclass; fieldID: jfieldID; value: jbyte); native;",
    "RemoteVM_SetStaticCharField", "Procedure RemoteVM.SetStaticCharField(clazz: jclass; fieldID: jfieldID; value: jchar); native;",
    "RemoteVM_SetStaticShortField", "Procedure RemoteVM.SetStaticShortField(clazz: jclass; fieldID: jfieldID; value: jshort); native;",
    "RemoteVM_SetStaticIntField", "Procedure RemoteVM.SetStaticIntField(clazz: jclass; fieldID: jfieldID; value: jint); native;",
    "RemoteVM_SetStaticFloatField", "Procedure RemoteVM.SetStaticFloatField(clazz: jclass; fieldID: jfieldID; value: jfloat); native;",
    "RemoteVM_SetStaticDoubleField", "Procedure RemoteVM.SetStaticDoubleField(clazz: jclass; fieldID: jfieldID; value: jdouble);",
    "RemoteVM_NewString", "Function RemoteVM.NewString(unicode_string: widestring): jstring; native;",
    "RemoteVM_GetStringLength", "Function RemoteVM.GetStringLength(str: jstring): jsize; native;",
    "RemoteVM_GetStringChars", "Function RemoteVM.GetStringChars(str: jstring): widestring; native;",
    "RemoteVM_NewStringUTF", "Function RemoteVM.NewStringUTF(utf_string: string): jstring; native;",
    "RemoteVM_GetStringUTFLength", "Function RemoteVM.GetStringUTFLength(str: jstring): jsize; native;",
    "RemoteVM_GetStringUTFChars", "Function RemoteVM_GetStringUTFChars(str: jstring): string; native;",
    "RemoteVM_GetArrayLength", "Function RemoteVM.GetArrayLength(arr: jarray): jsize; native;",
    "RemoteVM_NewObjectArray", "Function RemoteVM.NewObjectArray(len: jsize; clazz: jclass; init: jobject): jobjectArray; native;",
    "RemoteVM_NewBooleanArray", "Function RemoteVM.NewBooleanArray(len: jsize): jbooleanArray; native;",
    "RemoteVM_NewByteArray", "Function RemoteVM.NewByteArray(len: jsize): jbyteArray; native;",
    "RemoteVM_NewCharArray", "Function RemoteVM.NewCharArray(len: jsize): jcharArray; native;",
    "RemoteVM_NewShortArray", "Function RemoteVM.NewShortArray(len: jsize): jshortArray; native;",
    "RemoteVM_NewIntArray", "Function RemoteVM.NewIntArray(len: jsize): jintArray; native;",
    "RemoteVM_NewLongArray", "Function RemoteVM.NewLongArray(len: jsize): jlongArray; native;",
    "RemoteVM_NewFloatArray", "Function RemoteVM.NewFloatArray(len: jsize): jfloatArray; native;",
    "RemoteVM_NewDoubleArray", "Function RemoteVM.NewDoubleArray(len: jsize): jdoubleArray; native;",
    "RemoteVM_GetObjectArrayElements", "Function RemoteVM.GetObjectArrayElements(arr: jobjectArray): array of jobject; native;",
    "RemoteVM_SetObjectArrayElements", "Procedure RemoteVM.SetObjectArrayElements(arr: jobjectArray; index: jsize; elements: array of jobject); native;",
    "RemoteVM_GetBooleanArrayElements", "Function RemoteVM.GetBooleanArrayElements(arr: jbooleanArray): array of jboolean; native;",
    "RemoteVM_GetByteArrayElements", "Function RemoteVM.GetByteArrayElements(arr: jbyteArray): array of jbyte; native;",
    "RemoteVM_GetCharArrayElements", "Function RemoteVM.GetCharArrayElements(arr: jcharArray): array of jchar; native;",
    "RemoteVM_GetShortArrayElements", "Function RemoteVM.GetShortArrayElements(arr: jshortArray): array of jshort; native;",
    "RemoteVM_GetIntArrayElements", "Function RemoteVM.GetIntArrayElements(arr: jintArray): array of jint; native;",
    "RemoteVM_GetLongArrayElements", "Function RemoteVM.GetLongArrayElements(arr: jlongArray): array of jlong; native;",
    "RemoteVM_GetFloatArrayElements", "Function RemoteVM.GetFloatArrayElements(arr: jfloatArray): array of jfloat; native;",
    "RemoteVM_GetDoubleArrayElements", "Function RemoteVM.GetDoubleArrayElements(arr: jdoubleArray): array of jdouble; native;",
    "RemoteVM_GetBooleanArrayRegion", "Procedure RemoteVM.GetBooleanArrayRegion(arr: jbooleanArray; start, len: jsize): array of jboolean; native;",
    "RemoteVM_GetByteArrayRegion", "Procedure RemoteVM.GetByteArrayRegion(arr: jbyteArray; start, len: jsize): array of jbyte; native;",
    "RemoteVM_GetCharArrayRegion", "Procedure RemoteVM.GetCharArrayRegion(arr: jcharArray; start, len: jsize): array of jchar; native;",
    "RemoteVM_GetShortArrayRegion", "Procedure RemoteVM.GetShortArrayRegion(arr: jshortArray; start, len: jsize): array of jshort; native;",
    "RemoteVM_GetIntArrayRegion", "Procedure RemoteVM.GetIntArrayRegion(arr: jintArray; start, len: jsize): array of jint; native;",
    "RemoteVM_GetLongArrayRegion", "Procedure RemoteVM.GetLongArrayRegion(arr: jlongArray; start, len: jsize): array of jlong; native;",
    "RemoteVM_GetFloatArrayRegion", "Procedure RemoteVM.GetFloatArrayRegion(arr: jfloatArray; start, len: jsize): array of jfloat; native;",
    "RemoteVM_GetDoubleArrayRegion", "Procedure RemoteVM.GetDoubleArrayRegion(arr: jdoubleArray; start, len: jsize): array of jboolean; native;",
    "RemoteVM_SetBooleanArrayRegion", "Procedure RemoteVM.SetBooleanArrayRegion(arr: jbooleanArray; start, len: jsize; buf: array of jboolean); native;",
    "RemoteVM_SetByteArrayRegion", "Procedure RemoteVM.SetByteArrayRegion(arr: jbyteArray; start, len: jsize; buf: array of jbyte); native;",
    "RemoteVM_SetCharArrayRegion", "Procedure RemoteVM.SetCharArrayRegion(arr: jcharArray; start, len: jsize; buf: array of jchar); native;",
    "RemoteVM_SetShortArrayRegion", "Procedure RemoteVM.SetShortArrayRegion(arr: jshortArray; start, len: jsize; buf: array of jshort); native;",
    "RemoteVM_SetIntArrayRegion", "Procedure RemoteVM.SetIntArrayRegion(arr: jintArray; start, len: jsize; buf: array of jint); native;",
    "RemoteVM_SetLongArrayRegion", "Procedure RemoteVM.SetLongArrayRegion(arr: jlongArray; start, len: jsize; buf: array of jlong); native;",
    "RemoteVM_SetFloatArrayRegion", "Procedure RemoteVM.SetFloatArrayRegion(arr: jfloatArray; start, len: jsize; buf: array of jfloat); native;",
    "RemoteVM_SetDoubleArrayRegion", "Procedure RemoteVM.SetDoubleArrayRegion(arr: jdoubleArray; start, len: jsize; buf: array of jboolean); native;",
    "RemoteVM_MonitorEnter", "Function RemoteVM.MonitorEnter(obj: jobject): jint; native;",
    "RemoteVM_MonitorExit", "Function RemoteVM.MonitorExit(obj: jobject): jint; native;",
    "RemoteVM_GetJavaVM", "Function RemoteVM.GetJavaVM(): JavaVM; native;",
    "RemoteVM_NewDirectByteBuffer", "Function RemoteVM.NewDirectByteBuffer(address: Pointer; capacity: jlong): jobject; native;",
    "RemoteVM_GetDirectBufferAddress", "Function RemoteVM.GetDirectBufferAddress(buf: jobject): Pointer; native;",
    "RemoteVM_GetDirectBufferCapacity", "Function RemoteVM.GetDirectBufferCapacity(buf: jobject): jlong; native;",
    "RemoteVM_GetObjectRefType", "Function RemoteVM.GetObjectRefType(obj: jobject): jobjectRefType; native;"
};

static const char* PascalTypes[] =
{
	"ReflectionArrayType", "(RI_CHAR = 0, RI_BYTE = 1, RI_BOOLEAN = 2, RI_SHORT = 3, RI_INT = 4, RI_LONG = 5, RI_FLOAT = 6, RI_DOUBLE = 7, RI_STRING = 8, RI_OBJECT = 9)",
	"RIField", "packed record cls, field, desc: String; end;",

	//JVM Types
    "jint", "Int32",
    "jlong", "Int64",
    "jbyte", "UInt8",
    "jboolean", "ByteBool",
    "jchar", "UInt16",
    "jshort", "Int16",
    "jfloat", "Single",
    "jdouble", "Double",
    "jsize", "jint",

    "jobject", "type Pointer",
    "jclass", "type jobject",
    "jthrowable", "type jobject",
    "jstring", "type jobject",
    "jarray", "type jobject",

    "jbooleanArray", "type jarray",
    "jbyteArray", "type jarray",
    "jcharArray", "type jarray",
    "jshortArray", "type jarray",
    "jintArray", "type jarray",
    "jlongArray", "type jarray",
    "jfloatArray", "type jarray",
    "jdoubleArray", "type jarray",
    "jobjectArray", "type jarray",

    "jweak", "type jobject",
    "jfieldID", "type Pointer",
    "jmethodID", "type Pointer",

    "jvalue", "union z: jboolean; b: jbyte; c: jchar; s: jshort; i: jint; j: jlong; f: jfloat; d: jdouble; l: jobject; end",
    "jobjectRefType", "(JNIInvalidRefType = 0, JNILocalRefType = 1, JNIGlobalRefType = 2, JNIWeakGlobalRefType = 3)",

    "RemoteVM", "type TObject"
};

static const long int PascalExportCount = sizeof(PascalExports) / (sizeof(PascalExports[0]) * 2);
static const long int PascalTypeCount = sizeof(PascalTypes) / (sizeof(PascalTypes[0]) * 2);


#ifdef __cplusplus
extern "C"
{
#endif

EXPORT int GetPluginABIVersion() noexcept;
EXPORT int GetFunctionCount() noexcept;
EXPORT int GetTypeCount() noexcept;
EXPORT int GetFunctionInfo(int Index, void** Address, char** Definition) noexcept;
EXPORT int GetTypeInfo(int Index, char** Type, char** Definition) noexcept;

#if defined(PASCAL_CALLING_CONVENTION)
EXPORT void SetPluginMemManager(TMemoryManager MemMgr) noexcept;
#endif

EXPORT void SetPluginSimbaMethods(TSimbaMethods Methods) noexcept;
EXPORT void SetPluginSimbaMemoryAllocators(TSimbaMemoryAllocators Allocators) noexcept;
EXPORT void RegisterSimbaPlugin(TSimbaInfomation* Information, TSimbaMethodsExtended* Methods) noexcept;
EXPORT void OnAttach(void* info) noexcept;
EXPORT void OnDetach() noexcept;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif

EXPORT void Pascal_Reflect_Equal(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_InstanceOf(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Object(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Release_Object(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Release_Objects(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Boolean(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Char(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Byte(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Short(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Int(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Long(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Float(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Double(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_String(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Array(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Array_With_Size(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Array_Size(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Array_Index_Size(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Array_Indices(void** Params, void** Result) noexcept;

EXPORT void Pascal_Reflect_Array_SingleIndex(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Array_SingleIndex2D(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Array_SingleIndex3D(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Array_SingleIndex4D(void** Params, void** Result) noexcept;

EXPORT void Pascal_Reflect_Array_Index(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Array_Index2D(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Array_Index3D(void** Params, void** Result) noexcept;
EXPORT void Pascal_Reflect_Array_Index4D(void** Params, void** Result) noexcept;

EXPORT void Pascal_GetDebugImageBuffer(void** Params, void** Result) noexcept;
EXPORT void Pascal_SetGraphicsDebugging(void** Params, void** Result) noexcept;
EXPORT void Pascal_PairClient(void** Params, void** Result) noexcept;
EXPORT void Pascal_KillClientPID(void** Params, void** Result) noexcept;
EXPORT void Pascal_KillClient(void** Params, void** Result) noexcept;
EXPORT void Pascal_GetClients(void** Params, void** Result) noexcept;
EXPORT void Pascal_GetClientPID(void** Params, void** Result) noexcept;

EXPORT void Pascal_Inject(void** Params, void** Result) noexcept;
EXPORT void Pascal_Inject_PID(void** Params, void** Result) noexcept;

EXPORT void Pascal_HasFocus(void** Params, void** Result) noexcept;
EXPORT void Pascal_GainFocus(void** Params, void** Result) noexcept;
EXPORT void Pascal_LoseFocus(void** Params, void** Result) noexcept;
EXPORT void Pascal_IsInputEnabled(void** Params, void** Result) noexcept;
EXPORT void Pascal_SetInputEnabled(void** Params, void** Result) noexcept;
EXPORT void Pascal_GetRealMousePosition(void** Params, void** Result) noexcept;

EXPORT void Pascal_GetKeyboardSpeed(void** Params, void** Result) noexcept;
EXPORT void Pascal_SetKeyboardSpeed(void** Params, void** Result) noexcept;
EXPORT void Pascal_GetKeyboardRepeatDelay(void** Params, void** Result) noexcept;
EXPORT void Pascal_SetKeyboardRepeatDelay(void** Params, void** Result) noexcept;

// MARK: - RemoteVM
EXPORT void Pascal_RemoteVM_MaxMemoryChunkSize(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_AllocateMemory(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_ReadMemory(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_WriteMemory(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_FreeMemory(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetVersion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_DefineClass(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_FindClass(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_FromReflectedMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_FromReflectedField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_ToReflectedMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetSuperclass(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_IsAssignableFrom(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_ToReflectedField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_Throw(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_ThrowNew(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetExceptionMessage(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_FatalError(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_DeleteGlobalRef(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_IsSameObject(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_AllocObject(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_NewObject(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetObjectClass(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_IsInstanceOf(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetMethodID(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallObjectMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallBooleanMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallByteMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallCharMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallShortMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallIntMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallLongMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallFloatMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallDoubleMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallVoidMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallNonvirtualObjectMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallNonvirtualBooleanMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallNonvirtualByteMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallNonvirtualCharMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallNonvirtualShortMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallNonvirtualIntMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallNonvirtualLongMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallNonvirtualFloatMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallNonvirtualDoubleMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallNonvirtualVoidMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetFieldID(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetObjectField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetBooleanField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetByteField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetCharField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetShortField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetIntField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetLongField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetFloatField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetDoubleField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetObjectField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetBooleanField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetByteField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetCharField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetShortField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetIntField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetLongField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetFloatField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetDoubleField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStaticMethodID(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallStaticObjectMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallStaticBooleanMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallStaticByteMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallStaticCharMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallStaticShortMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallStaticIntMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallStaticLongMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallStaticFloatMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallStaticDoubleMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_CallStaticVoidMethod(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStaticFieldID(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStaticObjectField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStaticBooleanField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStaticByteField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStaticCharField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStaticShortField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStaticIntField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStaticLongField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStaticFloatField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStaticDoubleField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetStaticObjectField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetStaticBooleanField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetStaticByteField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetStaticCharField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetStaticShortField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetStaticIntField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetStaticLongField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetStaticFloatField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetStaticDoubleField(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_NewString(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStringLength(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStringChars(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_NewStringUTF(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStringUTFLength(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetStringUTFChars(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetArrayLength(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_NewObjectArray(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_NewBooleanArray(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_NewByteArray(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_NewCharArray(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_NewShortArray(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_NewIntArray(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_NewLongArray(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_NewFloatArray(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_NewDoubleArray(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetObjectArrayElements(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetObjectArrayElements(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetBooleanArrayElements(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetByteArrayElements(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetCharArrayElements(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetShortArrayElements(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetIntArrayElements(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetLongArrayElements(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetFloatArrayElements(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetDoubleArrayElements(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetBooleanArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetByteArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetCharArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetShortArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetIntArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetLongArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetFloatArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetDoubleArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetBooleanArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetByteArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetCharArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetShortArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetIntArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetLongArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetFloatArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_SetDoubleArrayRegion(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_MonitorEnter(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_MonitorExit(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetJavaVM(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_NewDirectByteBuffer(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetDirectBufferAddress(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetDirectBufferCapacity(void** Params, void** Result);
EXPORT void Pascal_RemoteVM_GetObjectRefType(void** Params, void** Result);

#ifdef __cplusplus
}
#endif

#endif // SIMBAPLUGIN_HXX_INCLUDED
