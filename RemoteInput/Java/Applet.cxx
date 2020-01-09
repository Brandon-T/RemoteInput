//
//  Applet.cxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#include "Applet.hxx"
#include <utility>

Applet::Applet(JNIEnv* env, jobject applet, bool canDelete) : Component(env, nullptr, applet, canDelete)
{
	this->cls = env->FindClass("java/awt/Applet");
	env->DeleteLocalRef(std::exchange(this->cls, static_cast<jclass>(env->NewGlobalRef(this->cls))));
}

Component Applet::getComponent(std::int32_t index)
{
	jclass containerClass = env->FindClass("java/awt/Container");
	static jmethodID methodId = env->GetMethodID(containerClass, "getComponent", "(I)Ljava/awt/Component;");
	env->DeleteLocalRef(containerClass);
	
	jobject object = env->CallObjectMethod(this->component, methodId, index);
	if (object)
	{
		env->DeleteLocalRef(std::exchange(object, env->NewGlobalRef(object)));
		return {env, object};
	}
	return {env, nullptr};
}
