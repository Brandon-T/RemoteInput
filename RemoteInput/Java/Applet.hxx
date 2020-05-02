//
//  Applet.hxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#ifndef APPLET_HXX_INCLUDED
#define APPLET_HXX_INCLUDED

#include <jni.h>
#include "Component.hxx"

class Applet final : public Component
{
public:
	Applet(JNIEnv* env, jobject applet, bool canDelete = true) noexcept;
	
	Component getComponent(std::int32_t index) const noexcept;
};

#endif /* APPLET_HXX_INCLUDED */
