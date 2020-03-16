//
//  PointerInfo.hxx
//  RemoteInput
//
//  Created by Brandon on 2020-03-16.
//  Copyright © 2020 XIO. All rights reserved.
//

#ifndef PointerInfo_HXX_INCLUDED
#define PointerInfo_HXX_INCLUDED

#include <jni.h>
#include "Component.hxx"

class PointerInfo
{
private:
	JNIEnv* env;
	jclass cls;
	jobject pointerInfo;
	
	PointerInfo(JNIEnv* env, jclass cls, jobject pointerInfo);
	
public:
	PointerInfo(PointerInfo&& other);
	PointerInfo(const PointerInfo& other) = delete;
	~PointerInfo();
	
	PointerInfo& operator = (PointerInfo&& other);
	PointerInfo& operator = (const PointerInfo& other) = delete;
	
	static PointerInfo getPointerInfo(JNIEnv* env);
	
	void getLocation(std::int32_t &x, std::int32_t &y);
	
	static void PointToScreen(JNIEnv* env, std::int32_t &x, std::int32_t &y, Component* component);
};

#endif /* PointerInfo_HXX_INCLUDED */
