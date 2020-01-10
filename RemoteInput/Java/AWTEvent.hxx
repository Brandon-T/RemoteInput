//
//  AWTEvent.hxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#ifndef AWTEvent_HXX_INCLUDED
#define AWTEvent_HXX_INCLUDED

#include <jni.h>

class AWTEvent
{
protected:
	jobject self;
	
public:
	AWTEvent();
	AWTEvent(jobject self);
	AWTEvent(AWTEvent&& other);
	AWTEvent(const AWTEvent& other) = delete;
	virtual ~AWTEvent();
	
	AWTEvent& operator = (AWTEvent&& other);
	AWTEvent& operator = (const AWTEvent& other) = delete;
	
	jobject get();
};

#endif /* AWTEvent_HXX_INCLUDED */
