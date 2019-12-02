//
//  ReflectionHook.hxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-01.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef ReflectionHook_HXX_INCLUDED
#define ReflectionHook_HXX_INCLUDED

#include <string>
#include <jni.h>

struct ReflectionHook
{
	jobject object;
    std::string cls;
    std::string field;
    std::string desc;
    
    ReflectionHook(jobject object = nullptr, std::string cls = "", std::string field = "", std::string desc = "") : object(object), cls(cls), field(field), desc(desc) {}
	
	void read(void* input)
	{
		std::uint8_t* ptr = static_cast<std::uint8_t*>(input);
		object = *reinterpret_cast<jobject*>(ptr);
		ptr += sizeof(jobject);
		
		read_string(ptr, cls);
		read_string(ptr, field);
		read_string(ptr, desc);
	}
	
	void write(void* output) const
	{
		std::uint8_t* ptr = static_cast<std::uint8_t*>(output);
		*reinterpret_cast<jobject*>(ptr) = object;
		ptr += sizeof(jobject);
		
		write_string(ptr, cls);
		write_string(ptr, field);
		write_string(ptr, desc);
	}
	
private:
	void read_string(std::uint8_t* &input, std::string &string)
	{
		std::size_t length = *reinterpret_cast<std::size_t*>(input);
		input += sizeof(std::size_t);
		
		string = std::string(reinterpret_cast<const char*>(input), length);
		input += length * sizeof(char);
		++input;
	}
	
	void write_string(std::uint8_t* &output, const std::string &string) const
	{
		*reinterpret_cast<std::size_t*>(output) = string.length();
		output += sizeof(std::size_t);
		
		memcpy(output, &string[0], string.length() * sizeof(char));
		output += string.length() * sizeof(char);
		*output++ = '\0';
	}
};

#endif /* ReflectionHook_HXX_INCLUDED */
