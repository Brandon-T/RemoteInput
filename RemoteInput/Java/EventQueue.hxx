//
//  EventQueue.hxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#ifndef EventQueue_HXX_INCLUDED
#define EventQueue_HXX_INCLUDED

#include <jni.h>
#include "AWTEvent.hxx"

class EventQueue
{
private:
	JNIEnv* env;
	jclass cls;
	jobject queue;
	
public:
	EventQueue(JNIEnv* env, jclass cls, jobject queue) noexcept;
	EventQueue(EventQueue&& other) noexcept;
	EventQueue(const EventQueue& other) = delete;
	~EventQueue() noexcept;
	
	EventQueue& operator = (EventQueue&& other) noexcept;
	EventQueue& operator = (const EventQueue& other) = delete;
	
	void postEvent(AWTEvent* event) const noexcept;
};

#endif /* EventQueue_HXX_INCLUDED */
