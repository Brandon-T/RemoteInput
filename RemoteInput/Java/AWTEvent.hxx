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
	AWTEvent() noexcept;
	AWTEvent(jobject self) noexcept;
	AWTEvent(AWTEvent&& other) noexcept;
	AWTEvent(const AWTEvent& other) = delete;
	virtual ~AWTEvent() noexcept;
	
	AWTEvent& operator = (AWTEvent&& other) noexcept;
	AWTEvent& operator = (const AWTEvent& other) = delete;
	
	jobject get() const noexcept;
};

#endif /* AWTEvent_HXX_INCLUDED */
