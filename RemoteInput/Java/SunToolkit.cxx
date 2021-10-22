//
// Created by Brandon on 2021-10-21.
//

#include "SunToolkit.hxx"

namespace java
{
    Window SunToolkit::getContainingWindow(Component* component)
    {
        JNIEnv* env = component->getEnv();
        jclass cls = env->FindClass("sun/awt/SunToolkit");
        if (cls)
        {
            static jmethodID methodId = env->GetStaticMethodID(cls, "getContainingWindow", "(Ljava/awt/Component;)Ljava/awt/Window;");
            jobject containing_window = env->CallStaticObjectMethod(cls, methodId, component->get());
            env->DeleteLocalRef(cls);
            return Window{env, containing_window};
        }
        return Window{env, nullptr};
    }
}