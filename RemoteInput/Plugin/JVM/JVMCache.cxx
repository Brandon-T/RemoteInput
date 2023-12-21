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
    other.class_cache.clear();
    other.field_cache.clear();
}

JVMCache& JVMCache::operator = (JVMCache&& other)
{
    jvm = other.jvm;
    class_loader = other.class_loader;
    load_class_method = other.load_class_method;
    class_cache = std::move(other.class_cache);
    field_cache = std::move(other.field_cache);

    other.jvm = nullptr;
    other.class_loader = nullptr;
    other.load_class_method = nullptr;
    other.class_cache.clear();
    other.field_cache.clear();
    return *this;
}

jclass JVMCache::GetClass(std::string_view name) noexcept
{
    if (auto it = class_cache.find(name); it != class_cache.end())
    {
        return it->second.get();
    }
    else
    {
        jstring class_name = jvm->NewStringUTF(name.data());
        if (class_name)
        {
            jclass clazz = static_cast<jclass>(jvm->CallObjectMethod(class_loader, load_class_method, class_name, true));
            jvm->DeleteLocalRef(class_name);

            if (clazz)
            {
                jvm->DeleteLocalRef(std::exchange(clazz, static_cast<jclass>(jvm->NewGlobalRef(clazz))));
                class_cache.emplace(name, clazz);
                return clazz;
            }
        }

        return nullptr;
    }
}

jfieldID JVMCache::GetFieldID(jclass clazz, std::string_view name, std::string_view sig, bool is_static) noexcept
{
    std::size_t key = field_hash(clazz, name, sig);
    if (auto it = field_cache.find(key); it != field_cache.end())
    {
        return it->second;
    }

    jfieldID field = is_static ? jvm->GetStaticFieldID(clazz, name.data(), sig.data()) : jvm->GetFieldID(clazz, name.data(), sig.data());
    if (field)
    {
        field_cache[key] = field;
        return field;
    }
    return nullptr;
}

void JVMCache::clear()
{
    class_cache.clear();
    field_cache.clear();
    for (auto &item: class_cache)
    {
        item.second.release();
    }
}

std::size_t JVMCache::field_hash(jclass clazz, std::string_view field_name, std::string_view signature) noexcept
{
    std::size_t hash = 0;
    hash_combine(hash, clazz);
    hash_combine(hash, field_name);
    hash_combine(hash, signature);
    return hash;
}

/*std::size_t JVMCache::field_hash(const char* field_name, const char* signature) noexcept
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
}*/