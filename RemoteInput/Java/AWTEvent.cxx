//
//  AWTEvent.cxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#include "AWTEvent.hxx"

AWTEvent::AWTEvent() noexcept : self(nullptr)
{
}

AWTEvent::~AWTEvent() noexcept
{
}

AWTEvent::AWTEvent(jobject self) noexcept : self(self)
{
}

AWTEvent::AWTEvent(AWTEvent&& other) noexcept : self(other.self)
{
	other.self = nullptr;
}

AWTEvent& AWTEvent::operator = (AWTEvent&& other) noexcept
{
	this->self = other.self;
	other.self = nullptr;
	return *this;
}

jobject AWTEvent::get() const noexcept
{
	return self;
}
