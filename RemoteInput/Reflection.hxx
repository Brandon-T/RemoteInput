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

	template<typename T, typename U>
    auto make_safe_local(U object) -> std::unique_ptr<typename std::remove_pointer<T>::type, std::function<void(T)>>
    {
		auto deleter = [&](T ptr){
            jvm->DeleteLocalRef(static_cast<jobject>(ptr));
        };

        return std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>{static_cast<T>(object), deleter};
    }

    JVM* jvm;
    jobject frame;
    jobject applet;
    jobject classLoader;

    void PrintClasses();

public:
    Reflection();
    Reflection(JNIEnv* env);
    Reflection(const Reflection& other) = delete;
    Reflection(Reflection&& other);
    ~Reflection();

    Reflection& operator = (const Reflection& other) = delete;
    Reflection& operator = (Reflection&& other);

    bool Initialize(jobject awtFrame);

    bool Attach();
	bool AttachAsDaemon();
    bool Detach();

    std::string GetClassName(jobject object);
    std::string GetClassType(jobject object);
    jclass LoadClass(const char* clsToLoad);

	jobject getApplet();

	JVM* getVM();
    JNIEnv* getEnv();

	template<typename T>
    typename std::enable_if<std::is_same<std::string, typename std::remove_cv<T>::type>::value, T>::type
    getField(jstring object);

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
Reflection::getField(jstring string)
{
    if (string)
    {
		jsize length = jvm->GetStringUTFLength(string);
        const char* chars = jvm->GetStringUTFChars(string, nullptr);
        std::string result = std::string(chars, length);
        jvm->ReleaseStringUTFChars(string, chars);
        return result;
    }
    return std::string();
}

template<typename T>
typename std::enable_if<std::is_same<std::string, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getField(jobject object, ReflectionHook hook)
{
    if (!object)
    {
        auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
		HANDLE_JAVA_CLASS_NOT_FOUND(std::string());
        jfieldID field = jvm->GetStaticFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(std::string());
        auto string = make_safe_local<jstring>(jvm->GetStaticObjectField(cls.get(), field));
		HANDLE_JAVA_EXCEPTION(std::string());

        if (string)
        {
			jsize length = jvm->GetStringUTFLength(string.get());
			if (length > 0)
			{
				const char* chars = jvm->GetStringUTFChars(string.get(), nullptr);
				if (chars)
				{
					std::string result = std::string(chars, length);
					jvm->ReleaseStringUTFChars(string.get(), chars);
					return result;
				}
			}
        }
        return std::string();
    }

    auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
	HANDLE_JAVA_CLASS_NOT_FOUND(std::string());
    jfieldID field = jvm->GetFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(std::string());
    auto string = make_safe_local<jstring>(jvm->GetObjectField(object, field));
	HANDLE_JAVA_EXCEPTION(std::string());

    if (string)
    {
		jsize length = jvm->GetStringUTFLength(string.get());
		if (length > 0)
		{
			const char* chars = jvm->GetStringUTFChars(string.get(), nullptr);
			if (chars)
			{
				std::string result = std::string(chars, length);
				jvm->ReleaseStringUTFChars(string.get(), chars);
				return result;
			}
		}
    }
    return std::string();
}


template<typename T>
typename std::enable_if<!std::is_same<std::string, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getField(jobject object, ReflectionHook hook)
{
    if (!object)
    {
        auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
		HANDLE_JAVA_CLASS_NOT_FOUND(nullptr);
        jfieldID field = jvm->GetStaticFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(nullptr);
        return static_cast<T>(jvm->NewGlobalRef(make_safe_local<T>(jvm->GetStaticObjectField(cls.get(), field)).get()));
    }

    auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
	HANDLE_JAVA_CLASS_NOT_FOUND(nullptr);
    jfieldID field = jvm->GetFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(nullptr);
    return static_cast<T>(jvm->NewGlobalRef(make_safe_local<T>(jvm->GetObjectField(object, field)).get()));
}

template<typename T>
typename std::enable_if<!std::is_same<std::string, typename std::remove_cv<T>::type>::value, std::unique_ptr<typename std::remove_pointer<T>::type, std::function<void(T)>>>::type
Reflection::getFieldSafe(jobject object, ReflectionHook hook)
{
    if (!object)
    {
        auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
		HANDLE_JAVA_CLASS_NOT_FOUND(nullptr);
        jfieldID field = jvm->GetStaticFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(nullptr);
        return make_safe<T>(jvm->NewGlobalRef(make_safe_local<T>(jvm->GetStaticObjectField(cls.get(), field)).get()));
    }

    auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
	HANDLE_JAVA_CLASS_NOT_FOUND(nullptr);
    jfieldID field = jvm->GetFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(nullptr);
    return make_safe<T>(jvm->NewGlobalRef(make_safe_local<T>(jvm->GetObjectField(object, field)).get()));
}

template<typename T>
typename std::enable_if<std::is_same<jint, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
		HANDLE_JAVA_CLASS_NOT_FOUND(-1);
        jfieldID field = jvm->GetFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(-1);
		return jvm->GetIntField(object, field);
    }

    auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
	HANDLE_JAVA_CLASS_NOT_FOUND(-1);
    jfieldID field = jvm->GetStaticFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(-1);
	return jvm->GetStaticIntField(cls.get(), field);
}

template<typename T>
typename std::enable_if<std::is_same<jlong, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
		HANDLE_JAVA_CLASS_NOT_FOUND(-1);
        jfieldID field = jvm->GetFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(-1);
		return jvm->GetLongField(object, field);
    }

    auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
	HANDLE_JAVA_CLASS_NOT_FOUND(-1);
    jfieldID field = jvm->GetStaticFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(-1);
	return jvm->GetStaticLongField(cls.get(), field);
}

template<typename T>
typename std::enable_if<std::is_same<jboolean, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
		HANDLE_JAVA_CLASS_NOT_FOUND(false);
        jfieldID field = jvm->GetFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(false);
        return jvm->GetBooleanField(object, field);
    }

    auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
	HANDLE_JAVA_CLASS_NOT_FOUND(false);
    jfieldID field = jvm->GetStaticFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(false);
    return jvm->GetStaticBooleanField(cls.get(), field);
}

template<typename T>
typename std::enable_if<std::is_same<jbyte, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
		HANDLE_JAVA_CLASS_NOT_FOUND(0);
        jfieldID field = jvm->GetFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(0);
        return jvm->GetByteField(object, field);
    }

    auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
	HANDLE_JAVA_CLASS_NOT_FOUND(0);
    jfieldID field = jvm->GetStaticFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(0);
    return jvm->GetStaticByteField(cls.get(), field);
}

template<typename T>
typename std::enable_if<std::is_same<jchar, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
		HANDLE_JAVA_CLASS_NOT_FOUND('\0');
        jfieldID field = jvm->GetFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION('\0');
        return jvm->GetCharField(object, field);
    }

    auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
	HANDLE_JAVA_CLASS_NOT_FOUND('\0');
    jfieldID field = jvm->GetStaticFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION('\0');
    return jvm->GetStaticCharField(cls.get(), field);
}

template<typename T>
typename std::enable_if<std::is_same<jshort, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
		HANDLE_JAVA_CLASS_NOT_FOUND(-1);
        jfieldID field = jvm->GetFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(-1);
        return jvm->GetShortField(object, field);
    }

    auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
	HANDLE_JAVA_CLASS_NOT_FOUND(-1);
    jfieldID field = jvm->GetStaticFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(-1);
    return jvm->GetStaticShortField(cls.get(), field);
}

template<typename T>
typename std::enable_if<std::is_same<jfloat, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
		HANDLE_JAVA_CLASS_NOT_FOUND(-1.0);
        jfieldID field = jvm->GetFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(-1.0);
        return jvm->GetFloatField(object, field);
    }

    auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
	HANDLE_JAVA_CLASS_NOT_FOUND(-1.0);
    jfieldID field = jvm->GetStaticFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(-1.0);
    return jvm->GetStaticFloatField(cls.get(), field);
}

template<typename T>
typename std::enable_if<std::is_same<jdouble, typename std::remove_cv<T>::type>::value, T>::type
Reflection::getPrimitive(jobject object, ReflectionHook hook)
{
    if (object)
    {
        auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
		HANDLE_JAVA_CLASS_NOT_FOUND(-1.0);
        jfieldID field = jvm->GetFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
		HANDLE_JAVA_EXCEPTION(-1.0);
        return jvm->GetDoubleField(object, field);
    }

    auto cls = make_safe_local<jclass>(this->LoadClass(hook.cls.c_str()));
	HANDLE_JAVA_CLASS_NOT_FOUND(-1.0);
    jfieldID field = jvm->GetStaticFieldID(cls.get(), hook.field.c_str(), hook.desc.c_str());
	HANDLE_JAVA_EXCEPTION(-1.0);
    return jvm->GetStaticDoubleField(cls.get(), field);
}

#endif // REFLECTION_HXX_INCLUDED
