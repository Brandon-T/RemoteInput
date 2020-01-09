//
//  InputOutput.cxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-28.
//  Copyright © 2019 XIO. All rights reserved.
//

#include "InputOutput.hxx"
#include "ControlCenter.hxx"
#include "Platform.hxx"
#include "Random.hxx"

enum ControlKeys : std::uint32_t
{
	VK_FUNCTION = 0x0,
	VK_NUM_LOCK = 0x90,
	VK_SHIFT = 0x10,
	VK_LSHIFT = 0xA0,
	VK_RSHIFT = 0xA1,
	VK_CONTROL = 0x11,
	VK_LEFT_CONTROL = 0xA2,
	VK_RIGHT_CONTROL = 0xA3,
	VK_LEFT_ALT = 0xA4,
	VK_RIGHT_ALT = 0xA5,

	VK_COMMAND = 0x9D, //VK_META
	VK_LEFT_WINDOWS = 0x5B,
	VK_RIGHT_WINDOWS = 0x5C,
	VK_APPS = 0x5D
};

static ControlKeys control_keys[] = {
	ControlKeys::VK_FUNCTION,
	ControlKeys::VK_NUM_LOCK,
	ControlKeys::VK_LSHIFT,
	ControlKeys::VK_RSHIFT,
	ControlKeys::VK_LEFT_CONTROL,
	ControlKeys::VK_RIGHT_CONTROL,
	ControlKeys::VK_LEFT_ALT,
	ControlKeys::VK_RIGHT_ALT,
	ControlKeys::VK_COMMAND,
	ControlKeys::VK_LEFT_WINDOWS,
	ControlKeys::VK_RIGHT_WINDOWS,
	ControlKeys::VK_APPS
};

static KeyEvent::KeyCodes control_keys_locations[] = {
	KeyEvent::KeyCodes::KEY_LOCATION_UNKNOWN,
	KeyEvent::KeyCodes::KEY_LOCATION_STANDARD,
	KeyEvent::KeyCodes::KEY_LOCATION_LEFT,
	KeyEvent::KeyCodes::KEY_LOCATION_LEFT,
	KeyEvent::KeyCodes::KEY_LOCATION_RIGHT,
	KeyEvent::KeyCodes::KEY_LOCATION_LEFT,
	KeyEvent::KeyCodes::KEY_LOCATION_LEFT,
	KeyEvent::KeyCodes::KEY_LOCATION_RIGHT,
	KeyEvent::KeyCodes::KEY_LOCATION_LEFT,
	KeyEvent::KeyCodes::KEY_LOCATION_RIGHT,
	KeyEvent::KeyCodes::KEY_LOCATION_LEFT,
	KeyEvent::KeyCodes::KEY_LOCATION_LEFT,
	KeyEvent::KeyCodes::KEY_LOCATION_RIGHT,
	KeyEvent::KeyCodes::KEY_LOCATION_RIGHT
};

InputOutput::InputOutput(Reflection* reflector) : vm(reflector->getVM()->getVM()), applet(reflector->getApplet()), input_thread(2), mutex(), currently_held_key(-1), held_keys(), x(-1), y(-1), w(-1), h(-1), click_count(0), mouse_buttons()
{
}

void InputOutput::hold_key(std::int32_t code)
{
	extern std::unique_ptr<ControlCenter> control_center;
	if (!control_center)
	{
		return;
	}
	
	//Key is not already held..
	if (std::find(held_keys.begin(), held_keys.end(), code) == held_keys.end())
	{
		//Key is a control key
		if (std::find(std::begin(control_keys), std::end(control_keys), code) != std::end(control_keys))
		{
			std::lock_guard<std::mutex>(this->mutex);
			
			//Control Keys only generate a single held event..
			held_keys.push_back(code);
			
			//DispatchEvent
			Component receiver = control_center->reflect_canvas();
			
			JNIEnv* env = receiver.getEnv();
			if (!this->has_focus(&receiver))
			{
				this->gain_focus(&receiver);
			}
			
			std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			std::int32_t modifiers = GetActiveKeyModifiers();
			std::int32_t keycode = GetJavaKeyCode(code);
			std::int32_t location = GetKeyLocation(code);
			
			KeyEvent::Dispatch(env,
							   &receiver,
							   &receiver,
							   KeyEvent::KeyCodes::KEY_PRESSED,
							   when,
							   modifiers,
							   keycode,
							   static_cast<jchar>(KeyEvent::KeyCodes::CHAR_UNDEFINED),
							   location);
		}
		else
		{
			//Key already being pressed so just replace it
			if (currently_held_key != -1)
			{
				std::lock_guard<std::mutex>(this->mutex);
				
				currently_held_key = code;
				held_keys.push_back(code);
				
				//Dispatch Event
				Component receiver = control_center->reflect_canvas();
				if (!this->has_focus(&receiver))
				{
					this->gain_focus(&receiver);
				}
				
				JNIEnv* env = receiver.getEnv();
				std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
				std::int32_t modifiers = GetActiveKeyModifiers();
				std::int32_t keycode = GetJavaKeyCode(code);
				std::int32_t location = GetKeyLocation(code);
				
				KeyEvent::Dispatch(env,
								   &receiver,
								   &receiver,
								   KeyEvent::KeyCodes::KEY_PRESSED,
								   when,
								   modifiers,
								   keycode,
								   NativeKeyCodeToChar(code, modifiers),
								   location);

				KeyEvent::Dispatch(env,
									&receiver,
									&receiver,
									KeyEvent::KeyCodes::KEY_TYPED,
									when,
									modifiers,
									0,
									NativeKeyCodeToChar(code, modifiers),
									KeyEvent::KeyCodes::KEY_LOCATION_UNKNOWN);
			}
			else
			{
				this->mutex.lock();
				currently_held_key = code;
				held_keys.push_back(code);
				
				//Dispatch Event
				Component receiver = control_center->reflect_canvas();
				if (!this->has_focus(&receiver))
				{
					this->gain_focus(&receiver);
				}
				
				JNIEnv* env = receiver.getEnv();
				std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
				std::int32_t modifiers = GetActiveKeyModifiers();
				std::int32_t keycode = GetJavaKeyCode(code);
				std::int32_t location = GetKeyLocation(code);
				
				KeyEvent::Dispatch(env,
								   &receiver,
								   &receiver,
								   KeyEvent::KeyCodes::KEY_PRESSED,
								   when,
								   modifiers,
								   keycode,
								   NativeKeyCodeToChar(code, modifiers),
								   location);

				KeyEvent::Dispatch(env,
									&receiver,
									&receiver,
									KeyEvent::KeyCodes::KEY_TYPED,
									when,
									modifiers,
									0,
									NativeKeyCodeToChar(code, modifiers),
									KeyEvent::KeyCodes::KEY_LOCATION_UNKNOWN);
				this->mutex.unlock();
				
				
				input_thread.add_task([&]{
					JNIEnv* env = nullptr;
					this->vm->AttachCurrentThreadAsDaemon(reinterpret_cast<void**>(&env), nullptr);
					
					Applet applet{env, this->applet, false};
					Component receiver = applet.getComponent(0);
					
					while (currently_held_key != -1)
					{
						std::lock_guard<std::mutex>(this->mutex);
						yield_thread(std::chrono::milliseconds(Random::instance()->generate_random_int(15, 70)));
						std::int32_t code = currently_held_key;
						
						//Dispatch Event
						if (!this->has_focus(&receiver))
						{
							this->gain_focus(&receiver);
						}
						
						std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
						std::int32_t modifiers = GetActiveKeyModifiers();
						std::int32_t keycode = GetJavaKeyCode(code);
						std::int32_t location = GetKeyLocation(code);
						
						KeyEvent::Dispatch(env,
										   &receiver,
										   &receiver,
										   KeyEvent::KeyCodes::KEY_PRESSED,
										   when,
										   modifiers,
										   keycode,
										   NativeKeyCodeToChar(code, modifiers),
										   location);

						KeyEvent::Dispatch(env,
											&receiver,
											&receiver,
											KeyEvent::KeyCodes::KEY_TYPED,
											when,
											modifiers,
											0,
											NativeKeyCodeToChar(code, modifiers),
											KeyEvent::KeyCodes::KEY_LOCATION_UNKNOWN);
					}
				});
			}
		}
	}
}

void InputOutput::release_key(std::int32_t code)
{
	extern std::unique_ptr<ControlCenter> control_center;
	if (!control_center)
	{
		return;
	}
	
	if (auto it = std::find(held_keys.begin(), held_keys.end(), code); it != held_keys.end())
	{
		if (std::find(std::begin(control_keys), std::end(control_keys), code) != std::end(control_keys))
		{
			std::lock_guard<std::mutex>(this->mutex);
			
			held_keys.erase(it); //held_keys.erase(std::remove(held_keys.begin(), held_keys.end(), code), held_keys.end());
			
			//Dispatch Event
			Component receiver = control_center->reflect_canvas();
			if (!this->has_focus(&receiver))
			{
				this->gain_focus(&receiver);
			}
			
			JNIEnv* env = receiver.getEnv();
			std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			std::int32_t modifiers = GetActiveKeyModifiers();
			std::int32_t keycode = GetJavaKeyCode(code);
			std::int32_t location = GetKeyLocation(code);

			KeyEvent::Dispatch(env,
							   &receiver,
							   &receiver,
							   KeyEvent::KeyCodes::KEY_RELEASED,
							   when,
							   modifiers,
							   keycode,
							   static_cast<jchar>(KeyEvent::KeyCodes::CHAR_UNDEFINED),
							   location);
		}
		else
		{
			//Remove the held key from the list..
			this->mutex.lock();
			held_keys.erase(it);
			
			//Find the next non-control held-key..
			auto jt = std::find_if(held_keys.crbegin(), held_keys.crend(), [](std::int32_t key){
				return std::find(std::begin(control_keys), std::end(control_keys), key) == std::end(control_keys);
			});
			
			//Set the next currently held key to the first non-control key..
			currently_held_key = jt != held_keys.crend() ? *jt : -1;
			this->mutex.unlock();

			//Dispatch Event
			std::lock_guard<std::mutex>(this->mutex);
			Component receiver = control_center->reflect_canvas();
			if (!this->has_focus(&receiver))
			{
				this->gain_focus(&receiver);
			}
			
			JNIEnv* env = receiver.getEnv();
			std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			std::int32_t modifiers = GetActiveKeyModifiers();
			std::int32_t keycode = GetJavaKeyCode(code);
			std::int32_t location = GetKeyLocation(code);

			KeyEvent::Dispatch(env,
							   &receiver,
							   &receiver,
							   KeyEvent::KeyCodes::KEY_RELEASED,
							   when,
							   modifiers,
							   keycode,
							   NativeKeyCodeToChar(code, modifiers),
							   location);
		}
	}
}

bool InputOutput::is_key_held(std::int32_t code)
{
	return std::find(std::begin(held_keys), std::end(held_keys), code) != std::end(held_keys);
}

bool InputOutput::any_key_held(std::array<std::int32_t, 4>&& keys)
{
	return std::any_of(std::cbegin(held_keys), std::cend(held_keys), [&](std::int32_t key){
		return std::find(std::cbegin(keys), std::cend(keys), key) != std::cend(keys);
	});
}

void InputOutput::send_string(std::string string, std::int32_t keywait, std::int32_t keymodwait)
{
	extern std::unique_ptr<ControlCenter> control_center;
	if (!control_center)
	{
		return;
	}
	
	Component receiver = control_center->reflect_canvas();
	JNIEnv* env = receiver.getEnv();
	
	if (!this->has_focus(&receiver))
	{
		this->gain_focus(&receiver);
	}
	
	bool isShiftDown = false;
	
	for (std::size_t i = 0; i < string.length(); ++i)
	{
		char c = string[i];
		char n = i == string.length() - 1 ? '\0' : string[i + 1];
		std::int32_t modifiers = this->ModifiersForChar(c);
		
		//Modifier Key
		if (modifiers & InputEvent::InputEventMasks::SHIFT_DOWN_MASK)
		{
			if (!isShiftDown)
			{
				isShiftDown = true;
				
				std::int32_t code = VK_LSHIFT;
				std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
				std::int32_t modifiers = InputEvent::InputEventMasks::SHIFT_DOWN_MASK;
				std::int32_t keycode = GetJavaKeyCode(code);
				std::int32_t location = GetKeyLocation(code);
				
				KeyEvent::Dispatch(env,
								   &receiver,
								   &receiver,
								   KeyEvent::KeyCodes::KEY_PRESSED,
								   when,
								   modifiers,
								   keycode,
								   static_cast<jchar>(KeyEvent::KeyCodes::CHAR_UNDEFINED),
								   location);

				yield_thread(std::chrono::milliseconds(Random::instance()->generate_random_int(50, 70) + keymodwait));
			}
		}
		
		//Character Key
		std::int32_t code = static_cast<std::int32_t>(c);
		std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		std::int32_t keycode = GetJavaKeyCode(code);
		std::int32_t location = GetKeyLocation(code);
		
		KeyEvent::Dispatch(env,
						   &receiver,
						   &receiver,
						   KeyEvent::KeyCodes::KEY_PRESSED,
						   when,
						   modifiers,
						   keycode,
						   NativeKeyCodeToChar(code, modifiers),
						   location);

		KeyEvent::Dispatch(env,
							&receiver,
							&receiver,
							KeyEvent::KeyCodes::KEY_TYPED,
							when,
							modifiers,
							0,
							NativeKeyCodeToChar(code, modifiers),
							KeyEvent::KeyCodes::KEY_LOCATION_UNKNOWN);
		
		yield_thread(std::chrono::milliseconds(Random::instance()->generate_random_int(15, 70) + keywait));
		when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

		KeyEvent::Dispatch(env,
						   &receiver,
						   &receiver,
						   KeyEvent::KeyCodes::KEY_RELEASED,
						   when,
						   modifiers,
						   keycode,
						   NativeKeyCodeToChar(code, modifiers),
						   location);
		
		yield_thread(std::chrono::milliseconds(Random::instance()->generate_random_int(15, 70) + keywait));
		
		//Modifier Key
		if ((isShiftDown && i == string.length() - 1) || (n != '\0' && !(this->ModifiersForChar(n) & InputEvent::InputEventMasks::SHIFT_DOWN_MASK)))
		{
			isShiftDown = false;
			
			std::int32_t code = VK_LSHIFT;
			std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			std::int32_t modifiers = InputEvent::InputEventMasks::SHIFT_DOWN_MASK;
			std::int32_t keycode = GetJavaKeyCode(code);
			std::int32_t location = GetKeyLocation(code);

			KeyEvent::Dispatch(env,
							   &receiver,
							   &receiver,
							   KeyEvent::KeyCodes::KEY_RELEASED,
							   when,
							   modifiers,
							   keycode,
							   static_cast<jchar>(KeyEvent::KeyCodes::CHAR_UNDEFINED),
							   location);
			
			yield_thread(std::chrono::milliseconds(Random::instance()->generate_random_int(50, 70) + keymodwait));
		}
	}
}

bool InputOutput::has_focus(Component* component)
{
	return component->hasFocus();
}

void InputOutput::gain_focus(Component* component)
{
	FocusEvent::Dispatch(component->getEnv(), component, FocusEvent::FocusCodes::FOCUS_GAINED, false);
}

void InputOutput::lose_focus(Component* component)
{
	FocusEvent::Dispatch(component->getEnv(), component, FocusEvent::FocusCodes::FOCUS_LOST, true);
}

void InputOutput::get_mouse_position(std::int32_t* x, std::int32_t* y)
{
	extern std::unique_ptr<ControlCenter> control_center;
	if (!control_center)
	{
		return;
	}
	
	if (this->x < 0 || this->y < 0)
	{
		Component receiver = control_center->reflect_canvas();
		receiver.getMousePosition(this->x, this->y);
		receiver.getSize(this->w, this->h);
	}
	
	*x = this->x;
	*y = this->y;
}

void InputOutput::move_mouse(std::int32_t x, std::int32_t y)
{
	extern std::unique_ptr<ControlCenter> control_center;
	if (!control_center)
	{
		return;
	}
	
	Component receiver = control_center->reflect_canvas();
	JNIEnv* env = receiver.getEnv();
	
	receiver.getMousePosition(this->x, this->y);
	receiver.getSize(this->w, this->h);
	
	if (!this->has_focus(&receiver))
	{
		this->gain_focus(&receiver);
	}
	
	std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	
	bool isRequestedPositionInsideComponent = !(x < 0 || y < 0 || x > w || y > h);
	bool isMouseInsideComponent = !(this->x < 0 || this->y < 0 || this->x > this->w || this->y > this->h);
	bool isDragging = mouse_buttons[0] || mouse_buttons[1] || mouse_buttons[2];
	
	//Button priority is 1 (left), 3 (right), 2 (middle)
	std::int32_t button = mouse_buttons[0] ? 1 : mouse_buttons[2] ? 3 : mouse_buttons[1] ? 2 : 0;
	std::int32_t buttonMask = (mouse_buttons[0] ? InputEvent::GetDownMaskForButton(mouse_buttons[0]) : 0) |
	                          (mouse_buttons[1] ? InputEvent::GetDownMaskForButton(mouse_buttons[1]) : 0) |
							  (mouse_buttons[2] ? InputEvent::GetDownMaskForButton(mouse_buttons[2]) : 0);

	//Key extended masks
	buttonMask |= GetActiveKeyModifiers();
	
	if (isRequestedPositionInsideComponent && !isMouseInsideComponent)
	{
		//MOUSE_ENTERED
		MouseEvent::Dispatch(env, &receiver, &receiver, MouseEvent::MouseEventCodes::MOUSE_ENTERED, when, buttonMask, x, y, 0, false, 0);
		MouseEvent::Dispatch(env, &receiver, &receiver, MouseEvent::MouseEventCodes::MOUSE_MOVED, when, buttonMask, x, y, 0, false, 0);
	}
	else if (!isRequestedPositionInsideComponent && isMouseInsideComponent)
	{
		//MOUSE_EXITED
		MouseEvent::Dispatch(env, &receiver, &receiver, MouseEvent::MouseEventCodes::MOUSE_MOVED, when, buttonMask, x, y, 0, false, 0);
		MouseEvent::Dispatch(env, &receiver, &receiver, MouseEvent::MouseEventCodes::MOUSE_EXITED, when, buttonMask, x, y, 0, false, 0);
	}
	else if (isRequestedPositionInsideComponent && isMouseInsideComponent)
	{
		//MOUSE_MOVED OR MOUSE_DRAGGED
		if (isDragging)
		{
			MouseEvent::Dispatch(env, &receiver, &receiver, MouseEvent::MouseEventCodes::MOUSE_DRAGGED, when, buttonMask, x, y, click_count, false, button);
		}
		else
		{
			MouseEvent::Dispatch(env, &receiver, &receiver, MouseEvent::MouseEventCodes::MOUSE_MOVED, when, buttonMask, x, y, 0, false, 0);
		}
	}
	else if (!isRequestedPositionInsideComponent && !isMouseInsideComponent)
	{
		//MOUSE_DRAGGED OUTSIDE
		if (isDragging)
		{
			MouseEvent::Dispatch(env, &receiver, &receiver, MouseEvent::MouseEventCodes::MOUSE_DRAGGED, when, buttonMask, x, y, click_count, false, button);
		}
	}
}

void InputOutput::hold_mouse(std::int32_t x, std::int32_t y, std::int32_t button)
{
	if (!this->is_mouse_held(button))
	{
		extern std::unique_ptr<ControlCenter> control_center;
		if (!control_center)
		{
			return;
		}
		
		Component receiver = control_center->reflect_canvas();
		JNIEnv* env = receiver.getEnv();
		
		if (!this->has_focus(&receiver))
		{
			this->gain_focus(&receiver);
		}
		
		std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

		//Button priority is 1 (left), 3 (right), 2 (middle)
		std::int32_t button = mouse_buttons[0] ? 1 : mouse_buttons[2] ? 3 : mouse_buttons[1] ? 2 : 0;
		std::int32_t buttonMask = (mouse_buttons[0] ? InputEvent::GetDownMaskForButton(mouse_buttons[0]) : 0) |
								  (mouse_buttons[1] ? InputEvent::GetDownMaskForButton(mouse_buttons[1]) : 0) |
								  (mouse_buttons[2] ? InputEvent::GetDownMaskForButton(mouse_buttons[2]) : 0);

		//Key extended masks
		buttonMask |= GetActiveKeyModifiers();
		
		mouse_buttons[button - 1] = true;
		MouseEvent::Dispatch(env, &receiver, &receiver, MouseEvent::MouseEventCodes::MOUSE_PRESSED, when, buttonMask, x, y, click_count, false, button);
	}
}

void InputOutput::release_mouse(std::int32_t x, std::int32_t y, std::int32_t button)
{
	if (this->is_mouse_held(button))
	{
		extern std::unique_ptr<ControlCenter> control_center;
		if (!control_center)
		{
			return;
		}
		
		Component receiver = control_center->reflect_canvas();
		JNIEnv* env = receiver.getEnv();
		
		if (!this->has_focus(&receiver))
		{
			this->gain_focus(&receiver);
		}
		
		std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

		bool isRequestedPositionInsideComponent = !(x < 0 || y < 0 || x > w || y > h);
		bool isMouseInsideComponent = !(this->x < 0 || this->y < 0 || this->x > this->w || this->y > this->h);
		
		//Button priority is 1 (left), 3 (right), 2 (middle)
		std::int32_t button = mouse_buttons[0] ? 1 : mouse_buttons[2] ? 3 : mouse_buttons[1] ? 2 : 0;
		std::int32_t buttonMask = (mouse_buttons[0] ? InputEvent::GetDownMaskForButton(mouse_buttons[0]) : 0) |
								  (mouse_buttons[1] ? InputEvent::GetDownMaskForButton(mouse_buttons[1]) : 0) |
								  (mouse_buttons[2] ? InputEvent::GetDownMaskForButton(mouse_buttons[2]) : 0);

		//Key extended masks
		buttonMask |= GetActiveKeyModifiers();
		
		mouse_buttons[button - 1] = false;
		MouseEvent::Dispatch(env, &receiver, &receiver, MouseEvent::MouseEventCodes::MOUSE_RELEASED, when, buttonMask, x, y, click_count, false, button);
		
		if (isRequestedPositionInsideComponent && isMouseInsideComponent)
		{
			MouseEvent::Dispatch(env, &receiver, &receiver, MouseEvent::MouseEventCodes::MOUSE_CLICKED, when, buttonMask, x, y, click_count, false, button);
		}
	}
}

bool InputOutput::is_mouse_held(std::int32_t button)
{
	switch (button)
	{
		case 1: return mouse_buttons[0];
		case 2: return mouse_buttons[1];
		case 3: return mouse_buttons[2];
		default: return false;
	}
}

jchar InputOutput::NativeKeyCodeToChar(std::int32_t keycode, std::int32_t modifiers)
{
	static const std::int16_t mapping[256] = { //char16_t
		0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
		0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
		0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
		0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
		0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
		0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
		0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
		0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
		0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
		0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
		0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
		0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
		0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
		0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
		0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
		0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
		0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
		0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
		0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
		0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
		0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
		0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
		0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
		0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
		0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
		0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
		0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
		0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
		0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
		0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
		0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
		0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
	};
	
	if (modifiers != 0)
	{
		static const std::string normal_chars = "`1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,./";
		static const std::string shift_chars = "~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?";
		if (auto index = normal_chars.find(static_cast<char>(mapping[keycode])); index != std::string::npos)
		{
			return static_cast<jchar>(shift_chars[index]);
		}
	}
	return mapping[keycode];
}

std::int32_t InputOutput::CharToJavaKeyCode(char c)
{
	//Convert char to Java VK-Codes
	//0x00 and 0xFF are not type-able characters..
	static const std::uint32_t mapping[256] = {
		0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
		0x008, 0x009, 0x00D, 0x000, 0x000, 0x000, 0x000, 0x000,
		0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
		0x000, 0x000, 0x000, 0x01B, 0x000, 0x000, 0x000, 0x000,
		0x020, 0x205, 0x098, 0x208, 0x203, 0x000, 0x096, 0x0DE,
		0x207, 0x20A, 0x097, 0x209, 0x02C, 0x02D, 0x02E, 0x02F,
		0x030, 0x031, 0x032, 0x033, 0x034, 0x035, 0x036, 0x037,
		0x038, 0x039, 0x201, 0x03B, 0x099, 0x03D, 0x0A0, 0x000,
		0x200, 0x041, 0x042, 0x043, 0x044, 0x045, 0x046, 0x047,
		0x048, 0x049, 0x04A, 0x04B, 0x04C, 0x04D, 0x04E, 0x04F,
		0x050, 0x051, 0x052, 0x053, 0x054, 0x055, 0x056, 0x057,
		0x058, 0x059, 0x05A, 0x05B, 0x05C, 0x05D, 0x202, 0x20B,
		0x0C0, 0x041, 0x042, 0x043, 0x044, 0x045, 0x046, 0x047,
		0x048, 0x049, 0x04A, 0x04B, 0x04C, 0x04D, 0x04E, 0x04F,
		0x050, 0x051, 0x052, 0x053, 0x054, 0x055, 0x056, 0x057,
		0x058, 0x059, 0x05A, 0x0A1, 0x0FF, 0x0A2, 0x0FF, 0x07F,
		0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
		0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
		0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
		0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
		0x000, 0x206, 0x0FF, 0x0FF, 0x000, 0x0FF, 0x000, 0x0FF,
		0x0FF, 0x000, 0x000, 0x0FF, 0x000, 0x000, 0x000, 0x000,
		0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF,
		0x000, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF,
		0x0FF, 0x0FF, 0x0FF, 0x000, 0x0FF, 0x0FF, 0x0FF, 0x0FF,
		0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x000,
		0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF,
		0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF,
		0x0FF, 0x0FF, 0x0FF, 0x000, 0x0FF, 0x0FF, 0x0FF, 0x0FF,
		0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x000,
		0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF,
		0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x000
	};
	return mapping[c];
}

std::int32_t InputOutput::GetJavaKeyCode(std::int32_t native_key_code)
{
	//Convert WinAPI VK-Codes to Java VK-Codes
	//0x00 and 0xFF are not type-able characters..
	static const std::uint32_t windows_mapping[256] = {
		0x0000, 0x0000, 0x0000, 0x0003, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0008, 0x0009, 0x0000, 0x0000, 0x000C, 0x000D, 0x0000, 0x0000,
		0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0000, 0x0000,
		0x0018, 0x0019, 0x0000, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
		0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
		0x0028, 0x0000, 0x0000, 0x0000, 0x009A, 0x009B, 0x007F, 0x009C,
		0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
		0x0038, 0x0039, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
		0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
		0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
		0x0058, 0x0059, 0x005A, 0x020C, 0x020C, 0x020D, 0x0000, 0x0000,
		0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
		0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
		0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
		0x0078, 0x0079, 0x007A, 0x007B, 0xF000, 0xF001, 0xF002, 0xF003,
		0xF004, 0xF005, 0xF006, 0xF007, 0xF008, 0xF009, 0xF00A, 0xF00B,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0090, 0x0091, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0010, 0x0010, 0x0011, 0x0011, 0x0012, 0x0012, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x003B, 0x0209, 0x002C, 0x002D, 0x002E, 0x002F,
		0x0083, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x005B, 0x005C, 0x005D, 0x0098, 0x0000,
		0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	};
	return windows_mapping[native_key_code];
}

std::int32_t InputOutput::GetKeyLocation(std::int32_t keycode)
{
	if (std::find(std::begin(control_keys), std::end(control_keys), keycode) != std::end(control_keys))
	{
		return control_keys_locations[keycode];
	}
	return KeyEvent::KeyCodes::KEY_LOCATION_STANDARD;
}

std::int32_t InputOutput::GetActiveKeyModifiers()
{
	std::int32_t modifiers = 0;
	
	//Key extended masks
	if (this->any_key_held({VK_SHIFT, VK_LSHIFT, VK_RSHIFT}))
	{
		modifiers |= InputEvent::InputEventMasks::SHIFT_DOWN_MASK;
	}
	
	if (this->any_key_held({VK_CONTROL, VK_LEFT_CONTROL, VK_RIGHT_CONTROL}))
	{
		modifiers |= InputEvent::InputEventMasks::CTRL_DOWN_MASK;
	}
	
	if (this->is_key_held(VK_LEFT_ALT))
	{
		modifiers |= InputEvent::InputEventMasks::ALT_DOWN_MASK;
	}
	
	if (this->is_key_held(VK_RIGHT_ALT))
	{
		modifiers |= InputEvent::InputEventMasks::ALT_GRAPH_DOWN_MASK;
	}
	
	if (this->any_key_held({VK_LEFT_WINDOWS, VK_RIGHT_WINDOWS, VK_COMMAND}))
	{
		modifiers |= InputEvent::InputEventMasks::META_DOWN_MASK;
	}
	return modifiers;
}

std::int32_t InputOutput::ModifiersForChar(char c)
{
	std::int32_t modifiers = 0;
	static const std::string shift_chars = "~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?";
	if (shift_chars.find(c) != std::string::npos)
	{
		modifiers |= InputEvent::InputEventMasks::SHIFT_DOWN_MASK;
	}
	return 0;
}
