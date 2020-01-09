//
//  Toolkit.hxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#ifndef Toolkit_HXX_INCLUDED
#define Toolkit_HXX_INCLUDED

#include <jni.h>
#include "EventQueue.hxx"

class Toolkit
{
private:
	JNIEnv* env;
	jclass cls;
	jobject toolkit;
	
	Toolkit(JNIEnv* env, jclass cls, jobject toolkit);
	
public:
	Toolkit(Toolkit&& other);
	Toolkit(const Toolkit& other) = delete;
	~Toolkit();
	
	Toolkit& operator = (Toolkit&& other);
	Toolkit& operator = (const Toolkit& other) = delete;
	
	static Toolkit getDefaultToolkit(JNIEnv* env);
	
	EventQueue getSystemEventQueue();
};

#endif /* Toolkit_HXX_INCLUDED */
