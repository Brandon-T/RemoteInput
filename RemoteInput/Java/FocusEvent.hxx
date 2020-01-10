//
//  FocusEvent.hxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#ifndef FocusEvent_HXX_INCLUDED
#define FocusEvent_HXX_INCLUDED

#include <jni.h>
#include "AWTEvent.hxx"
#include "Component.hxx"

class FocusEvent final : public AWTEvent
{
public:
	enum class Cause : std::uint32_t;

private:
	JNIEnv* env;
	jclass cls;
	static jobject GetCauseDescription(JNIEnv* env, Cause cause);

public:
	FocusEvent(JNIEnv* env, Component* receiver, std::int32_t id, bool temporary, Cause cause);
	~FocusEvent();

	static void Dispatch(JNIEnv* env, Component* receiver, std::int32_t id, bool temporary, Cause cause, bool is_system_generated = false);
	static void Post(JNIEnv* env, Component* receiver, std::int32_t id, bool temporary, Cause cause, bool is_system_generated = false);

	#warning "WINDOWS BS"
	#ifdef MOUSE_EVENT
	#undef MOUSE_EVENT
	#endif

	enum class Cause : std::uint32_t
	{
		UNKNOWN,
        MOUSE_EVENT,
        TRAVERSAL,
        TRAVERSAL_UP,
        TRAVERSAL_DOWN,
        TRAVERSAL_FORWARD,
        TRAVERSAL_BACKWARD,
        ROLLBACK,
        UNEXPECTED,
        ACTIVATION,
        CLEAR_GLOBAL_FOCUS_OWNER
	};

	enum FocusCodes : std::int32_t
	{
		FOCUS_GAINED = 1004,
		FOCUS_LOST = 1005
	};
};

#endif /* FocusEvent_HXX_INCLUDED */
