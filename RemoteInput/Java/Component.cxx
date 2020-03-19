//
//  Component.cxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#include "Component.hxx"
#include <utility>

Component::Component(JNIEnv* env, jobject component, bool canDelete) : env(env), cls(nullptr), component(component), canDelete(canDelete)
{
	if (!component)
	{
		this->cls = env->FindClass("java/awt/Component");
		env->DeleteLocalRef(std::exchange(this->cls, static_cast<jclass>(env->NewGlobalRef(this->cls))));
	}
	else
	{
		this->cls = env->GetObjectClass(component);
		env->DeleteLocalRef(std::exchange(this->cls, static_cast<jclass>(env->NewGlobalRef(this->cls))));
	}
}

Component::~Component()
{
	if (cls)
	{
		env->DeleteGlobalRef(cls);
	}
	
	if (canDelete && component)
	{
		env->DeleteGlobalRef(component);
	}
}

Component::Component(JNIEnv* env, jclass cls, jobject component, bool canDelete) : env(env), cls(cls), component(component), canDelete(canDelete)
{
}

Component::Component(Component&& other) : env(other.env), cls(other.cls), component(other.component), canDelete(other.canDelete)
{
	other.env = nullptr;
	other.cls = nullptr;
	other.component = nullptr;
	other.canDelete = false;
}

Component& Component::operator = (Component&& other)
{
	this->env = other.env;
	this->cls = other.cls;
	this->component = other.component;
	this->canDelete = other.canDelete;
	other.env = nullptr;
	other.cls = nullptr;
	other.component = nullptr;
	other.canDelete = false;
	return *this;
}

JNIEnv* Component::getEnv()
{
	return env;
}


jobject Component::get()
{
	return component;
}

void Component::getLocationOnScreen(std::int32_t &x, std::int32_t &y)
{
	jclass pointClass = env->FindClass("java/awt/Point");
	static jmethodID methodId = env->GetMethodID(cls, "getLocationOnScreen", "()Ljava/awt/Point;");
	static jfieldID xFieldId = env->GetFieldID(pointClass, "x", "I");
	static jfieldID yFieldId = env->GetFieldID(pointClass, "y", "I");
	
	jobject object = env->CallObjectMethod(component, methodId);
	if (object)
	{
		x = env->GetIntField(object, xFieldId);
		y = env->GetIntField(object, yFieldId);
		
		env->DeleteLocalRef(object);
		env->DeleteLocalRef(pointClass);
	}
	else
	{
		x = -1;
		y = -1;
	}
}

void Component::getMousePosition(std::int32_t &x, std::int32_t &y)
{
	jclass pointClass = env->FindClass("java/awt/Point");
	static jmethodID methodId = env->GetMethodID(cls, "getMousePosition", "()Ljava/awt/Point;");
	static jfieldID xFieldId = env->GetFieldID(pointClass, "x", "I");
	static jfieldID yFieldId = env->GetFieldID(pointClass, "y", "I");
	
	jobject object = env->CallObjectMethod(component, methodId);
	if (object)
	{
		x = env->GetIntField(object, xFieldId);
		y = env->GetIntField(object, yFieldId);
		
		env->DeleteLocalRef(object);
		env->DeleteLocalRef(pointClass);
	}
	else
	{
		x = -1;
		y = -1;
	}
}

void Component::getLocation(std::int32_t &x, std::int32_t &y)
{
	jclass pointClass = env->FindClass("java/awt/Point");
	static jmethodID methodId = env->GetMethodID(cls, "getLocation", "()Ljava/awt/Point;");
	static jfieldID xFieldId = env->GetFieldID(pointClass, "x", "I");
	static jfieldID yFieldId = env->GetFieldID(pointClass, "y", "I");
	
	jobject object = env->CallObjectMethod(component, methodId);
	if (object)
	{
		x = env->GetIntField(object, xFieldId);
		y = env->GetIntField(object, yFieldId);
		
		env->DeleteLocalRef(object);
		env->DeleteLocalRef(pointClass);
	}
	else
	{
		x = -1;
		y = -1;
	}
}

void Component::getSize(std::size_t &width, std::size_t &height)
{
	static jmethodID widthId = env->GetMethodID(cls, "getWidth", "()I");
	static jmethodID heightId = env->GetMethodID(cls, "getHeight", "()I");
	width = env->CallIntMethod(component, widthId);
	height = env->CallIntMethod(component, heightId);
}

Component Component::getComponentAt(std::int32_t x, std::int32_t y)
{
	static jmethodID methodId = env->GetMethodID(cls, "getComponentAt", "(II)Ljava/awt/Component;");
	jobject object = env->CallObjectMethod(component, methodId, x, y);
	if (object)
	{
		env->DeleteLocalRef(std::exchange(object, env->NewGlobalRef(object)));
		return {env, object};
	}
	return {env, nullptr};
}

void Component::dispatchEvent(AWTEvent* event)
{
	Component::dispatchEvent(event->get());
}

void Component::dispatchEvent(jobject event)
{
	static jmethodID methodId = env->GetMethodID(cls, "dispatchEvent", "(Ljava/awt/AWTEvent;)V");
	env->CallVoidMethod(component, methodId, event);
}

bool Component::isVisible()
{
	static jmethodID methodId = env->GetMethodID(cls, "isVisible", "()Z");
	return env->CallBooleanMethod(component, methodId);
}

bool Component::isValid()
{
	static jmethodID methodId = env->GetMethodID(cls, "isValid", "()Z");
	return env->CallBooleanMethod(component, methodId);
}

bool Component::isEnabled()
{
	static jmethodID methodId = env->GetMethodID(cls, "isEnabled", "()Z");
	return env->CallBooleanMethod(component, methodId);
}

void Component::setEnabled(bool enabled)
{
	static jmethodID methodId = env->GetMethodID(cls, "setEnabled", "(Z)V");
	env->CallVoidMethod(component, methodId, enabled);
}

bool Component::hasFocus()
{
	static jmethodID methodId = env->GetMethodID(cls, "hasFocus", "()Z");
	return env->CallBooleanMethod(component, methodId);
}

bool Component::requestFocusInWindow()
{
	static jmethodID methodId = env->GetMethodID(cls, "requestFocusInWindow", "()Z");
	return env->CallBooleanMethod(component, methodId);
}

void Component::requestFocus()
{
	static jmethodID methodId = env->GetMethodID(cls, "requestFocus", "()V");
	env->CallVoidMethod(component, methodId);
}
