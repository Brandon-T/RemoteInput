//
// Created by Brandon on 2023-03-25.
//

#include "JVMCache.hxx"

JVMCache::JVMCache(JVM* jvm, jobject class_loader) : jvm(jvm), class_loader(class_loader), load_class_method(nullptr)
{
    if (class_loader)
    {
        auto class_loader_class = make_safe_local<jclass>(jvm->GetObjectClass(class_loader));
        load_class_method = jvm->GetMethodID(class_loader_class.get(), "loadClass", "(Ljava/lang/String;Z)Ljava/lang/Class;");
    }
}

JVMCache::~JVMCache()
{
}

JVMCache::JVMCache(JVMCache&& other) : jvm(other.jvm), class_loader(other.class_loader), load_class_method(other.load_class_method), field_cache(std::move(other.field_cache)), class_cache(std::move(other.class_cache))
{
    other.jvm = nullptr;
    other.class_loader = nullptr;
    other.load_class_method = nullptr;
    other.field_cache.clear();
    other.class_cache.clear();
}

JVMCache& JVMCache::operator = (JVMCache&& other)
{
    jvm = other.jvm;
    class_loader = other.class_loader;
    load_class_method = other.load_class_method;
    field_cache = std::move(other.field_cache);
    class_cache = std::move(other.class_cache);

    other.jvm = nullptr;
    other.class_loader = nullptr;
    other.load_class_method = nullptr;
    other.field_cache.clear();
    other.class_cache.clear();
    return *this;
}

jfieldID JVMCache::GetFieldID(jclass clazz, const char* name, const char* sig, bool is_static) noexcept
{
    std::size_t key = field_hash(name, sig);
    if (auto it = field_cache.find(clazz); it != field_cache.end())
    {
        if (auto jt = it->second.find(key); jt != it->second.end())
        {
            return jt->second;
        }
    }

    jfieldID field = is_static ? jvm->GetStaticFieldID(clazz, name, sig) : jvm->GetFieldID(clazz, name, sig);
    if (field)
    {
        field_cache[clazz][key] = field;
        return field;
    }
    return nullptr;
}

jclass JVMCache::GetClass(const char* name) noexcept
{
    if (auto it = class_cache.find(name); it != class_cache.end())
    {
        return it->second.get();
    }
    else
    {
        auto class_name = make_safe_local<jstring>(jvm->NewStringUTF(name));
        auto clazz = make_safe_global<jclass>(jvm->CallObjectMethod(class_loader, load_class_method, class_name.get(), true));
        if (clazz)
        {
            jclass result = clazz.get();
            class_cache[name] = std::move(clazz);
            return result;
        }
        return nullptr;
    }
}

void JVMCache::clear()
{
    field_cache.clear();
    class_cache.clear();
}

std::size_t JVMCache::field_hash(const char* field_name, const char* signature) noexcept
{
    std::size_t field_hash = std::hash<std::string>()(field_name);
    std::size_t desc_hash = std::hash<std::string>()(signature);

    std::size_t hashes[] = {field_hash, desc_hash};

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