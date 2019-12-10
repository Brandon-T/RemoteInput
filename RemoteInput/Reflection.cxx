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
	if (this->jvm->getENV() || this->Attach())
	{
		jvm->DeleteGlobalRef(this->applet);
		jvm->DeleteGlobalRef(this->classLoader);
		jvm->DeleteGlobalRef(this->frame);
		this->Detach();
	}
	#endif
    delete jvm;
    jvm = nullptr;
}

bool Reflection::Initialize(jobject awtFrame)
{
    this->frame = jvm->NewGlobalRef(awtFrame);
    std::function<jobject(jobject)> findClient = [&](jobject component) {
		jobject result = nullptr;
        jclass cls = jvm->GetObjectClass(component);
        jmethodID mid = jvm->GetMethodID(cls, "getComponents", "()[Ljava/awt/Component;");

        //TODO: Should validate that it's actually a java.awt.Frame so I can call `getComponents`

        if (mid)
		{
            jobjectArray components = static_cast<jobjectArray>(jvm->CallObjectMethod(component, mid));
            jint len = jvm->GetArrayLength(components);

            for (jint i = 0; i < len; ++i)
			{
                jobject component = jvm->GetObjectArrayElement(components, i); //Some java.awt.Panel.
				if (jvm->IsInstanceOf(component, jvm->FindClass("java/applet/Applet")))
				{
					return component;
				}

                /*if (getClassName(component) == "client")
				{
                    return jvm->NewGlobalRef(component);
                }*/

				jvm->DeleteLocalRef(cls);
                result = findClient(component);
				if (result)
				{
					return result;
				}
            }
        }

        return result;
    };

    //Find Client Class.
    this->applet = jvm->NewGlobalRef(findClient(awtFrame));

    if (this->applet)
    {
        jclass cls = jvm->GetObjectClass(this->applet);
        jmethodID mid = jvm->GetMethodID(cls, "getClass", "()Ljava/lang/Class;");
        jobject clsObj = jvm->CallObjectMethod(this->applet, mid);
        cls = jvm->GetObjectClass(clsObj);

        //Get Client's ClassLoader.
        mid = jvm->GetMethodID(cls, "getClassLoader", "()Ljava/lang/ClassLoader;");
        this->classLoader = jvm->NewGlobalRef(jvm->CallObjectMethod(clsObj, mid));
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
        jclass clsLoaderClass = jvm->GetObjectClass(classLoader);
        jfieldID field = jvm->GetFieldID(clsLoaderClass, "classes", "Ljava/util/Vector;");
        jobject classes = jvm->GetObjectField(classLoader, field);
        jmethodID toArray = jvm->GetMethodID(jvm->GetObjectClass(classes), "toArray", "()[Ljava/lang/Object;");
        jobjectArray clses = static_cast<jobjectArray>(jvm->CallObjectMethod(classes, toArray));

        printf("LOADED CLASSES:\n");
        for (int i = 0; i < jvm->GetArrayLength(clses); ++i) {
            jobject clsObj = jvm->GetObjectArrayElement(clses, i);
            std::string name = this->GetClassName(clsObj);
            printf("%s\n", name.c_str());
        }

        printf("FINISHED\n");
        fflush(stdout);
    }
}

std::string Reflection::GetClassName(jobject object)
{
    std::function<std::string(jobject)> getClassName = [&](jobject object) {
		jclass cls = jvm->GetObjectClass(object);
		jmethodID mid = jvm->GetMethodID(cls, "getName", "()Ljava/lang/String;");
		jstring strObj = static_cast<jstring>(jvm->CallObjectMethod(object, mid));

		const char* str = jvm->GetStringUTFChars(strObj, nullptr);
		std::string class_name = str;
		jvm->ReleaseStringUTFChars(strObj, str);
		return class_name;
	};
	return getClassName(object);
}

std::string Reflection::GetClassType(jobject object)
{
    std::function<std::string(jobject)> getClassType = [&](jobject component) {
		jclass cls = jvm->GetObjectClass(component);
		jmethodID mid = jvm->GetMethodID(cls, "getClass", "()Ljava/lang/Class;");
		jobject clsObj = jvm->CallObjectMethod(component, mid);

		cls = jvm->GetObjectClass(clsObj);
		mid = jvm->GetMethodID(cls, "getName", "()Ljava/lang/String;");
		jstring strObj = static_cast<jstring>(jvm->CallObjectMethod(clsObj, mid));

		const char* str = jvm->GetStringUTFChars(strObj, nullptr);
		std::string class_name = str;
		jvm->ReleaseStringUTFChars(strObj, str);
		return class_name;
	};

	return getClassType(object);
}

jclass Reflection::LoadClass(const char* clsToLoad)
{
    jclass cls = jvm->GetObjectClass(classLoader);
    jmethodID loadClass = jvm->GetMethodID(cls, "loadClass", "(Ljava/lang/String;Z)Ljava/lang/Class;");
    return static_cast<jclass>(jvm->CallObjectMethod(classLoader, loadClass, jvm->NewStringUTF(clsToLoad), true));
}

JVM* Reflection::getVM()
{
	return this->jvm;
}

JNIEnv* Reflection::getEnv()
{
    return this->jvm->getENV();
}
