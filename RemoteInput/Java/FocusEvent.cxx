//
//  FocusEvent.cxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#include "FocusEvent.hxx"
#include <utility>

FocusEvent::FocusEvent(JNIEnv* env, Component* receiver, std::int32_t id, bool temporary, Cause cause) : AWTEvent(), env(env), cls(nullptr)
{
	this->cls = env->FindClass("java/awt/event/FocusEvent");
	env->DeleteLocalRef(std::exchange(this->cls, static_cast<jclass>(env->NewGlobalRef(this->cls))));
	
	static jmethodID methodId = env->GetMethodID(cls, "<init>", "(Ljava/awt/Component;IZ)V");
	self = env->NewObject(cls, methodId, receiver->get(), id, temporary);
	env->DeleteLocalRef(std::exchange(self, static_cast<jclass>(env->NewGlobalRef(self))));
}

FocusEvent::~FocusEvent()
{
	env->DeleteGlobalRef(cls);
	env->DeleteGlobalRef(self);
}

void FocusEvent::Dispatch(JNIEnv* env, Component* receiver, std::int32_t id, bool temporary, Cause cause, bool is_system_generated)
{
	jclass cls = env->FindClass("java/awt/event/FocusEvent");
	if (cls)
	{
		static jmethodID methodId = env->GetMethodID(cls, "<init>", "(Ljava/awt/Component;IZ)V");
		jobject event = env->NewObject(cls, methodId, receiver->get(), id, temporary);
		
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

void FocusEvent::Post(JNIEnv* env, Component* receiver, std::int32_t id, bool temporary, Cause cause, bool is_system_generated)
{
	jclass cls = env->FindClass("java/awt/event/FocusEvent");
	if (cls)
	{
		static jmethodID methodId = env->GetMethodID(cls, "<init>", "(Ljava/awt/Component;IZ)V");
		jobject event = env->NewObject(cls, methodId, receiver->get(), id, temporary);
		
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

jobject FocusEvent::GetCauseDescription(JNIEnv* env, Cause cause)
{
	//Java 9+
	static const char* const causes[] = {
		"UNKNOWN",
        "MOUSE_EVENT",
        "TRAVERSAL",
        "TRAVERSAL_UP",
        "TRAVERSAL_DOWN",
        "TRAVERSAL_FORWARD",
        "TRAVERSAL_BACKWARD",
        "ROLLBACK",
        "UNEXPECTED",
        "ACTIVATION",
        "CLEAR_GLOBAL_FOCUS_OWNER"
	};

	jclass cause_cls = env->FindClass("java/awt/event/FocusEvent$Cause");
	if (cause_cls)
	{
		static jfieldID fieldId = env->GetStaticFieldID(cause_cls , causes[static_cast<std::uint32_t>(cause)], "Ljava/awt/event/FocusEvent$Cause;");
		return env->GetStaticObjectField(cause_cls, fieldId);
	}
	return nullptr;
}
