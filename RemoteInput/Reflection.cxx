#include "Reflection.hxx"

Reflection::Reflection() : jvm(new JVM()), frame(nullptr), applet(nullptr), classLoader(nullptr)
{
}

Reflection::Reflection(JNIEnv* env) : jvm(new JVM(env)), frame(nullptr), applet(nullptr), classLoader(nullptr)
{
}

Reflection::~Reflection()
{
	#if defined(__APPLE__)
	printf("EXITING VM -- VM ABORTS ON CLEANUP\n");
	#else
	printf("EXITING VM -- VM ABORTS ON CLEANUP\n");
	/*if (this->jvm->getENV() || this->Attach())
	{
		jvm->DeleteGlobalRef(this->applet);
		jvm->DeleteGlobalRef(this->classLoader);
		jvm->DeleteGlobalRef(this->frame);
		this->Detach();
	}*/
	#endif
    delete jvm;
    jvm = nullptr;
}

bool Reflection::Initialize(jobject awtFrame)
{
    std::function<jobject(jobject)> findClient = [&](jobject component) {
		jobject result = nullptr;
        auto cls = make_safe_local<jclass>(jvm->GetObjectClass(component));
        jmethodID mid = jvm->GetMethodID(cls.get(), "getComponents", "()[Ljava/awt/Component;");

        if (mid)
		{
            auto components = make_safe_local<jobjectArray>(jvm->CallObjectMethod(component, mid));
            jint len = jvm->GetArrayLength(components.get());

            for (jint i = 0; i < len; ++i)
			{
				//Some java.awt.Panel.
                auto component = make_safe_local<jobject>(jvm->GetObjectArrayElement(components.get(), i));
				if (jvm->IsInstanceOf(component.get(), jvm->FindClass("java/applet/Applet")))
				{
					return jvm->NewGlobalRef(component.get());
				}

                result = findClient(component.get());
				if (result)
				{
					return result;
				}
            }
        }

        return result;
    };

    //Find Client Class.
	this->frame = jvm->NewGlobalRef(awtFrame);
    this->applet = findClient(awtFrame);

    if (this->applet)
    {
        auto cls = make_safe_local<jclass>(jvm->GetObjectClass(this->applet));
        jmethodID mid = jvm->GetMethodID(cls.get(), "getClass", "()Ljava/lang/Class;");
        auto clsObj = make_safe_local<jobject>(jvm->CallObjectMethod(this->applet, mid));
        cls = make_safe_local<jclass>(jvm->GetObjectClass(clsObj.get()));

        //Get Client's ClassLoader.
        mid = jvm->GetMethodID(cls.get(), "getClassLoader", "()Ljava/lang/ClassLoader;");
        this->classLoader = jvm->NewGlobalRef(make_safe_local<jobject>(jvm->CallObjectMethod(clsObj.get(), mid)).get());
        return true;
    }

    return false;
}

bool Reflection::Attach()
{
    return this->jvm->AttachCurrentThread() == JNI_OK;
}

bool Reflection::AttachAsDaemon()
{
	//JVM can shut down without waiting for our thread to detach..
	return this->jvm->AttachCurrentThreadAsDaemon() == JNI_OK;
}

bool Reflection::Detach()
{
    return this->jvm->DetachCurrentThread() == JNI_OK;
}

void Reflection::PrintClasses()
{
    if (this->classLoader)
    {
        auto clsLoaderClass = make_safe_local<jclass>(jvm->GetObjectClass(classLoader));
        jfieldID field = jvm->GetFieldID(clsLoaderClass.get(), "classes", "Ljava/util/Vector;");
        auto classes = make_safe_local<jobject>(jvm->GetObjectField(classLoader, field));
        jmethodID toArray = jvm->GetMethodID(make_safe_local<jclass>(jvm->GetObjectClass(classes.get())).get(), "toArray", "()[Ljava/lang/Object;");
        auto clses = make_safe_local<jobjectArray>(jvm->CallObjectMethod(classes.get(), toArray));

        printf("LOADED CLASSES:\n");
        for (int i = 0; i < jvm->GetArrayLength(clses.get()); ++i) {
            auto clsObj = make_safe_local<jobject>(jvm->GetObjectArrayElement(clses.get(), i));
            std::string name = this->GetClassName(clsObj.get());
            printf("%s\n", name.c_str());
        }

        printf("FINISHED\n");
        fflush(stdout);
    }
}

std::string Reflection::GetClassName(jobject object)
{
    std::function<std::string(jobject)> getClassName = [&](jobject object) {
		auto cls = make_safe_local<jclass>(jvm->GetObjectClass(object));
		jmethodID mid = jvm->GetMethodID(cls.get(), "getName", "()Ljava/lang/String;");
		auto strObj = make_safe_local<jstring>(jvm->CallObjectMethod(object, mid));

		const char* str = jvm->GetStringUTFChars(strObj.get(), nullptr);
		std::string class_name = str;
		jvm->ReleaseStringUTFChars(strObj.get(), str);
		return class_name;
	};
	return getClassName(object);
}

std::string Reflection::GetClassType(jobject object)
{
    std::function<std::string(jobject)> getClassType = [&](jobject component) {
		auto cls = make_safe_local<jclass>(jvm->GetObjectClass(component));
		jmethodID mid = jvm->GetMethodID(cls.get(), "getClass", "()Ljava/lang/Class;");
		auto clsObj = make_safe_local<jobject>(jvm->CallObjectMethod(component, mid));

		cls = make_safe_local<jclass>(jvm->GetObjectClass(clsObj.get()));
		mid = jvm->GetMethodID(cls.get(), "getName", "()Ljava/lang/String;");
		auto strObj = make_safe_local<jstring>(jvm->CallObjectMethod(clsObj.get(), mid));

		const char* str = jvm->GetStringUTFChars(strObj.get(), nullptr);
		std::string class_name = str;
		jvm->ReleaseStringUTFChars(strObj.get(), str);
		return class_name;
	};

	return getClassType(object);
}

jclass Reflection::LoadClass(const char* clsToLoad)
{
    auto cls = make_safe_local<jclass>(jvm->GetObjectClass(classLoader));
    jmethodID loadClass = jvm->GetMethodID(cls.get(), "loadClass", "(Ljava/lang/String;Z)Ljava/lang/Class;");
	auto className = make_safe_local<jstring>(jvm->NewStringUTF(clsToLoad));
    return static_cast<jclass>(jvm->CallObjectMethod(classLoader, loadClass, className.get(), true));
}

JVM* Reflection::getVM()
{
	return this->jvm;
}

JNIEnv* Reflection::getEnv()
{
    return this->jvm->getENV();
}
