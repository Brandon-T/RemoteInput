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

#include "JNI_Common.hxx"

struct ReflectionHook
{
	jobject object;
    std::string cls;
    std::string field;
    std::string desc;
    
    ReflectionHook(jobject object = nullptr, std::string cls = "", std::string field = "", std::string desc = "") noexcept;
	
	void read(void* input) noexcept;
	void write(void* output) const noexcept;
	
private:
	void read_string(std::uint8_t* &ptr, std::string &string) const noexcept;
	void write_string(std::uint8_t* &ptr, const std::string &string) const noexcept;
};

#endif /* ReflectionHook_HXX_INCLUDED */
