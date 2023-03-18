//
// Created by Brandon on 2023-03-25.
//

#ifndef REMOTEINPUT_JVMCACHE_HXX
#define REMOTEINPUT_JVMCACHE_HXX

#include <unordered_map>
#include <utility>
#include <memory>
#include <functional>
#include "ReflectionHook.hxx"
#include "JVM.hxx"

class JVMCache
{
public:
    JVMCache(JVM* jvm, jobject class_loader);
    JVMCache(const JVMCache&) = delete;
    JVMCache(JVMCache&&);
    ~JVMCache();

    JVMCache& operator = (const JVMCache&) = delete;
    JVMCache& operator = (JVMCache&&);

    jfieldID GetFieldID(jclass clazz, const char* name, const char* sig, bool is_static) noexcept;
    jclass GetClass(const char* name) noexcept;

    void clear();

private:
    JVM* jvm;
    jobject class_loader;
    jmethodID load_class_method;

    std::unordered_map<jclass, std::unordered_map<std::size_t, jfieldID>> field_cache;
    std::unordered_map<std::string, std::unique_ptr<typename std::remove_pointer<jclass>::type, std::function<void(jclass)>>> class_cache;

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

    static std::size_t field_hash(const char* field_name, const char* signature) noexcept;
};


#endif //REMOTEINPUT_JVMCACHE_HXX
