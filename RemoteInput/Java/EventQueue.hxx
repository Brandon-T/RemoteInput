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
	EventQueue(JNIEnv* env, jclass cls, jobject queue);
	EventQueue(EventQueue&& other);
	EventQueue(const EventQueue& other) = delete;
	~EventQueue();
	
	EventQueue& operator = (EventQueue&& other);
	EventQueue& operator = (const EventQueue& other) = delete;
	
	void postEvent(AWTEvent* event);
};

#endif /* EventQueue_HXX_INCLUDED */
