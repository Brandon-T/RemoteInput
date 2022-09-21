//
// Created by Brandon on 2022-09-21.
//

#ifndef REMOTEINPUT_NATIVEPLUGIN_HXX
#define REMOTEINPUT_NATIVEPLUGIN_HXX

#include <cstdint>
#include <cstdio>
#include <new>

#include "EIOS.hxx"

#ifdef __cplusplus
extern "C"
{
#endif

EXPORT void EIOS_Inject(const char* process_name) noexcept;
EXPORT void EIOS_Inject_PID(std::int32_t pid) noexcept;
EXPORT EIOS* EIOS_From_PID(std::int32_t pid) noexcept;
EXPORT jobject Reflect_Object(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept;
EXPORT jboolean Reflect_IsSame_Object(EIOS* eios, jobject first, jobject second) noexcept;
EXPORT jboolean Reflect_InstanceOf(EIOS* eios, jobject object, const char* cls) noexcept;
EXPORT void Reflect_Release_Object(EIOS* eios, jobject object) noexcept;
EXPORT void Reflect_Release_Objects(EIOS* eios, jobject* objects, std::size_t amount) noexcept;
EXPORT bool Reflect_Bool(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept;
EXPORT char Reflect_Char(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept;
EXPORT std::uint8_t Reflect_Byte(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept;
EXPORT std::int16_t Reflect_Short(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept;
EXPORT std::int32_t Reflect_Int(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept;
EXPORT std::int64_t Reflect_Long(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept;
EXPORT float Reflect_Float(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept;
EXPORT double Reflect_Double(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept;
EXPORT void Reflect_String(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc, char* output, std::size_t output_size) noexcept;
EXPORT jarray Reflect_Array(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept;
EXPORT jarray Reflect_Array_With_Size(EIOS* eios, jobject object, std::size_t* output_size, const char* cls, const char* field, const char* desc) noexcept;
EXPORT jarray Reflect_Array_With_Size2D(EIOS* eios, jobject object, std::size_t x, std::size_t* output_size, const char* cls, const char* field, const char* desc) noexcept;
EXPORT jarray Reflect_Array_With_Size3D(EIOS* eios, jobject object, std::size_t x, std::size_t y, std::size_t* output_size, const char* cls, const char* field, const char* desc) noexcept;
EXPORT jarray Reflect_Array_With_Size4D(EIOS* eios, jobject object, std::size_t x, std::size_t y, std::size_t z, std::size_t* output_size, const char* cls, const char* field, const char* desc) noexcept;
EXPORT std::size_t Reflect_Array_Size(EIOS* eios, jarray array) noexcept;
EXPORT void* Reflect_Array_Index(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t index, std::size_t length) noexcept;
EXPORT void* Reflect_Array_Index2D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t x, std::size_t y, std::size_t length) noexcept;
EXPORT void* Reflect_Array_Index3D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t x, std::size_t y, std::size_t z, std::size_t length) noexcept;
EXPORT void* Reflect_Array_Index4D(EIOS* eios, jarray array, ReflectionArrayType type, std::size_t x, std::size_t y, std::size_t z, std::size_t w, std::size_t length) noexcept;
EXPORT void* Reflect_Array_Indices(EIOS* eios, jarray array, ReflectionArrayType type, std::int32_t* indices, std::size_t length) noexcept;

#ifdef __cplusplus
}
#endif

#endif //REMOTEINPUT_NATIVEPLUGIN_HXX
