//
//  AWTEventAccessor.cxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#include "AWTEventAccessor.hxx"
#include <utility>

AWTEventAccessor::AWTEventAccessor(JNIEnv* env, jclass cls, jobject accessor) : env(env), cls(cls), accessor(accessor)
{
	env->DeleteLocalRef(std::exchange(this->cls, static_cast<jclass>(env->NewGlobalRef(this->cls))));
	env->DeleteLocalRef(std::exchange(this->accessor, env->NewGlobalRef(this->accessor)));
}

AWTEventAccessor::~AWTEventAccessor()
{
	if (cls)
	{
		env->DeleteGlobalRef(cls);
	}
	
	if (accessor)
	{
		env->DeleteGlobalRef(accessor);
	}
}

AWTEventAccessor::AWTEventAccessor(AWTEventAccessor&& other) : env(other.env), cls(other.cls), accessor(other.accessor)
{
	other.env = nullptr;
	other.cls = nullptr;
	other.accessor = nullptr;
}

AWTEventAccessor& AWTEventAccessor::operator = (AWTEventAccessor&& other)
{
	this->env = other.env;
	this->cls = other.cls;
	this->accessor = other.accessor;
	other.env = nullptr;
	other.cls = nullptr;
	other.accessor = nullptr;
	return *this;
}

void AWTEventAccessor::setSystemGenerated(AWTEvent* event)
{
	static jmethodID methodId = env->GetMethodID(cls, "setSystemGenerated", "(Ljava/awt/AWTEvent;)V");
	env->CallVoidMethod(accessor, methodId, event->get());
}

void AWTEventAccessor::setPosted(AWTEvent* event)
{
	static jmethodID methodId = env->GetMethodID(cls, "setPosted", "(Ljava/awt/AWTEvent;)V");
	env->CallVoidMethod(accessor, methodId, event->get());
}
