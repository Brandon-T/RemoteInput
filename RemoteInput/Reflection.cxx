#include "Reflection.hxx"
#include <utility>

Reflection::Reflection() noexcept : env(nullptr), frame(nullptr), applet(nullptr), classLoader(nullptr), cache(env, classLoader)
{
}

Reflection::Reflection(Reflection&& other) noexcept : env(other.env), frame(other.frame), applet(other.applet), classLoader(other.classLoader), cache(std::move(other.cache))
{
    other.env = nullptr;
    other.frame = nullptr;
    other.applet = nullptr;
    other.classLoader = nullptr;
    other.cache = {nullptr, nullptr};
}

Reflection::~Reflection() noexcept
{
    if (env)
    {
//        env->DeleteGlobalRef(classLoader);
//        env->DeleteGlobalRef(applet);
//        env->DeleteGlobalRef(frame);
    }

    ClearCache();
}

Reflection& Reflection::operator = (Reflection&& other) noexcept
{
    std::swap(env, other.env);
    std::swap(frame, other.frame);
    std::swap(applet, other.applet);
    std::swap(classLoader, other.classLoader);
    std::swap(cache, other.cache);
    return *this;
}

std::unique_ptr<Reflection> Reflection::Create(jobject awtFrame) noexcept
{
    using Result = std::unique_ptr<typename std::remove_pointer<jobject>::type, std::function<void(jobject)>>;
    auto IsDecendentOf = [](JNIEnv* env, jobject object, const char* className) -> bool {
        auto cls = make_safe_local<jclass>(env, env->FindClass(className));
        return env->IsInstanceOf(object, cls.get());
    };

    std::function<Result(JNIEnv*, jobject)> findApplet = [&](JNIEnv* env, jobject component) -> Result {
        if (component && IsDecendentOf(env, component, "java/awt/Container"))
        {
            auto cls = make_safe_local<jclass>(env, env->GetObjectClass(component));
            jmethodID mid = env->GetMethodID(cls.get(), "getComponents", "()[Ljava/awt/Component;");

            if (mid)
            {
                auto components = make_safe_local<jobjectArray>(env, env->CallObjectMethod(component, mid));
                jint len = env->GetArrayLength(components.get());

                for (jint i = 0; i < len; ++i)
                {
                    //Some java.awt.Panel.
                    auto component = make_safe_local<jobject>(env, env->GetObjectArrayElement(components.get(), i));
                    auto applet_class = make_safe_local<jclass>(env, env->FindClass("java/applet/Applet"));
                    if (env->IsInstanceOf(component.get(), applet_class.get()))
                    {
                        return component;
                    }

                    auto result = findApplet(env, component.get());
                    if (result)
                    {
                        return result;
                    }
                }
            }
        }

        return {};
    };

    std::function<Result(JNIEnv* env, jobject)> findCanvas = [&](JNIEnv* env, jobject component) -> Result {
        if (component && IsDecendentOf(env, component, "java/awt/Container"))
        {
            auto cls = make_safe_local<jclass>(env, env->GetObjectClass(component));
            jmethodID mid = env->GetMethodID(cls.get(), "getComponents", "()[Ljava/awt/Component;");

            if (mid)
            {
                auto components = make_safe_local<jobjectArray>(env, env->CallObjectMethod(component, mid));
                jint len = env->GetArrayLength(components.get());

                for (jint i = 0; i < len; ++i)
                {
                    //Some java.awt.Panel.
                    auto component = make_safe_local<jobject>(env, env->GetObjectArrayElement(components.get(), i));
                    auto canvas_class = make_safe_local<jclass>(env, env->FindClass("java/awt/Canvas"));
                    if (env->IsInstanceOf(component.get(), canvas_class.get()))
                    {
                        return component;
                    }

                    auto result = findCanvas(env, component.get());
                    if (result)
                    {
                        return result;
                    }
                }
            }
        }

        return {};
    };

    auto getParent = [&](JNIEnv* env, jobject component) -> Result {
        if (component)
        {
            auto cls = make_safe_local<jclass>(env, env->GetObjectClass(component));
            if (cls)
            {
                jmethodID mid = env->GetMethodID(cls.get(), "getParent", "()Ljava/awt/Container;");
                if (mid)
                {
                    return make_safe_local<jobject>(env, env->CallObjectMethod(component, mid));
                }
            }
        }
        return {};
    };

    //Attach Current Thread.
    auto reflection = std::unique_ptr<Reflection>(new Reflection());
    if (!reflection->Attach())
    {
        return nullptr;
    }

    JNIEnv* env = reflection->getEnv();
    if (!IsDecendentOf(env, awtFrame, "java/awt/Frame"))
    {
        return nullptr;
    }

    //Find Canvas Class.
    auto applet = findApplet(env, awtFrame);
    if (!applet)
    {
        applet = getParent(env, findCanvas(env, awtFrame).get());
    }

    if (applet)
    {
        reflection->frame = env->NewGlobalRef(awtFrame);
        env->DeleteLocalRef(std::exchange(reflection->applet, env->NewGlobalRef(applet.release())));

        auto cls = make_safe_local<jclass>(env, env->GetObjectClass(reflection->applet));
        jmethodID mid = env->GetMethodID(cls.get(), "getClass", "()Ljava/lang/Class;");
        auto clsObj = make_safe_local<jobject>(env, env->CallObjectMethod(reflection->applet, mid));
        cls.reset(env->GetObjectClass(clsObj.get()));

        //Get Canvas's ClassLoader.
        mid = env->GetMethodID(cls.get(), "getClassLoader", "()Ljava/lang/ClassLoader;");
        reflection->classLoader = env->NewGlobalRef(make_safe_local<jobject>(env, env->CallObjectMethod(clsObj.get(), mid)).get());
        reflection->cache = {env, reflection->classLoader};
        return reflection;
    }
    return nullptr;
}

std::unique_ptr<Reflection> Reflection::Clone() noexcept
{
    JNIEnv* new_env = nullptr;
    if (JVM(env).AttachCurrentThread(&new_env) == JNI_OK)
    {
        auto reflection = std::unique_ptr<Reflection>(new Reflection());
        reflection->env = new_env;
        reflection->frame = new_env->NewGlobalRef(frame);
        reflection->applet = new_env->NewGlobalRef(applet);
        reflection->classLoader = new_env->NewGlobalRef(classLoader);
        reflection->cache = {env, classLoader};
        return reflection;
    }
    return {};
}

bool Reflection::Attach() noexcept
{
    return JVM().AttachCurrentThread(&env) == JNI_OK;
}

bool Reflection::Detach() noexcept
{
    return JVM(env).DetachCurrentThread() == JNI_OK;
}

void Reflection::PrintClasses() const noexcept
{
    if (this->classLoader)
    {
        auto clsLoaderClass = make_safe_local<jclass>(env, env->GetObjectClass(classLoader));
        jfieldID field = env->GetFieldID(clsLoaderClass.get(), "classes", "Ljava/util/Vector;");
        auto classes = make_safe_local<jobject>(env, env->GetObjectField(classLoader, field));
        jmethodID toArray = env->GetMethodID(make_safe_local<jclass>(env, env->GetObjectClass(classes.get())).get(), "toArray", "()[Ljava/lang/Object;");
        auto clses = make_safe_local<jobjectArray>(env, env->CallObjectMethod(classes.get(), toArray));

        printf("LOADED CLASSES:\n");
        for (int i = 0; i < env->GetArrayLength(clses.get()); ++i)
        {
            auto clsObj = make_safe_local<jobject>(env, env->GetObjectArrayElement(clses.get(), i));
            std::string name = this->GetClassName(clsObj.get());
            printf("%s\n", name.c_str());
        }

        printf("FINISHED\n");
        fflush(stdout);
    }
}

std::string Reflection::GetClassName(jobject object) const noexcept
{
    auto getClassName = [&](jobject object) -> std::string {
        auto cls = make_safe_local<jclass>(env, env->GetObjectClass(object));
        jmethodID mid = env->GetMethodID(cls.get(), "getName", "()Ljava/lang/String;");
        auto strObj = make_safe_local<jstring>(env, env->CallObjectMethod(object, mid));

        if (strObj)
        {
            const char *str = env->GetStringUTFChars(strObj.get(), nullptr);
            std::string class_name = str;
            env->ReleaseStringUTFChars(strObj.get(), str);
            return class_name;
        }
        return std::string();
    };
    return getClassName(object);
}

std::string Reflection::GetClassType(jobject object) const noexcept
{
    auto getClassType = [&](jobject component) -> std::string {
        auto cls = make_safe_local<jclass>(env, env->GetObjectClass(component));
        jmethodID mid = env->GetMethodID(cls.get(), "getClass", "()Ljava/lang/Class;");
        auto clsObj = make_safe_local<jobject>(env, env->CallObjectMethod(component, mid));

        cls.reset(env->GetObjectClass(clsObj.get()));
        mid = env->GetMethodID(cls.get(), "getName", "()Ljava/lang/String;");
        auto strObj = make_safe_local<jstring>(env, env->CallObjectMethod(clsObj.get(), mid));

        if (strObj)
        {
            const char *str = env->GetStringUTFChars(strObj.get(), nullptr);
            std::string class_name = str;
            env->ReleaseStringUTFChars(strObj.get(), str);
            return class_name;
        }
        return std::string();
    };

    return getClassType(object);
}

bool Reflection::IsDecendentOf(jobject object, const char* className) const noexcept
{
    auto cls = make_safe_local<jclass>(env, env->FindClass(className));
    return env->IsInstanceOf(object, cls.get());
}

jclass Reflection::LoadClass(std::string_view name) noexcept
{
    return cache.GetClass(env, name);
}

void Reflection::ClearCache() noexcept
{
    cache.clear();
}

jfieldID Reflection::GetFieldID(jclass cls, std::string_view name, std::string_view desc, bool is_static) noexcept
{
    return cache.GetFieldID(env, cls, name, desc, is_static);
}

jobject Reflection::getApplet() const noexcept
{
    return this->applet;
}

JNIEnv* Reflection::getEnv() const noexcept
{
    return this->env;
}