//
//  AWTAccessor.hxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#ifndef AWTAccessor_HXX_INCLUDED
#define AWTAccessor_HXX_INCLUDED

#include <jni.h>
#include "AWTEventAccessor.hxx"

class AWTAccessor
{
private:
	JNIEnv* env;
	jclass cls;
	jobject accessor;
	
	AWTAccessor(JNIEnv* env, jclass cls, jobject accessor);
	
public:
	AWTAccessor(AWTAccessor&& other);
	AWTAccessor(const AWTAccessor& other) = delete;
	~AWTAccessor();
	
	AWTAccessor& operator = (AWTAccessor&& other);
	AWTAccessor& operator = (const AWTAccessor& other) = delete;
	
	static AWTEventAccessor getAWTEventAccessor(JNIEnv* env);
};

#endif /* AWTAccessor_HXX_INCLUDED */
