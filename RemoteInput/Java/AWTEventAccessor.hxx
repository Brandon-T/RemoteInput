//
//  AWTEventAccessor.hxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#ifndef AWTEventAccessor_HXX_INCLUDED
#define AWTEventAccessor_HXX_INCLUDED

#include <jni.h>
#include "AWTEvent.hxx"

class AWTEventAccessor
{
private:
	JNIEnv* env;
	jclass cls;
	jobject accessor;
	
public:
	AWTEventAccessor(JNIEnv* env, jclass cls, jobject accessor);
	AWTEventAccessor(AWTEventAccessor&& other);
	AWTEventAccessor(const AWTEventAccessor& other) = delete;
	~AWTEventAccessor();
	
	AWTEventAccessor& operator = (AWTEventAccessor&& other);
	AWTEventAccessor& operator = (const AWTEventAccessor& other) = delete;
	
	void setSystemGenerated(AWTEvent* event);
	void setPosted(AWTEvent* event);
};

#endif /* AWTEventAccessor_HXX_INCLUDED */
