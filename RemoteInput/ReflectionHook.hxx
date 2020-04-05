//
//  ReflectionHook.hxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-01.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef ReflectionHook_HXX_INCLUDED
#define ReflectionHook_HXX_INCLUDED

#include <cstring>
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
		std::memcpy(&object, ptr, sizeof(jobject));
		ptr += sizeof(jobject);
		
		read_string(ptr, cls);
		read_string(ptr, field);
		read_string(ptr, desc);
	}
	
	void write(void* output) const
	{
		std::uint8_t* ptr = static_cast<std::uint8_t*>(output);
		memcpy(ptr, &object, sizeof(jobject));
		ptr += sizeof(jobject);
		
		write_string(ptr, cls);
		write_string(ptr, field);
		write_string(ptr, desc);
	}
	
private:
	void read_string(std::uint8_t* &ptr, std::string &string)
	{
		std::size_t length = 0;
		memcpy(&length, ptr, sizeof(length));
		ptr += sizeof(length);
		
		string.resize(length, '\0');
		memcpy(&string[0], ptr, length * sizeof(char));
		ptr += length * sizeof(char);
	}
	
	void write_string(std::uint8_t* &ptr, const std::string &string) const
	{
		std::size_t length = string.length();
		memcpy(ptr, &length, sizeof(length));
		ptr += sizeof(length);
		
		memcpy(ptr, &string[0], length * sizeof(char));
		ptr += length * sizeof(char);
	}
};

#endif /* ReflectionHook_HXX_INCLUDED */
