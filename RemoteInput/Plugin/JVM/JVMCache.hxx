//
// Created by Brandon on 2023-03-25.
//

#ifndef REMOTEINPUT_JVMCACHE_HXX
#define REMOTEINPUT_JVMCACHE_HXX

#include <unordered_map>
#include <utility>
#include <memory>
#include <functional>
#include <string_view>
#include "ReflectionHook.hxx"
#include "JVM.hxx"
#include "TypeTraits.hxx"

class JVMCache
{
public:
    JVMCache(JVM* jvm, jobject class_loader);
    JVMCache(const JVMCache&) = delete;
    JVMCache(JVMCache&&);
    ~JVMCache();

    JVMCache& operator = (const JVMCache&) = delete;
    JVMCache& operator = (JVMCache&&);

    jclass GetClass(std::string_view name) noexcept;
    jfieldID GetFieldID(jclass clazz, std::string_view name, std::string_view sig, bool is_static) noexcept;

    void clear();

private:
    JVM* jvm;
    jobject class_loader;
    jmethodID load_class_method;

    std::unordered_map<std::string, std::unique_ptr<typename std::remove_pointer<jclass>::type, std::function<void(jclass)>>, string_hash, std::equal_to<>> class_cache;
    std::unordered_map<std::size_t, jfieldID> field_cache;

    template<typename T>
    inline auto make_safe_local(auto object) const noexcept
    {
        auto deleter = [&](T object) noexcept {
            jvm->DeleteLocalRef(static_cast<jobject>(object));
        };

        return std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>{static_cast<T>(object), deleter};
    }

    template<typename T>
    inline auto make_safe_global(auto object) const noexcept
    {
        auto deleter = [&](T object) noexcept {
            jvm->DeleteGlobalRef(static_cast<jobject>(object));
        };

        jvm->DeleteLocalRef(std::exchange(object, jvm->NewGlobalRef(static_cast<jobject>(object))));
        return std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>{static_cast<T>(object), deleter};
    }

    static std::size_t field_hash(jclass clazz, std::string_view field_name, std::string_view signature) noexcept;
};


#endif //REMOTEINPUT_JVMCACHE_HXX
