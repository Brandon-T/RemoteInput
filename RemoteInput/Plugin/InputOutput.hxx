//
//  InputOutput.hxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-28.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef INPUTOUTPUT_HXX_INCLUDED
#define INPUTOUTPUT_HXX_INCLUDED

#include "Awt.hxx"
#include <cstdint>
#include <list>
#include <atomic>
#include <mutex>
#include "ThreadPool.hxx"
#include "Reflection.hxx"

class InputOutput
{
private:
	JavaVM* vm;
	jobject applet;
	
	std::mutex mutex;
	ThreadPool input_thread;
	
	std::atomic<std::int32_t> currently_held_key;
	std::list<std::int32_t> held_keys;
	
	std::int32_t CharToJavaKeyCode(char c);
	jchar NativeKeyCodeToChar(std::int32_t keycode);
	std::int32_t GetJavaKeyCode(std::int32_t native_key_code);
	std::int32_t GetKeyLocation(std::int32_t keycode);
	
	bool has_focus(Component* component);
	void gain_focus(Component* component);
	void lose_focus(Component* component);
	
public:
	InputOutput(Reflection* reflector);
	void hold_key(std::int32_t code);
	void release_key(std::int32_t code);
	bool is_key_held(std::int32_t code);
	void send_string(std::string string, std::int32_t keywait, std::int32_t keymodwait);
};

#endif /* INPUTOUTPUT_HXX_INCLUDED */
