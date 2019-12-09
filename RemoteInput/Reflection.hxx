#ifndef REFLECTION_HXX_INCLUDED
#define REFLECTION_HXX_INCLUDED

#include <string>
#include <memory>
#include <functional>
#include <jni.h>
#include "JVM.hxx"
#include "ReflectionHook.hxx"

class Reflection final
{
private:
	#define HANDLE_JAVA_EXCEPTION(RESULT) \
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
	
    template<typename T>
    auto make_safe(jobject object) -> std::unique_ptr<typename std::remove_pointer<T>::type, std::function<void(T)>>
    {
		auto deleter = [&](T ptr){
            jvm->DeleteGlobalRef(static_cast<jobject>(ptr));
        };

        return std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>{static_cast<T>(object), deleter};
    }

    JVM* jvm;
    jobject frame;
    jobject applet;
    jobject classLoader;

public:
    void PrintClasses();

public:
    Reflection();
    Reflection(JNIEnv* env);
    ~Reflection();

    bool Initialize(jobject awtFrame);

    bool Attach();
	bool AttachAsDaemon();
    bool Detach();

    std::string GetClassName(jobject object);
    std::string GetClassType(jobject object);
    jclass LoadClass(const char* clsToLoad);

	JVM* getVM();
    JNIEnv* getEnv();

    template<typename T>
    typename std::enable_if<std::is_same<std::string, typename std::remove_cv<T>::type>::value, T>::type
    getField(jobject object, ReflectionHook hook);

    template<typename T>
    typename std::enable_if<!std::is_same<std::string, typename std::remove_cv<T>::type>::value, T>::type
    getField(jobject object, ReflectionHook hook);

    template<typename T>
    typename std::enable_if<!std::is_same<std::string, typename std::remove_cv<T>::type>::value, std::unique_ptr<typename std::remove_pointer<T>::type, std::function<void(T)>>>::type
    getFieldSafe(jobject object, ReflectionHook hook);

    template<typename T>
    typename std::enable_if<std::is_same<jint, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, ReflectionHook hook);

    template<typename T>
    typename std::enable_if<std::is_same<jlong, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, ReflectionHook hook);

    template<typename T>
    typename std::enable_if<std::is_same<jboolean, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, ReflectionHook hook);

	template<typename T>
    typename std::enable_if<std::is_same<jbyte, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, ReflectionHook hook);

    template<typename T>
    typename std::enable_if<std::is_same<jchar, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, ReflectionHook hook);

    template<typename T>
    typename std::enable_if<std::is_same<jshort, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, ReflectionHook hook);

    template<typename T>
    typename std::enable_if<std::is_same<jfloat, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, ReflectionHook hook);

    template<typename T>
    typename std::enable_if<std::is_same<jdouble, typename std::remove_cv<T>::type>::value, T>::type
    getPrimitive(jobject object, ReflectionHook hook);
};




template<typename T>
typename std::enable_if<std::is_same<std::string, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getField(jobject object, ReflectionHook hook)
{
    if (!object)
    {
        jclass cls = this->LoadClass(hook.cls.c_str());
        jfieldID field = jvm->GetStaticFieldID(cls, hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(std::string());
        jobject string = jvm->GetStaticObjectField(cls, field);
		HANDLE_JAVA_EXCEPTION(std::string());

        if (string)
        {
            const char* chars = jvm->GetStringUTFChars(static_cast<jstring>(string), nullptr);
            std::string result = chars;
            jvm->ReleaseStringUTFChars(static_cast<jstring>(string), chars);
            return result;
        }
        return std::string();
    }

    jclass cls = this->LoadClass(hook.cls.c_str());
    jfieldID field = jvm->GetFieldID(cls, hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(std::string());
    jobject string = jvm->GetObjectField(object, field);
	HANDLE_JAVA_EXCEPTION(std::string());

    if (string)
    {
        const char* chars = jvm->GetStringUTFChars(static_cast<jstring>(string), nullptr);
        std::string result = chars;
        jvm->ReleaseStringUTFChars(static_cast<jstring>(string), chars);
        return result;
    }
    return std::string();
}


template<typename T>
typename std::enable_if<!std::is_same<std::string, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getField(jobject object, ReflectionHook hook)
{
    if (!object)
    {
        jclass cls = this->LoadClass(hook.cls.c_str());
        jfieldID field = jvm->GetStaticFieldID(cls, hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(nullptr);
        return static_cast<T>(jvm->NewGlobalRef(jvm->GetStaticObjectField(cls, field)));
    }

    jclass cls = this->LoadClass(hook.cls.c_str());
    jfieldID field = jvm->GetFieldID(cls, hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(nullptr);
    return static_cast<T>(jvm->NewGlobalRef(jvm->GetObjectField(object, field)));
}

template<typename T>
typename std::enable_if<!std::is_same<std::string, typename std::remove_cv<T>::type>::value, std::unique_ptr<typename std::remove_pointer<T>::type, std::function<void(T)>>>::type
Reflection::getFieldSafe(jobject object, ReflectionHook hook)
{
    if (!object)
    {
        jclass cls = this->LoadClass(hook.cls.c_str());
        jfieldID field = jvm->GetStaticFieldID(cls, hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(nullptr);
        return make_safe<T>(jvm->NewGlobalRef(jvm->GetStaticObjectField(cls, field)));
    }

    jclass cls = this->LoadClass(hook.cls.c_str());
    jfieldID field = jvm->GetFieldID(cls, hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(nullptr);
    return make_safe<T>(jvm->NewGlobalRef(jvm->GetObjectField(object, field)));
}

template<typename T>
typename std::enable_if<std::is_same<jint, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        jclass cls = this->LoadClass(hook.cls.c_str());
        jfieldID field = jvm->GetFieldID(cls, hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(-1);
		return jvm->GetIntField(object, field); //* static_cast<std::int32_t>(hook.multiplier);
    }

    jclass cls = this->LoadClass(hook.cls.c_str());
    jfieldID field = jvm->GetStaticFieldID(cls, hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(-1);
	return jvm->GetStaticIntField(cls, field); // * static_cast<std::int32_t>(hook.multiplier);
}

template<typename T>
typename std::enable_if<std::is_same<jlong, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        jclass cls = this->LoadClass(hook.cls.c_str());
        jfieldID field = jvm->GetFieldID(cls, hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(-1);
		return jvm->GetLongField(object, field); // * static_cast<std::int64_t>(hook.multiplier);
    }

    jclass cls = this->LoadClass(hook.cls.c_str());
    jfieldID field = jvm->GetStaticFieldID(cls, hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(-1);
	return jvm->GetStaticLongField(cls, field); // * static_cast<std::int64_t>(hook.multiplier);
}

template<typename T>
typename std::enable_if<std::is_same<jboolean, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        jclass cls = this->LoadClass(hook.cls.c_str());
        jfieldID field = jvm->GetFieldID(cls, hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(false);
        return jvm->GetBooleanField(object, field);
    }

    jclass cls = this->LoadClass(hook.cls.c_str());
    jfieldID field = jvm->GetStaticFieldID(cls, hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(false);
    return jvm->GetStaticBooleanField(cls, field);
}

template<typename T>
typename std::enable_if<std::is_same<jbyte, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        jclass cls = this->LoadClass(hook.cls.c_str());
        jfieldID field = jvm->GetFieldID(cls, hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(0);
        return jvm->GetByteField(object, field);
    }

    jclass cls = this->LoadClass(hook.cls.c_str());
    jfieldID field = jvm->GetStaticFieldID(cls, hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(0);
    return jvm->GetStaticByteField(cls, field);
}

template<typename T>
typename std::enable_if<std::is_same<jchar, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        jclass cls = this->LoadClass(hook.cls.c_str());
        jfieldID field = jvm->GetFieldID(cls, hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION('\0');
        return jvm->GetCharField(object, field);
    }

    jclass cls = this->LoadClass(hook.cls.c_str());
    jfieldID field = jvm->GetStaticFieldID(cls, hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION('\0');
    return jvm->GetStaticCharField(cls, field);
}

template<typename T>
typename std::enable_if<std::is_same<jshort, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        jclass cls = this->LoadClass(hook.cls.c_str());
        jfieldID field = jvm->GetFieldID(cls, hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(-1);
        return jvm->GetShortField(object, field);
    }

    jclass cls = this->LoadClass(hook.cls.c_str());
    jfieldID field = jvm->GetStaticFieldID(cls, hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(-1);
    return jvm->GetStaticShortField(cls, field);
}

template<typename T>
typename std::enable_if<std::is_same<jfloat, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        jclass cls = this->LoadClass(hook.cls.c_str());
        jfieldID field = jvm->GetFieldID(cls, hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(-1.0);
        return jvm->GetFloatField(object, field);
    }

    jclass cls = this->LoadClass(hook.cls.c_str());
    jfieldID field = jvm->GetStaticFieldID(cls, hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(-1.0);
    return jvm->GetStaticFloatField(cls, field);
}

template<typename T>
typename std::enable_if<std::is_same<jdouble, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        jclass cls = this->LoadClass(hook.cls.c_str());
        jfieldID field = jvm->GetFieldID(cls, hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(-1.0);
        return jvm->GetDoubleField(object, field);
    }

    jclass cls = this->LoadClass(hook.cls.c_str());
    jfieldID field = jvm->GetStaticFieldID(cls, hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(-1.0);
    return jvm->GetStaticDoubleField(cls, field);
}

#endif // REFLECTION_HXX_INCLUDED
