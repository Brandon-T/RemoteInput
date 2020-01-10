//
//  InputOutput.hxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-28.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef INPUTOUTPUT_HXX_INCLUDED
#define INPUTOUTPUT_HXX_INCLUDED

#include <cstdint>
#include <list>
#include <atomic>
#include <mutex>
#include <array>
#include "DetachedThreadPool.hxx"
#include "Reflection.hxx"
#include "Component.hxx"

class InputOutput final
{
private:
	JavaVM* vm;
	jobject applet;
	std::mutex mutex;
	DetachedThreadPool input_thread;

	// MARK: - Input Variables
	std::atomic<std::int32_t> currently_held_key;
	std::list<std::int32_t> held_keys;
	std::int32_t x;
	std::int32_t y;
	std::size_t w;
	std::size_t h;
	std::int32_t click_count;
	bool mouse_buttons[3];

	// MARK: - Functions
	std::int32_t CharToJavaKeyCode(char c);
	jchar NativeKeyCodeToChar(std::int32_t keycode, std::int32_t modifiers);
	std::int32_t GetJavaKeyCode(std::int32_t native_key_code);
	std::int32_t GetKeyLocation(std::int32_t keycode);
	std::int32_t GetActiveKeyModifiers();
	std::int32_t ModifiersForChar(char c);
	std::int32_t SimbaMouseButtonToJava(std::int32_t button);

	bool has_focus(Component* component);
	void gain_focus(Component* component);
	void lose_focus(Component* component);

	bool any_key_held(std::array<std::int32_t, 4>&& keys);

public:
	InputOutput(Reflection* reflector);
	~InputOutput();

	void hold_key(std::int32_t code);
	void release_key(std::int32_t code);
	bool is_key_held(std::int32_t code);
	void send_string(std::string string, std::int32_t keywait, std::int32_t keymodwait);

	void get_mouse_position(std::int32_t* x, std::int32_t* y);
	void move_mouse(std::int32_t x, std::int32_t y);
	void hold_mouse(std::int32_t x, std::int32_t y, std::int32_t button);
	void release_mouse(std::int32_t x, std::int32_t y, std::int32_t button);
	void scroll_mouse(std::int32_t x, std::int32_t y, std::int32_t lines);
	bool is_mouse_held(std::int32_t button);


	//Internal
	void get_applet_dimensions(std::int32_t &x, std::int32_t &y, std::int32_t &width, std::int32_t &height);
	void get_applet_mouse_position(std::int32_t &x, std::int32_t &y);
};

#endif /* INPUTOUTPUT_HXX_INCLUDED */
