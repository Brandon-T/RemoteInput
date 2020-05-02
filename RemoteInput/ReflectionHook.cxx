//
//  ReflectionHook.cxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-01.
//  Copyright © 2019 XIO. All rights reserved.
//

#include "ReflectionHook.hxx"

ReflectionHook::ReflectionHook(jobject object, std::string cls, std::string field, std::string desc) noexcept : object(object), cls(cls), field(field), desc(desc)
{}

void ReflectionHook::read(void* input) noexcept
{
    std::uint8_t* ptr = static_cast<std::uint8_t*>(input);
    std::memcpy(&object, ptr, sizeof(jobject));
    ptr += sizeof(jobject);

    read_string(ptr, cls);
    read_string(ptr, field);
    read_string(ptr, desc);
}

void ReflectionHook::write(void* output) const noexcept
{
    std::uint8_t* ptr = static_cast<std::uint8_t*>(output);
    memcpy(ptr, &object, sizeof(jobject));
    ptr += sizeof(jobject);

    write_string(ptr, cls);
    write_string(ptr, field);
    write_string(ptr, desc);
}

void ReflectionHook::read_string(std::uint8_t* &ptr, std::string &string) const noexcept
{
    std::size_t length = 0;
    memcpy(&length, ptr, sizeof(length));
    ptr += sizeof(length);

    string.resize(length, '\0');
    memcpy(&string[0], ptr, length * sizeof(char));
    ptr += length * sizeof(char);
}

void ReflectionHook::write_string(std::uint8_t* &ptr, const std::string &string) const noexcept
{
    std::size_t length = string.length();
    memcpy(ptr, &length, sizeof(length));
    ptr += sizeof(length);

    memcpy(ptr, &string[0], length * sizeof(char));
    ptr += length * sizeof(char);
}