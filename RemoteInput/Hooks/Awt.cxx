//
//  Awt.cxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-27.
//  Copyright © 2019 XIO. All rights reserved.
//

#include "Awt.hxx"

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

JNIEnv* Component::getEnv()
{
	return env;
}


jobject Component::get()
{
	return component;
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
	static jmethodID methodId = env->GetMethodID(cls, "dispatchEvent", "(Ljava/awt/AWTEvent;)V");
	env->CallVoidMethod(component, methodId, event->get());
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


Frame::Frame(JNIEnv* env, jobject frame, bool canDelete) : Component(env, nullptr, frame, canDelete)
{
	this->cls = env->FindClass("java/awt/JFrame");
	env->DeleteLocalRef(std::exchange(this->cls, static_cast<jclass>(env->NewGlobalRef(this->cls))));
}

Component Frame::getContentPane()
{
	static jmethodID methodId = env->GetMethodID(cls, "getContentPane", "()Ljava/awt/Component;");
	jobject object = env->CallObjectMethod(component, methodId);
	if (object)
	{
		env->DeleteLocalRef(std::exchange(object, env->NewGlobalRef(object)));
		return {env, object};
	}
	return {env, nullptr};
}


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


// MARK: - Events

AWTEvent::AWTEvent() : self(nullptr)
{
}

AWTEvent::AWTEvent(jobject self) : self(self)
{
}

AWTEvent::AWTEvent(AWTEvent&& other) : self(other.self)
{
	other.self = nullptr;
}

AWTEvent& AWTEvent::operator = (AWTEvent&& other)
{
	this->self = other.self;
	other.self = nullptr;
	return *this;
}

jobject AWTEvent::get()
{
	return self;
}

FocusEvent::FocusEvent(JNIEnv* env, Component* receiver, std::int32_t id, bool temporary) : AWTEvent(), env(env), cls(nullptr)
{
	this->cls = env->FindClass("java/awt/event/FocusEvent");
	env->DeleteLocalRef(std::exchange(this->cls, static_cast<jclass>(env->NewGlobalRef(this->cls))));
	
	static jmethodID methodId = env->GetMethodID(cls, "<init>", "(Ljava/awt/Component;IZ)V");
	self = env->NewObject(cls, methodId, receiver->get(), id);
	env->DeleteLocalRef(std::exchange(self, static_cast<jclass>(env->NewGlobalRef(self))));
}

FocusEvent::~FocusEvent()
{
	env->DeleteGlobalRef(cls);
	env->DeleteGlobalRef(self);
}

void FocusEvent::Dispatch(JNIEnv* env, Component* receiver, std::int32_t id, bool temporary)
{
	jclass cls = env->FindClass("java/awt/event/FocusEvent");
	if (cls)
	{
		static jmethodID methodId = env->GetMethodID(cls, "<init>", "(Ljava/awt/Component;IZ)V");
		jobject event = env->NewObject(cls, methodId, receiver->get(), id, temporary);
		if (event)
		{
			receiver->dispatchEvent(event);
			env->DeleteLocalRef(event);
		}
		
		env->DeleteLocalRef(cls);
	}
}

KeyEvent::KeyEvent(JNIEnv* env, Component* source, std::int32_t id, std::int64_t when, std::int32_t modifiers, std::int32_t keyCode, jchar keyChar, std::int32_t keyLocation) : AWTEvent(), env(env), cls(nullptr)
{
	this->cls = env->FindClass("java/awt/event/KeyEvent");
	env->DeleteLocalRef(std::exchange(this->cls, static_cast<jclass>(env->NewGlobalRef(this->cls))));
	
	static jmethodID methodId = env->GetMethodID(cls, "<init>", "(Ljava/awt/Component;IJIICI)V");
	self = env->NewObject(cls, methodId, source->get(), id, when, modifiers, keyCode, keyChar, keyLocation);
	env->DeleteLocalRef(std::exchange(self, static_cast<jclass>(env->NewGlobalRef(self))));
}

KeyEvent::~KeyEvent()
{
	env->DeleteGlobalRef(cls);
	env->DeleteGlobalRef(self);
}

void KeyEvent::Dispatch(JNIEnv* env, Component* receiver, Component* source, std::int32_t id, std::int64_t when, std::int32_t modifiers, std::int32_t keyCode, jchar keyChar, std::int32_t keyLocation)
{
	jclass cls = env->FindClass("java/awt/event/KeyEvent");
	if (cls)
	{
		static jmethodID methodId = env->GetMethodID(cls, "<init>", "(Ljava/awt/Component;IJIICI)V");
		jobject event = env->NewObject(cls, methodId, source->get(), id, when, modifiers, keyCode, keyChar, keyLocation);
		if (event)
		{
			receiver->dispatchEvent(event);
			env->DeleteLocalRef(event);
		}
		
		env->DeleteLocalRef(cls);
	}
}

MouseEvent::MouseEvent(JNIEnv* env, Component* source, std::int32_t id, std::int64_t when, std::int32_t modifiers, std::int32_t x, std::int32_t y, std::int32_t clickCount, bool popupTrigger, std::int32_t button) : AWTEvent(), env(env), cls(nullptr)
{
	this->cls = env->FindClass("java/awt/event/MouseEvent");
	env->DeleteLocalRef(std::exchange(this->cls, static_cast<jclass>(env->NewGlobalRef(this->cls))));
	
	static jmethodID methodId = env->GetMethodID(cls, "<init>", "(Ljava/awt/Component;IJIIIIZI)V");
	self = env->NewObject(cls, methodId, source->get(), id, when, modifiers, x, y, clickCount, popupTrigger, button);
	env->DeleteLocalRef(std::exchange(self, static_cast<jclass>(env->NewGlobalRef(self))));
}

MouseEvent::~MouseEvent()
{
	env->DeleteGlobalRef(cls);
	env->DeleteGlobalRef(self);
}
	
void MouseEvent::Dispatch(JNIEnv* env, Component* receiver, Component* source, std::int32_t id, std::int64_t when, std::int32_t modifiers, std::int32_t x, std::int32_t y, std::int32_t clickCount, bool popupTrigger, std::int32_t button)
{
	jclass cls = env->FindClass("java/awt/event/MouseEvent");
	if (cls)
	{
		static jmethodID methodId = env->GetMethodID(cls, "<init>", "(Ljava/awt/Component;IJIIIIZI)V");
		jobject event = env->NewObject(cls, methodId, source->get(), id, when, modifiers, x, y, clickCount, popupTrigger, button);
		if (event)
		{
			receiver->dispatchEvent(event);
			env->DeleteLocalRef(event);
		}
		
		env->DeleteLocalRef(cls);
	}
}

std::int32_t InputEvent::GetDownMaskForButton(std::int32_t button)
{
	static const std::int32_t masks[] = {
		InputEventMasks::BUTTON1_DOWN_MASK,
		InputEventMasks::BUTTON2_DOWN_MASK,
		InputEventMasks::BUTTON3_DOWN_MASK,
		1 << 14, //4th physical button (this is not a wheel!)
		1 << 15, //(this is not a wheel!)
		1 << 16,
		1 << 17,
		1 << 18,
		1 << 19,
		1 << 20,
		1 << 21,
		1 << 22,
		1 << 23,
		1 << 24,
		1 << 25,
		1 << 26,
		1 << 27,
		1 << 28,
		1 << 29,
		1 << 30
	};
	
	return masks[button - 1];
}
