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
#include "Window.hxx"

class Applet final : public Window
{
public:
	Applet(JNIEnv* env, jobject applet, bool canDelete = true) noexcept;
};

#endif /* APPLET_HXX_INCLUDED */
