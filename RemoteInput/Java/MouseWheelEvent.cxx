//
//  MouseWheelEvent.cxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-10.
//  Copyright © 2020 XIO. All rights reserved.
//

#include "MouseWheelEvent.hxx"
#include <utility>

MouseWheelEvent::MouseWheelEvent(JNIEnv* env, Component* source, std::int32_t id, std::int64_t when, std::int32_t modifiers, std::int32_t x, std::int32_t y, std::int32_t xAbs, std::int32_t yAbs, std::int32_t clickCount, bool popupTrigger, std::int32_t scrollType, std::int32_t scrollAmount, std::int32_t wheelRotation, double preciseWheelRotation) noexcept : AWTEvent(), env(env), cls(nullptr)
{
	this->cls = env->FindClass("java/awt/event/MouseWheelEvent");
	env->DeleteLocalRef(std::exchange(this->cls, static_cast<jclass>(env->NewGlobalRef(this->cls))));
	
	static jmethodID methodId = env->GetMethodID(cls, "<init>", "(Ljava/awt/Component;IJIIIIIIZIIID)V");
	self = env->NewObject(cls, methodId, source->get(), id, when, modifiers, x, y, xAbs, yAbs, clickCount, popupTrigger, scrollType, scrollAmount, wheelRotation, preciseWheelRotation);
	env->DeleteLocalRef(std::exchange(self, static_cast<jclass>(env->NewGlobalRef(self))));
}

MouseWheelEvent::~MouseWheelEvent() noexcept
{
	env->DeleteGlobalRef(cls);
	env->DeleteGlobalRef(self);
}
	
void MouseWheelEvent::Dispatch(JNIEnv* env, Component* receiver, Component* source, std::int32_t id, std::int64_t when, std::int32_t modifiers, std::int32_t x, std::int32_t y, std::int32_t xAbs, std::int32_t yAbs, std::int32_t clickCount, bool popupTrigger, std::int32_t scrollType, std::int32_t scrollAmount, std::int32_t wheelRotation, double preciseWheelRotation, bool is_system_generated) noexcept
{
	jclass cls = env->FindClass("java/awt/event/MouseWheelEvent");
	if (cls)
	{
		static jmethodID methodId = env->GetMethodID(cls, "<init>", "(Ljava/awt/Component;IJIIIIIIZIIID)V");
		jobject event = env->NewObject(cls, methodId, source->get(), id, when, modifiers, x, y, xAbs, yAbs, clickCount, popupTrigger, scrollType, scrollAmount, wheelRotation, preciseWheelRotation);
		if (event)
		{
			if (is_system_generated)
			{
				//Get AWTEventAccessor
				jclass cls = env->FindClass("sun/awt/AWTEventAccessor");
				static jmethodID accessor_methodId = env->GetStaticMethodID(cls, "getAWTEventAccessor", "()Lsun/awt/AWTEventAccessor;");
				jobject accessor = env->CallStaticObjectMethod(cls, accessor_methodId);
				
				//Set System Generated
				static jmethodID system_generated_methodId = env->GetMethodID(cls, "setSystemGenerated", "(Ljava/awt/AWTEvent;)V");
				env->CallVoidMethod(accessor, system_generated_methodId, event);
				
				//Cleanup
				env->DeleteLocalRef(accessor);
				env->DeleteLocalRef(cls);
			}
			
			receiver->dispatchEvent(event);
			env->DeleteLocalRef(event);
		}
		
		env->DeleteLocalRef(cls);
	}
}

void MouseWheelEvent::Post(JNIEnv* env, Component* source, std::int32_t id, std::int64_t when, std::int32_t modifiers, std::int32_t x, std::int32_t y, std::int32_t xAbs, std::int32_t yAbs, std::int32_t clickCount, bool popupTrigger, std::int32_t scrollType, std::int32_t scrollAmount, std::int32_t wheelRotation, double preciseWheelRotation, bool is_system_generated) noexcept
{
	jclass cls = env->FindClass("java/awt/event/MouseWheelEvent");
	if (cls)
	{
		static jmethodID methodId = env->GetMethodID(cls, "<init>", "(Ljava/awt/Component;IJIIIIIIZIIID)V");
		jobject event = env->NewObject(cls, methodId, source->get(), id, when, modifiers, x, y, xAbs, yAbs, clickCount, popupTrigger, scrollType, scrollAmount, wheelRotation, preciseWheelRotation);
		if (event)
		{
			if (is_system_generated)
			{
				//Get AWTEventAccessor
				jclass cls = env->FindClass("sun/awt/AWTEventAccessor");
				static jmethodID accessor_methodId = env->GetStaticMethodID(cls, "getAWTEventAccessor", "()Lsun/awt/AWTEventAccessor;");
				jobject accessor = env->CallStaticObjectMethod(cls, accessor_methodId);
				
				//Set System Generated
				static jmethodID system_generated_methodId = env->GetMethodID(cls, "setSystemGenerated", "(Ljava/awt/AWTEvent;)V");
				env->CallVoidMethod(accessor, system_generated_methodId, event);
				
				//Cleanup
				env->DeleteLocalRef(accessor);
				env->DeleteLocalRef(cls);
			}
			
			//Get Default Toolkit
			jclass cls = env->FindClass("java/awt/Toolkit");
			static jmethodID toolkit_methodId = env->GetStaticMethodID(cls, "getDefaultToolkit", "()Ljava/awt/Toolkit;");
			jobject toolkit = env->CallStaticObjectMethod(cls, toolkit_methodId);
			
			//Get System Event Queue
			static jmethodID queue_methodId = env->GetMethodID(cls, "getSystemEventQueue", "()Ljava/awt/EventQueue;");
			jobject queue = env->CallObjectMethod(toolkit, queue_methodId);
			
			//Post Event
			static jmethodID post_methodId = env->GetMethodID(cls, "postEvent", "(Ljava/awt/AWTEvent;)V");
			env->CallVoidMethod(queue, post_methodId, event);
			
			//Cleanup
			env->DeleteLocalRef(queue);
			env->DeleteLocalRef(toolkit);
			env->DeleteLocalRef(cls);
			
			env->DeleteLocalRef(event);
		}
		
		env->DeleteLocalRef(cls);
	}
}
