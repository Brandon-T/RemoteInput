#include "Reflection.hxx"

Reflection::Reflection() noexcept : jvm(new JVM()), frame(nullptr), applet(nullptr), classLoader(nullptr)
{
}

Reflection::Reflection(JNIEnv* env) noexcept : jvm(new JVM(env)), frame(nullptr), applet(nullptr), classLoader(nullptr)
{
}

Reflection::Reflection(Reflection&& other) noexcept : jvm(other.jvm), frame(other.frame), applet(other.applet), classLoader(other.classLoader)
{
    other.jvm = nullptr;
    other.frame = nullptr;
    other.applet = nullptr;
    other.classLoader = nullptr;
}

Reflection::~Reflection() noexcept
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

Reflection& Reflection::operator = (Reflection&& other) noexcept
{
    std::swap(jvm, other.jvm);
    std::swap(frame, other.frame);
    std::swap(applet, other.applet);
    std::swap(classLoader, other.classLoader);
    return *this;
}

bool Reflection::Initialize(jobject awtFrame) noexcept
{
    using Result = std::unique_ptr<typename std::remove_pointer<jobject>::type, std::function<void(jobject)>>;
    std::function<Result(jobject)> findApplet = [&](jobject component) -> Result {
        if (component && this->IsDecendentOf(component, "java/awt/Container"))
        {
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
                    auto applet_class = make_safe_local<jclass>(jvm->FindClass("java/applet/Applet"));
                    if (jvm->IsInstanceOf(component.get(), applet_class.get()))
                    {
                        return component;
                    }

                    auto result = findApplet(component.get());
                    if (result)
                    {
                        return result;
                    }
                }
            }
        }

        return {};
    };

    std::function<Result(jobject)> findCanvas = [&](jobject component) -> Result {
        if (component && this->IsDecendentOf(component, "java/awt/Container"))
        {
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
                    auto canvas_class = make_safe_local<jclass>(jvm->FindClass("java/awt/Canvas"));
                    if (jvm->IsInstanceOf(component.get(), canvas_class.get()))
                    {
                        return component;
                    }

                    auto result = findCanvas(component.get());
                    if (result)
                    {
                        return result;
                    }
                }
            }
        }

        return {};
    };

    std::function<Result(jobject)> getParent = [&](jobject component) -> Result {
        if (component)
        {
            auto cls = make_safe_local<jclass>(jvm->GetObjectClass(component));
            if (cls)
            {
                jmethodID mid = jvm->GetMethodID(cls.get(), "getParent", "()Ljava/awt/Container;");
                if (mid)
                {
                    return make_safe_local<jobject>(jvm->CallObjectMethod(component, mid));
                }
            }
        }
        return {};
    };

    //Find Client Class.
    this->applet = findApplet(awtFrame).release() ?: getParent(findCanvas(awtFrame).get()).release();
    if (this->applet)
    {
        this->frame = jvm->NewGlobalRef(awtFrame);

        jvm->DeleteLocalRef(std::exchange(this->applet, jvm->NewGlobalRef(this->applet)));
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

bool Reflection::Attach() const noexcept
{
    return this->jvm && this->jvm->AttachCurrentThread() == JNI_OK;
}

bool Reflection::AttachAsDaemon() const noexcept
{
	//JVM can shut down without waiting for our thread to detach..
	return this->jvm && this->jvm->AttachCurrentThreadAsDaemon() == JNI_OK;
}

bool Reflection::Detach() const noexcept
{
    return this->jvm && this->jvm->DetachCurrentThread() == JNI_OK;
}

void Reflection::PrintClasses() const noexcept
{
    if (this->classLoader)
    {
        auto clsLoaderClass = make_safe_local<jclass>(jvm->GetObjectClass(classLoader));
        jfieldID field = jvm->GetFieldID(clsLoaderClass.get(), "classes", "Ljava/util/Vector;");
        auto classes = make_safe_local<jobject>(jvm->GetObjectField(classLoader, field));
        jmethodID toArray = jvm->GetMethodID(make_safe_local<jclass>(jvm->GetObjectClass(classes.get())).get(), "toArray", "()[Ljava/lang/Object;");
        auto clses = make_safe_local<jobjectArray>(jvm->CallObjectMethod(classes.get(), toArray));

        printf("LOADED CLASSES:\n");
        for (int i = 0; i < jvm->GetArrayLength(clses.get()); ++i)
		{
            auto clsObj = make_safe_local<jobject>(jvm->GetObjectArrayElement(clses.get(), i));
            std::string name = this->GetClassName(clsObj.get());
            printf("%s\n", name.c_str());
        }

        printf("FINISHED\n");
        fflush(stdout);
    }
}

std::string Reflection::GetClassName(jobject object) const noexcept
{
    std::function<std::string(jobject)> getClassName = [&](jobject object) {
		auto cls = make_safe_local<jclass>(jvm->GetObjectClass(object));
		jmethodID mid = jvm->GetMethodID(cls.get(), "getName", "()Ljava/lang/String;");
		auto strObj = make_safe_local<jstring>(jvm->CallObjectMethod(object, mid));

		if (strObj)
		{
            const char *str = jvm->GetStringUTFChars(strObj.get(), nullptr);
            std::string class_name = str;
            jvm->ReleaseStringUTFChars(strObj.get(), str);
            return class_name;
        }
		return std::string();
	};
	return getClassName(object);
}

std::string Reflection::GetClassType(jobject object) const noexcept
{
    std::function<std::string(jobject)> getClassType = [&](jobject component) {
		auto cls = make_safe_local<jclass>(jvm->GetObjectClass(component));
		jmethodID mid = jvm->GetMethodID(cls.get(), "getClass", "()Ljava/lang/Class;");
		auto clsObj = make_safe_local<jobject>(jvm->CallObjectMethod(component, mid));

		cls = make_safe_local<jclass>(jvm->GetObjectClass(clsObj.get()));
		mid = jvm->GetMethodID(cls.get(), "getName", "()Ljava/lang/String;");
		auto strObj = make_safe_local<jstring>(jvm->CallObjectMethod(clsObj.get(), mid));

		if (strObj)
		{
            const char *str = jvm->GetStringUTFChars(strObj.get(), nullptr);
            std::string class_name = str;
            jvm->ReleaseStringUTFChars(strObj.get(), str);
            return class_name;
        }
		return std::string();
	};

	return getClassType(object);
}

bool Reflection::IsDecendentOf(jobject object, const char* className) const noexcept
{
    auto cls = make_safe_local<jclass>(jvm->FindClass(className));
    return jvm->IsInstanceOf(object, cls.get());
}

jclass Reflection::LoadClass(const char* clsToLoad) const noexcept
{
    auto cls = make_safe_local<jclass>(jvm->GetObjectClass(classLoader));
    jmethodID loadClass = jvm->GetMethodID(cls.get(), "loadClass", "(Ljava/lang/String;Z)Ljava/lang/Class;");
	auto className = make_safe_local<jstring>(jvm->NewStringUTF(clsToLoad));
    return static_cast<jclass>(jvm->CallObjectMethod(classLoader, loadClass, className.get(), true));
}

jobject Reflection::getApplet() const noexcept
{
	return this->applet;
}

JVM* Reflection::getVM() const noexcept
{
	return this->jvm;
}

JNIEnv* Reflection::getEnv() const noexcept
{
    return this->jvm->getENV();
}
