#include "Reflection.hxx"

Reflection::Reflection()
{
    this->jvm = new JVM();
}

Reflection::Reflection(JNIEnv* env)
{
    this->jvm = new JVM(env);
}

Reflection::~Reflection()
{
    jvm->DeleteGlobalRef(this->client);
    jvm->DeleteGlobalRef(this->classLoader);
    jvm->DeleteGlobalRef(this->frame);
    delete jvm;
    jvm = nullptr;
}

bool Reflection::Initialize(jobject awtFrame)
{
    this->frame = jvm->NewGlobalRef(awtFrame);

	std::function<std::string(jobject)> getClassName = [&](jobject component) {
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

    std::function<jobject(jobject)> findClient = [&](jobject component) {
        jobject result = nullptr;
        jclass cls = jvm->GetObjectClass(component);
        jmethodID mid = jvm->GetMethodID(cls, "getComponents", "()[Ljava/awt/Component;");

        //TODO: Should validate that it's actually a java.awt.Frame so I can call `getComponents`

        if (mid) {
            jobjectArray components = (jobjectArray)jvm->CallObjectMethod(component, mid);

            int len = jvm->GetArrayLength(components);

            for (int i = 0; i < len; ++i) {
                jobject component = jvm->GetObjectArrayElement(components, i); //Some java.awt.Panel.

				if (jvm->IsInstanceOf(component, jvm->FindClass("java/applet/Applet")))
				{
					return jvm->NewGlobalRef(component);
				}

                /*if (getClassName(component) == "client")
				{
                    return jvm->NewGlobalRef(component);
                }*/

                result = findClient(component);
            }
        }

        return result;
    };

    //Find Client Class.
    this->client = findClient(awtFrame);

    if (this->client)
    {
        jclass cls = jvm->GetObjectClass(this->client);
        jmethodID mid = jvm->GetMethodID(cls, "getClass", "()Ljava/lang/Class;");
        jobject clsObj = jvm->CallObjectMethod(this->client, mid);
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

            jmethodID mid = jvm->GetMethodID(jvm->GetObjectClass(clsObj), "getName", "()Ljava/lang/String;");

            // Call the getName() to get a jstring object back
            jstring strObj = (jstring)jvm->CallObjectMethod(clsObj, mid);

            // Now get the c string from the java jstring object
            const char* str = jvm->GetStringUTFChars(strObj, NULL);

            printf("%s\n", str);

            // Release the memory pinned char array
            jvm->ReleaseStringUTFChars(strObj, str);
        }

        printf("FINISHED\n");
        fflush(stdout);
    }
}

jclass Reflection::LoadClass(const char* clsToLoad)
{
    jclass cls = jvm->GetObjectClass(classLoader);
    jmethodID loadClass = jvm->GetMethodID(cls, "loadClass", "(Ljava/lang/String;Z)Ljava/lang/Class;");
    return static_cast<jclass>(jvm->NewGlobalRef(jvm->CallObjectMethod(classLoader, loadClass, jvm->NewStringUTF(clsToLoad), true)));
}

JNIEnv* Reflection::getEnv()
{
    return this->jvm->getENV();
}
