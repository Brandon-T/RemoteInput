//
//  AWTEvent.cxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#include "AWTEvent.hxx"

AWTEvent::AWTEvent() : self(nullptr)
{
}

AWTEvent::~AWTEvent()
{
}

AWTEvent::AWTEvent(jobject self) : self(self)
{
}

AWTEvent::AWTEvent(AWTEvent&& other) : self(other.self)
{
	other.self = nullptr;
}

AWTEvent& AWTEvent::operator = (AWTEvent&& other)
{
	this->self = other.self;
	other.self = nullptr;
	return *this;
}

jobject AWTEvent::get()
{
	return self;
}
