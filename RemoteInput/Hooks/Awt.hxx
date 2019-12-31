//
//  Awt.hxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-27.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef Canvas_hpp
#define Canvas_hpp

#include <jni.h>
#include "Reflection.hxx"

class AWTEvent
{
protected:
	jobject self;
	
public:
	AWTEvent();
	AWTEvent(jobject self);
	AWTEvent(AWTEvent&& other);
	AWTEvent(const AWTEvent& other) = delete;
	
	AWTEvent& operator = (AWTEvent&& other);
	AWTEvent& operator = (const AWTEvent& other) = delete;
	
	jobject get();
};


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
	~Component();


	Component& operator = (Component&& other);
	Component& operator = (const Component& other) = delete;
	
	JNIEnv* getEnv();
	jobject get();
	
	void getMousePosition(std::int32_t &x, std::int32_t &y);
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

class Frame final : public Component
{
public:
	Frame(JNIEnv* env, jobject frame, bool canDelete = true);
	
	Component getContentPane();
};

class Applet final : public Component
{
public:
	Applet(JNIEnv* env, jobject applet, bool canDelete = true);
	
	Component getComponent(std::int32_t index);
};



//MARK: - Events

class FocusEvent final : public AWTEvent
{
private:
	JNIEnv* env;
	jclass cls;
	
public:
	FocusEvent(JNIEnv* env, Component* receiver, std::int32_t id, bool temporary);
	~FocusEvent();
	
	static void Dispatch(JNIEnv* env, Component* receiver, std::int32_t id, bool temporary);
	
	enum FocusCodes : std::int32_t
	{
		FOCUS_GAINED = 1004,
		FOCUS_LOST = 1005
	};
	
	enum FocusCauses : std::uint32_t
	{
		ACTIVATION,
		MOUSE_EVENT
	};
};

class KeyEvent final : public AWTEvent
{
private:
	JNIEnv* env;
	jclass cls;
	
public:
	KeyEvent(JNIEnv* env, Component* source, std::int32_t id, std::int64_t when, std::int32_t modifiers, std::int32_t keyCode, jchar keyChar, std::int32_t keyLocation);
	~KeyEvent();
	
	static void Dispatch(JNIEnv* env, Component* receiver, Component* source, std::int32_t id, std::int64_t when, std::int32_t modifiers, std::int32_t keyCode, jchar keyChar, std::int32_t keyLocation);
	
	enum KeyCodes : std::int32_t
	{
		KEY_TYPED = 400,
		KEY_PRESSED = 401,
		KEY_RELEASED = 402,
		
		VK_UNDEFINED = 0x0,
		CHAR_UNDEFINED = 0xFF,
		KEY_LOCATION_UNKNOWN = 0x0,
		KEY_LOCATION_STANDARD = 0x1,
		KEY_LOCATION_LEFT = 0x2,
		KEY_LOCATION_RIGHT = 0x3,
		KEY_LOCATION_NUMPAD = 0x4
	};
};

class MouseEvent final : public AWTEvent
{
private:
	JNIEnv* env;
	jclass cls;
	
public:
	MouseEvent(JNIEnv* env, Component* source, std::int32_t id, std::int64_t when, std::int32_t modifiers, std::int32_t x, std::int32_t y, std::int32_t clickCount, bool popupTrigger, std::int32_t button);
	~MouseEvent();
	
	static void Dispatch(JNIEnv* env, Component* receiver, Component* source, std::int32_t id, std::int64_t when, std::int32_t modifiers, std::int32_t x, std::int32_t y, std::int32_t clickCount, bool popupTrigger, std::int32_t button);
	
	enum MouseEventCodes : std::uint32_t
	{
		MOUSE_CLICKED = 500,
		MOUSE_PRESSED = 501,
		MOUSE_RELEASED = 502,
		MOUSE_MOVED = 503,
		MOUSE_ENTERED = 504,
		MOUSE_EXITED = 505,
		MOUSE_DRAGGED = 506,
		MOUSE_WHEEL = 507,
		
		NOBUTTON = 0,
		BUTTON1 = 1,
		BUTTON2 = 2,
		BUTTON3 = 3,
	};
};

struct InputEvent
{
	enum InputEventMasks : std::uint32_t
	{
		SHIFT_DOWN_MASK = 1 << 6,
		CTRL_DOWN_MASK = 1 << 7,
		META_DOWN_MASK = 1 << 8,
		ALT_DOWN_MASK = 1 << 9,
		BUTTON1_DOWN_MASK = 1 << 10,
		BUTTON2_DOWN_MASK = 1 << 11,
		BUTTON3_DOWN_MASK = 1 << 12,
		ALT_GRAPH_DOWN_MASK = 1 << 13
	};
	
	static std::int32_t GetDownMaskForButton(std::int32_t button);
};

#endif /* Canvas_hpp */
