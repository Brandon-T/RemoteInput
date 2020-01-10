//
//  Component.hxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#ifndef COMPONENT_HXX_INCLUDED
#define COMPONENT_HXX_INCLUDED

#include <jni.h>
#include <cstdint>
#include <cstddef>
#include "AWTEvent.hxx"

class Component
{
protected:
	JNIEnv* env;
	jclass cls;
	jobject component;
	bool canDelete;
	
public:
	Component(JNIEnv* env, jobject component, bool canDelete = true);
	Component(JNIEnv* env, jclass cls, jobject component, bool canDelete = true);
	Component(Component&& other);
	Component(const Component& other) = delete;
	virtual ~Component();


	Component& operator = (Component&& other);
	Component& operator = (const Component& other) = delete;
	
	JNIEnv* getEnv();
	jobject get();
	
	void getLocationOnScreen(std::int32_t &x, std::int32_t &y);
	void getMousePosition(std::int32_t &x, std::int32_t &y);
	void getLocation(std::int32_t &x, std::int32_t &y);
	void getSize(std::size_t &width, std::size_t &height);
	Component getComponentAt(std::int32_t x, std::int32_t y);
	void dispatchEvent(AWTEvent* event);
	void dispatchEvent(jobject event);
	
	
	bool isVisible();
	bool isValid();
	
	bool hasFocus();
	bool requestFocusInWindow();
	void requestFocus();
};

#endif /* COMPONENT_HXX_INCLUDED */
