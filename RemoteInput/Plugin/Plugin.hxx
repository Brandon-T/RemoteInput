//
//  Plugin.hxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-21.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef PLUGIN_HXX_INCLUDED
#define PLUGIN_HXX_INCLUDED

#include <cstdint>
#include <cstdio>
#include <new>

#include "EIOS.hxx"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
extern HMODULE module;
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
EXPORT jarray Reflect_Array_With_Size(EIOS* eios, jobject object, std::size_t* output_size, const char* cls, const char* field, const char* desc);
EXPORT std::size_t Reflect_Array_Size(EIOS* eios, jarray array);

EXPORT void Reflect_Array_Index(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t index, std::size_t length, void* buffer);

EXPORT void Reflect_Array_Index2D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, void* buffer);

EXPORT void Reflect_Array_Index3D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z, void* buffer);

EXPORT void Reflect_Array_Index4D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t w, void* buffer);

#ifdef __cplusplus
}
#endif

#endif /* PLUGIN_HXX_INCLUDED */
