//
//  EventQueue.cxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#include "EventQueue.hxx"
#include <utility>

EventQueue::EventQueue(JNIEnv* env, jclass cls, jobject queue) : env(env), cls(cls), queue(queue)
{
	env->DeleteLocalRef(std::exchange(this->cls, static_cast<jclass>(env->NewGlobalRef(this->cls))));
	env->DeleteLocalRef(std::exchange(this->queue, env->NewGlobalRef(this->queue)));
}

EventQueue::~EventQueue()
{
	if (cls)
	{
		env->DeleteGlobalRef(cls);
	}
	
	if (queue)
	{
		env->DeleteGlobalRef(queue);
	}
}

EventQueue::EventQueue(EventQueue&& other) : env(other.env), cls(other.cls), queue(other.queue)
{
	other.env = nullptr;
	other.cls = nullptr;
	other.queue = nullptr;
}

EventQueue& EventQueue::operator = (EventQueue&& other)
{
	this->env = other.env;
	this->cls = other.cls;
	this->queue = other.queue;
	other.env = nullptr;
	other.cls = nullptr;
	other.queue = nullptr;
	return *this;
}

void EventQueue::postEvent(AWTEvent* event)
{
	static jmethodID methodId = env->GetMethodID(cls, "postEvent", "(Ljava/awt/AWTEvent;)V");
	env->CallVoidMethod(queue, methodId, event->get());
}
