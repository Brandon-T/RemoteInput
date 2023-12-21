#ifndef REFLECTION_HXX_INCLUDED
#define REFLECTION_HXX_INCLUDED

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <cstdint>
#include <string_view>
#include "JNI_Common.hxx"
#include "JVM.hxx"
#include "ReflectionHook.hxx"
#include "JVMCache.hxx"

enum class ReflectionType: std::uint8_t
{
    CHAR,
    BYTE,
    BOOL,
    SHORT,
    INT,
    LONG,
    FLOAT,
    DOUBLE,
    STRING,
    OBJECT,
    ARRAY
};

class Reflection final
{
private:
    #define HANDLE_JAVA_CLASS_NOT_FOUND(RESULT) \
    if (!cls)\
    {\
        fprintf(stderr, "No Such Class %s Field: %s.%s -> %s\n", object ? "Instance" : "Static", hook.cls.c_str(), hook.field.c_str(), hook.desc.c_str());\
        jvm->ExceptionClear();\
        return RESULT;\
    }\
    else if (jvm->ExceptionCheck())\
    {\
        jvm->ExceptionDescribe();\
        jvm->ExceptionClear();\
        return RESULT;\
    }

    #define HANDLE_JAVA_FIELD_NOT_FOUND(RESULT) \
    if (!field)\
    {\
        fprintf(stderr, "No Such %s Field: %s.%s -> %s\n", object ? "Instance" : "Static", hook.cls.c_str(), hook.field.c_str(), hook.desc.c_str());\
        jvm->ExceptionClear();\
        return RESULT;\
    }\
    else if (jvm->ExceptionCheck())\
    {\
        jvm->ExceptionDescribe();\
        jvm->ExceptionClear();\
        return RESULT;\
    }

    template<typename T, typename U>
    auto make_safe_local(U object) const noexcept
    {
        auto deleter = [&](T ptr) {
            if (jvm && ptr)
            {
                jvm->DeleteLocalRef(static_cast<jobject>(ptr));
            }
        };

        return std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>{static_cast<T>(object), deleter};
    }

    template<typename T, typename U>
    T promote_to_global(U object) const noexcept
    {
        jvm->DeleteLocalRef(std::exchange(object, jvm->NewGlobalRef(static_cast<jobject>(object))));
        return static_cast<T>(object);
    }

    JVM* jvm;
    jobject frame;
    jobject applet;
    jobject classLoader;
    JVMCache cache;

    void PrintClasses() const noexcept;
    jfieldID GetFieldID(jclass cls, std::string_view name, std::string_view desc, bool is_static) noexcept;

public:
    Reflection() noexcept;
    Reflection(JNIEnv* env) noexcept;
    Reflection(Reflection&& other) noexcept;
    Reflection(const Reflection& other) = delete;
    ~Reflection() noexcept;

    Reflection& operator = (const Reflection& other) = delete;
    Reflection& operator = (Reflection&& other) noexcept;

    bool Initialize(jobject awtFrame) noexcept;

    bool Attach() const noexcept;
    bool AttachAsDaemon() const noexcept;
    bool Detach() const noexcept;

    std::string GetClassName(jobject object) const noexcept;
    std::string GetClassType(jobject object) const noexcept;
    bool IsDecendentOf(jobject object, const char* className) const noexcept;

    jclass LoadClass(std::string_view name) noexcept;
    void ClearCache() noexcept;


    jobject getApplet() const noexcept;

    JVM* getVM() const noexcept;
    JNIEnv* getEnv() const noexcept;

    template<typename T>
    typename std::enable_if<std::is_same<std::string, typename std::remove_cv<T>::type>::value, T>::type
    getField(jstring object) noexcept;

    template<typename T>
    typename std::enable_if<std::is_same<std::string, typename std::remove_cv<T>::type>::value, T>::type
    getField(jobject object, const ReflectionHook &hook) noexcept;

    template<typename T>
    typename std::enable_if<!std::is_same<std::string, typename std::remove_cv<T>::type>::value, T>::type
    getField(jobject object, const ReflectionHook &hook) noexcept;

    template<typename T>
    typename std::enable_if<std::is_same<jint, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, const ReflectionHook &hook) noexcept;

    template<typename T>
    typename std::enable_if<std::is_same<jlong, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, const ReflectionHook &hook) noexcept;

    template<typename T>
    typename std::enable_if<std::is_same<jboolean, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, const ReflectionHook &hook) noexcept;

    template<typename T>
    typename std::enable_if<std::is_same<jbyte, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, const ReflectionHook &hook) noexcept;

    template<typename T>
    typename std::enable_if<std::is_same<jchar, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, const ReflectionHook &hook) noexcept;

    template<typename T>
    typename std::enable_if<std::is_same<jshort, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, const ReflectionHook &hook) noexcept;

    template<typename T>
    typename std::enable_if<std::is_same<jfloat, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, const ReflectionHook &hook) noexcept;

    template<typename T>
    typename std::enable_if<std::is_same<jdouble, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, const ReflectionHook &hook) noexcept;
};


template<typename T>
typename std::enable_if<std::is_same<std::string, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getField(jstring string) noexcept
{
    if (string)
    {
        jsize length = jvm->GetStringUTFLength(string);
        if (length > 0)
        {
            const char* chars = jvm->GetStringUTFChars(string, nullptr);
            if (chars)
            {
                std::string result = std::string(chars, length);
                jvm->ReleaseStringUTFChars(string, chars);
                return result;
            }
        }
    }
    return std::string();
}

template<typename T>
typename std::enable_if<std::is_same<std::string, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getField(jobject object, const ReflectionHook &hook) noexcept
{
    jclass cls = LoadClass(hook.cls);
    HANDLE_JAVA_CLASS_NOT_FOUND(std::string());
    jfieldID field = GetFieldID(cls, hook.field, hook.desc, !object);
    HANDLE_JAVA_FIELD_NOT_FOUND(std::string());
    auto string = make_safe_local<jstring>(object ? jvm->GetObjectField(object, field) : jvm->GetStaticObjectField(cls, field));
    HANDLE_JAVA_FIELD_NOT_FOUND(std::string());
    return getField<T>(string.get());
}


template<typename T>
typename std::enable_if<!std::is_same<std::string, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getField(jobject object, const ReflectionHook &hook) noexcept
{
    jclass cls = LoadClass(hook.cls);
    HANDLE_JAVA_CLASS_NOT_FOUND(nullptr);
    jfieldID field = GetFieldID(cls, hook.field, hook.desc, !object);
    HANDLE_JAVA_FIELD_NOT_FOUND(nullptr);
    return promote_to_global<T>(object ? jvm->GetObjectField(object, field) : jvm->GetStaticObjectField(cls, field));
}

template<typename T>
typename std::enable_if<std::is_same<jint, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, const ReflectionHook &hook) noexcept
{
    jclass cls = LoadClass(hook.cls);
    HANDLE_JAVA_CLASS_NOT_FOUND(-1);
    jfieldID field = GetFieldID(cls, hook.field, hook.desc, !object);
    HANDLE_JAVA_FIELD_NOT_FOUND(-1);
    return object ? jvm->GetIntField(object, field) : jvm->GetStaticIntField(cls, field);
}

template<typename T>
typename std::enable_if<std::is_same<jlong, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, const ReflectionHook &hook) noexcept
{
    jclass cls = LoadClass(hook.cls);
    HANDLE_JAVA_CLASS_NOT_FOUND(-1);
    jfieldID field = GetFieldID(cls, hook.field, hook.desc, !object);
    HANDLE_JAVA_FIELD_NOT_FOUND(-1);
    return object ? jvm->GetLongField(object, field) : jvm->GetStaticLongField(cls, field);
}

template<typename T>
typename std::enable_if<std::is_same<jboolean, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, const ReflectionHook &hook) noexcept
{
    jclass cls = this->LoadClass(hook.cls);
    HANDLE_JAVA_CLASS_NOT_FOUND(false);
    jfieldID field = GetFieldID(cls, hook.field, hook.desc, !object);
    HANDLE_JAVA_FIELD_NOT_FOUND(false);
    return object ? jvm->GetBooleanField(object, field) : jvm->GetStaticBooleanField(cls, field);
}

template<typename T>
typename std::enable_if<std::is_same<jbyte, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, const ReflectionHook &hook) noexcept
{
    jclass cls = LoadClass(hook.cls);
    HANDLE_JAVA_CLASS_NOT_FOUND(0);
    jfieldID field = GetFieldID(cls, hook.field, hook.desc, !object);
    HANDLE_JAVA_FIELD_NOT_FOUND(0);
    return object ? jvm->GetByteField(object, field) : jvm->GetStaticByteField(cls, field);
}

template<typename T>
typename std::enable_if<std::is_same<jchar, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, const ReflectionHook &hook) noexcept
{
    jclass cls = LoadClass(hook.cls.c_str());
    HANDLE_JAVA_CLASS_NOT_FOUND('\0');
    jfieldID field = GetFieldID(cls, hook.field.c_str(), hook.desc.c_str(), !object);
    HANDLE_JAVA_FIELD_NOT_FOUND('\0');
    return object ? jvm->GetCharField(object, field) : jvm->GetStaticCharField(cls, field);
}

template<typename T>
typename std::enable_if<std::is_same<jshort, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, const ReflectionHook &hook) noexcept
{
    jclass cls = LoadClass(hook.cls);
    HANDLE_JAVA_CLASS_NOT_FOUND(-1);
    jfieldID field = GetFieldID(cls, hook.field, hook.desc, !object);
    HANDLE_JAVA_FIELD_NOT_FOUND(-1);
    return object ? jvm->GetShortField(object, field) : jvm->GetStaticShortField(cls, field);
}

template<typename T>
typename std::enable_if<std::is_same<jfloat, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, const ReflectionHook &hook) noexcept
{
    jclass cls = LoadClass(hook.cls);
    HANDLE_JAVA_CLASS_NOT_FOUND(-1.0);
    jfieldID field = GetFieldID(cls, hook.field, hook.desc, !object);
    HANDLE_JAVA_FIELD_NOT_FOUND(-1.0);
    return object ? jvm->GetFloatField(object, field) : jvm->GetStaticFloatField(cls, field);
}

template<typename T>
typename std::enable_if<std::is_same<jdouble, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, const ReflectionHook &hook) noexcept
{
    jclass cls = LoadClass(hook.cls);
    HANDLE_JAVA_CLASS_NOT_FOUND(-1.0);
    jfieldID field = GetFieldID(cls, hook.field, hook.desc, !object);
    HANDLE_JAVA_FIELD_NOT_FOUND(-1.0);
    return object ? jvm->GetDoubleField(object, field) : jvm->GetStaticDoubleField(cls, field);
}

#endif // REFLECTION_HXX_INCLUDED
