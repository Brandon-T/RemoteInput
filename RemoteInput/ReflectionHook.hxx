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
#include "Stream.hxx"

struct ReflectionHook
{
    jobject object;
    std::string cls;
    std::string field;
    std::string desc;
    
    ReflectionHook(jobject object = nullptr, std::string cls = "", std::string field = "", std::string desc = "") noexcept;
};

Stream& operator << (Stream& stream, const ReflectionHook &hook);
Stream& operator >> (Stream& stream, ReflectionHook &hook);

template<>
struct std::hash<ReflectionHook>
{
    std::size_t operator()(const ReflectionHook& hook) const noexcept
    {
        std::size_t cls_hash = std::hash<std::string>()(hook.cls);
        std::size_t field_hash = std::hash<std::string>()(hook.field);
        std::size_t desc_hash = std::hash<std::string>()(hook.desc);

        std::size_t hashes[] = {cls_hash, field_hash, desc_hash};

        std::size_t hash_one = (5381 << 16) + 5381;
        std::size_t hash_two = hash_one;

        for(std::size_t i = 0; i < sizeof(hashes) / sizeof(std::size_t); ++i)
        {
            if (i % 2 == 0)
            {
                hash_one = ((hash_one << 5) + hash_one + (hash_one >> 27)) ^ hashes[i];
            }
            else
            {
                hash_two = ((hash_two << 5) + hash_two + (hash_two >> 27)) ^ hashes[i];
            }
        }

        return hash_one + (hash_two * 1566083941);
    }
};

#endif /* ReflectionHook_HXX_INCLUDED */
