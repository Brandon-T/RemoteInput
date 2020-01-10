//
//  Frame.hxx
//  RemoteInput
//
//  Created by Brandon on 2020-01-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#ifndef FRAME_HXX_INCLUDED
#define FRAME_HXX_INCLUDED

#include <jni.h>
#include "Component.hxx"

class Frame final : public Component
{
public:
	Frame(JNIEnv* env, jobject frame, bool canDelete = true);
	
	Component getContentPane();
};

#endif /* FRAME_HXX_INCLUDED */
