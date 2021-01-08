//
// Created by Brandon on 2020-12-08.
//

#include "RemoteVM.hxx"
#include "EIOS.hxx"

enum class RemoteVMCommand: std::uint32_t
{
    ALLOCATE_MEMORY,
    READ_MEMORY,
    WRITE_MEMORY,
    FREE_MEMORY,

    GET_VERSION,
    DEFINE_CLASS,
    FIND_CLASS,
    FROM_REFLECTED_METHOD,
    FROM_REFLECTED_FIELD,
    TO_REFLECTED_METHOD,
    GET_SUPER_CLASS,
    IS_ASSIGNABLE_FROM,
    TO_REFLECTED_FIELD,
    THROW,
    THROW_NEW,
    GET_EXCEPTION_MESSAGE,
    FATAL_ERROR,
    DELETE_GLOBAL_REF,
    IS_SAME_OBJECT,
    ALLOC_OBJECT,
    NEW_OBJECT,
    GET_OBJECT_CLASS,
    IS_INSTANCE_OF,
    GET_METHOD_ID,
    CALL_OBJECT_METHOD,
    CALL_BOOLEAN_METHOD,
    CALL_BYTE_METHOD,
    CALL_CHAR_METHOD,
    CALL_SHORT_METHOD,
    CALL_INT_METHOD,
    CALL_LONG_METHOD,
    CALL_FLOAT_METHOD,
    CALL_DOUBLE_METHOD,
    CALL_VOID_METHOD,
    CALL_NON_VIRTUAL_OBJECT_METHOD,
    CALL_NON_VIRTUAL_BOOLEAN_METHOD,
    CALL_NON_VIRTUAL_BYTE_METHOD,
    CALL_NON_VIRTUAL_CHAR_METHOD,
    CALL_NON_VIRTUAL_SHORT_METHOD,
    CALL_NON_VIRTUAL_INT_METHOD,
    CALL_NON_VIRTUAL_LONG_METHOD,
    CALL_NON_VIRTUAL_FLOAT_METHOD,
    CALL_NON_VIRTUAL_DOUBLE_METHOD,
    CALL_NON_VIRTUAL_VOID_METHOD,
    GET_FIELD_ID,
    GET_OBJECT_FIELD,
    GET_BOOLEAN_FIELD,
    GET_BYTE_FIELD,
    GET_CHAR_FIELD,
    GET_SHORT_FIELD,
    GET_INT_FIELD,
    GET_LONG_FIELD,
    GET_FLOAT_FIELD,
    GET_DOUBLE_FIELD,
    SET_OBJECT_FIELD,
    SET_BOOLEAN_FIELD,
    SET_BYTE_FIELD,
    SET_CHAR_FIELD,
    SET_SHORT_FIELD,
    SET_INT_FIELD,
    SET_LONG_FIELD,
    SET_FLOAT_FIELD,
    SET_DOUBLE_FIELD,
    GET_STATIC_METHOD_ID,
    CALL_STATIC_OBJECT_METHOD,
    CALL_STATIC_BOOLEAN_METHOD,
    CALL_STATIC_BYTE_METHOD,
    CALL_STATIC_CHAR_METHOD,
    CALL_STATIC_SHORT_METHOD,
    CALL_STATIC_INT_METHOD,
    CALL_STATIC_LONG_METHOD,
    CALL_STATIC_FLOAT_METHOD,
    CALL_STATIC_DOUBLE_METHOD,
    CALL_STATIC_VOID_METHOD,
    GET_STATIC_FIELD_ID,
    GET_STATIC_OBJECT_FIELD,
    GET_STATIC_BOOLEAN_FIELD,
    GET_STATIC_BYTE_FIELD,
    GET_STATIC_CHAR_FIELD,
    GET_STATIC_SHORT_FIELD,
    GET_STATIC_INT_FIELD,
    GET_STATIC_LONG_FIELD,
    GET_STATIC_FLOAT_FIELD,
    GET_STATIC_DOUBLE_FIELD,
    SET_STATIC_OBJECT_FIELD,
    SET_STATIC_BOOLEAN_FIELD,
    SET_STATIC_BYTE_FIELD,
    SET_STATIC_CHAR_FIELD,
    SET_STATIC_SHORT_FIELD,
    SET_STATIC_INT_FIELD,
    SET_STATIC_LONG_FIELD,
    SET_STATIC_FLOAT_FIELD,
    SET_STATIC_DOUBLE_FIELD,
    NEW_STRING,
    GET_STRING_LENGTH,
    GET_STRING_CHARS,
    NEW_STRING_UTF,
    GET_STRING_UTF_LENGTH,
    GET_STRING_UTF_CHARS,
    GET_ARRAY_LENGTH,
    NEW_OBJECT_ARRAY,
    GET_OBJECT_ARRAY_ELEMENTS,
    SET_OBJECT_ARRAY_ELEMENTS,
    NEW_BOOLEAN_ARRAY,
    NEW_BYTE_ARRAY,
    NEW_CHAR_ARRAY,
    NEW_SHORT_ARRAY,
    NEW_INT_ARRAY,
    NEW_LONG_ARRAY,
    NEW_FLOAT_ARRAY,
    NEW_DOUBLE_ARRAY,
    GET_BOOLEAN_ARRAY_ELEMENTS,
    GET_BYTE_ARRAY_ELEMENTS,
    GET_CHAR_ARRAY_ELEMENTS,
    GET_SHORT_ARRAY_ELEMENTS,
    GET_INT_ARRAY_ELEMENTS,
    GET_LONG_ARRAY_ELEMENTS,
    GET_FLOAT_ARRAY_ELEMENTS,
    GET_DOUBLE_ARRAY_ELEMENTS,
    GET_BOOLEAN_ARRAY_REGION,
    GET_BYTE_ARRAY_REGION,
    GET_CHAR_ARRAY_REGION,
    GET_SHORT_ARRAY_REGION,
    GET_INT_ARRAY_REGION,
    GET_LONG_ARRAY_REGION,
    GET_FLOAT_ARRAY_REGION,
    GET_DOUBLE_ARRAY_REGION,
    SET_BOOLEAN_ARRAY_REGION,
    SET_BYTE_ARRAY_REGION,
    SET_CHAR_ARRAY_REGION,
    SET_SHORT_ARRAY_REGION,
    SET_INT_ARRAY_REGION,
    SET_LONG_ARRAY_REGION,
    SET_FLOAT_ARRAY_REGION,
    SET_DOUBLE_ARRAY_REGION,
    MONITOR_ENTER,
    MONITOR_EXIT,
    GET_JAVA_VM,
    NEW_DIRECT_BYTE_BUFFER,
    GET_DIRECT_BUFFER_ADDRESS,
    GET_DIRECT_BUFFER_CAPACITY,
    GET_OBJECT_REF_TYPE
};

template<class T, class... Ts>
struct is_same_of : std::bool_constant<(std::is_same<typename std::remove_cv<T>::type, typename std::remove_cv<Ts>::type>::value || ...)> { };

template<class T>
struct is_string
{
    static bool const value = false;
};

template<typename T>
struct is_string<std::basic_string<T>>
{
    static bool const value = true;
};

template<typename T>
typename std::enable_if<!is_string<std::decay_t<T>>::value && !is_vector<std::decay_t<T>>::value, std::decay_t<T>>::type Remote_VM_Read(void* &ptr) noexcept
{
    T result = *static_cast<T*>(ptr);
    ptr = static_cast<T*>(ptr) + 1;
    return result;
}

template<typename T>
typename std::enable_if<is_string<std::decay_t<T>>::value, std::decay_t<T>>::type Remote_VM_Read(void* &ptr) noexcept
{
    using string_type = std::decay_t<T>;
    using value_type = typename std::decay_t<T>::value_type;

    std::size_t length = *static_cast<std::size_t*>(ptr);
    ptr = static_cast<std::size_t*>(ptr) + 1;

    if (length > 0)
    {
        string_type result = string_type(reinterpret_cast<const value_type*>(ptr), length);
        ptr = static_cast<value_type*>(ptr) + (length * sizeof(value_type));
        ptr = static_cast<value_type*>(ptr) + 1;
        return result;
    }
    return string_type();
}

template<typename T>
typename std::enable_if<is_vector<std::decay_t<T>>::value, std::decay_t<T>>::type Remote_VM_Read(void* &ptr) noexcept
{
    using vector_type = std::decay_t<T>;
    using value_type = typename std::decay_t<T>::value_type;

    std::size_t size = *static_cast<std::size_t*>(ptr);
    ptr = static_cast<std::size_t*>(ptr) + 1;

    if (size > 0)
    {
        vector_type result = vector_type(size);
        std::memcpy(&result[0], ptr, size * sizeof(value_type));
        ptr = static_cast<char*>(ptr) + (size * sizeof(value_type));
        return result;
    }
    return vector_type();
}

template<typename T>
void Remote_VM_Write(void* &ptr, const T& result) noexcept
{
    *static_cast<T*>(ptr) = result;
    ptr = static_cast<T*>(ptr) + 1;
}

template<typename T>
void Remote_VM_Write(void* &ptr, const std::basic_string<T> &result) noexcept
{
    if (result.empty())
    {
        *static_cast<std::size_t*>(ptr) = 0;
        ptr = static_cast<std::size_t*>(ptr) + 1;
        return;
    }

    *static_cast<std::size_t*>(ptr) = result.length();
    ptr = static_cast<std::size_t*>(ptr) + 1;

    memcpy(ptr, &result[0], result.length() * sizeof(T));
    ptr = static_cast<T*>(ptr) + (result.length() * sizeof(T));

    *static_cast<T*>(ptr) = '\0';
    ptr = static_cast<T*>(ptr) + 1;
}

template<typename T>
void Remote_VM_Write(void* &ptr, const std::vector<T> &result) noexcept
{
    if (result.empty())
    {
        *static_cast<std::size_t*>(ptr) = 0;
        ptr = static_cast<std::size_t*>(ptr) + 1;
        return;
    }

    *static_cast<std::size_t*>(ptr) = result.size();
    ptr = static_cast<std::size_t*>(ptr) + 1;

    memcpy(ptr, &result[0], result.size() * sizeof(T));
    ptr = static_cast<char*>(ptr) + (result.size() * sizeof(T));
}

template<typename T>
T RemoteVM::local_to_global(T object) const noexcept
{
    if (object)
    {
        T result = static_cast<T>(env->NewGlobalRef(object));
        env->DeleteLocalRef(object);
        return result;
    }
    return nullptr;
}

template<typename R, typename... Args>
typename std::enable_if<!is_vector<R>::value, R>::type RemoteVM::SendCommand(RemoteVMCommand command, Args&&... args) const noexcept
{
    auto result = (control_center->*send_command)([&](ImageData* image_data) {
        void* arguments = image_data->args;
        image_data->command = EIOSCommand::REMOTE_VM_INSTRUCTION;
        Remote_VM_Write(arguments, command);
        (Remote_VM_Write(arguments, args), ...);

        /*([](auto& arguments, auto& arg) {
            Remote_VM_Write(arguments, arg);
        } (arguments, args), ...); //std::forward<void*&>(arguments);*/
    });

    if constexpr(std::is_same<R, void>::value)
    {
        return;
    }
    else if (result)
    {
        void* arguments = (control_center->*get_image_data)()->args;
        return Remote_VM_Read<R>(arguments);
    }
    else if constexpr(std::is_same<R, jboolean>::value)
    {
        return false;
    }
    else if constexpr(is_same_of<R, jfloat, jdouble>::value)
    {
        return 0.0;
    }
    else if constexpr(is_same_of<R, jchar, jbyte, jshort, jint, jlong, jsize>::value)
    {
        return -1;
    }
    else if constexpr (std::is_same<R, jobjectRefType>::value)
    {
        void* arguments = (control_center->*get_image_data)()->args;
        return Remote_VM_Read<R>(arguments);
    }
    else
    {
        return nullptr;
    }
}

template<typename R, typename... Args>
typename std::enable_if<is_vector<R>::value, R>::type RemoteVM::SendCommand(RemoteVMCommand command, Args&&... args) const noexcept
{
    auto result = (control_center->*send_command)([&](ImageData* image_data) {
        void* arguments = image_data->args;
        image_data->command = EIOSCommand::REMOTE_VM_INSTRUCTION;
        Remote_VM_Write(arguments, command);
        (Remote_VM_Write(arguments, args), ...);

        /*([](auto& arguments, auto& arg) {
            Remote_VM_Write(arguments, arg);
        } (arguments, args), ...); //std::forward<void*&>(arguments);*/
    });

    if (result)
    {
        void* arguments = (control_center->*get_image_data)()->args;
        return Remote_VM_Read<R>(arguments);
    }
    return {};
}

template<typename R, typename... Args>
R RemoteVM::ExecuteCommand(void* arguments, R (RemoteVM::*func)(Args...) const noexcept) const noexcept
{
    auto args = std::tuple<std::decay_t<Args>...>{Remote_VM_Read<std::decay_t<Args>>(arguments)...};
    return std::apply(func, std::tuple_cat(std::forward_as_tuple(this), std::forward<std::tuple<std::decay_t<Args>...>>(args)));
}


RemoteVM::RemoteVM(JNIEnv* env,
                   ControlCenter* control_center,
                   bool (ControlCenter::*send_command)(std::function<void(ImageData*)>&&) const,
                   ImageData* (ControlCenter::*get_image_data)() const ) noexcept : env(env), control_center(control_center), send_command(send_command), get_image_data(get_image_data) {}
RemoteVM::~RemoteVM() {}

RemoteVM::RemoteVM(RemoteVM&& other) : env(other.env), control_center(other.control_center)
{
    other.env = nullptr;
    other.control_center = nullptr;
}

RemoteVM& RemoteVM::operator = (RemoteVM&& other)
{
    env = other.env;
    control_center = other.control_center;
    other.env = nullptr;
    other.control_center = nullptr;
    return *this;
}

std::size_t RemoteVM::MaxMemoryChunkSize() const noexcept
{
    std::size_t max_size = sizeof(ImageData::args);
    max_size -= sizeof(std::underlying_type<RemoteVMCommand>);
    max_size -= sizeof(void*);
    max_size -= sizeof(std::size_t);
    return max_size;
}

void* RemoteVM::AllocateMemory(std::size_t size) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void*>(RemoteVMCommand::ALLOCATE_MEMORY, size);
    }
    return malloc(size);
}

bool RemoteVM::ReadMemory(void* destintation, void* source, std::size_t size) const noexcept
{
    if (this->send_command)
    {
        auto result = (control_center->*send_command)([&](ImageData *image_data) {
            void* arguments = image_data->args;
            image_data->command = EIOSCommand::REMOTE_VM_INSTRUCTION;
            Remote_VM_Write(arguments, RemoteVMCommand::READ_MEMORY);
            Remote_VM_Write(arguments, source);
            Remote_VM_Write(arguments, size);
        });

        if (result)
        {
            void* arguments = (control_center->*get_image_data)()->args;
            std::memcpy(destintation, arguments, size);
            return true;
        }
        return false;
    }

    std::memcpy(destintation, source, size);
    return true;
}

bool RemoteVM::WriteMemory(void* destination, void* source, std::size_t size) const noexcept
{
    if (this->send_command)
    {
        auto result = (control_center->*send_command)([&](ImageData *image_data) {
            void* arguments = image_data->args;
            image_data->command = EIOSCommand::REMOTE_VM_INSTRUCTION;
            Remote_VM_Write(arguments, RemoteVMCommand::WRITE_MEMORY);
            Remote_VM_Write(arguments, destination);
            Remote_VM_Write(arguments, size);
            std::memcpy(arguments, source, size);
        });

        return result;
    }

    std::memcpy(destination, source, size);
    return true;
}

void RemoteVM::FreeMemory(void* memory) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::FREE_MEMORY, memory);
    }

    free(memory);
}

jint RemoteVM::GetVersion() const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jint>(RemoteVMCommand::GET_VERSION);
    }

    return env->GetVersion();
}

jclass RemoteVM::DefineClass(const std::string &name, jobject loader, const jbyte* buf, jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jclass>(RemoteVMCommand::DEFINE_CLASS, name, loader, buf, len);
    }

    return local_to_global(env->DefineClass(name.c_str(), loader, buf, len));
}

jclass RemoteVM::FindClass(const std::string &name) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jclass>(RemoteVMCommand::FIND_CLASS, name);
    }
    return local_to_global(env->FindClass(name.c_str()));
}

jmethodID RemoteVM::FromReflectedMethod(jobject method) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jmethodID>(RemoteVMCommand::FROM_REFLECTED_METHOD, method);
    }
    return env->FromReflectedMethod(method);
}

jfieldID RemoteVM::FromReflectedField(jobject field) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jfieldID>(RemoteVMCommand::FROM_REFLECTED_FIELD, field);
    }
    return env->FromReflectedField(field);
}

jobject RemoteVM::ToReflectedMethod(jclass cls, jmethodID methodID, jboolean isStatic) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jobject>(RemoteVMCommand::TO_REFLECTED_METHOD, cls, methodID, isStatic);
    }
    return local_to_global(env->ToReflectedMethod(cls, methodID, isStatic));
}

jclass RemoteVM::GetSuperclass(jclass sub) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jclass>(RemoteVMCommand::GET_SUPER_CLASS, sub);
    }
    return local_to_global(env->GetSuperclass(sub));
}

jboolean RemoteVM::IsAssignableFrom(jclass sub, jclass sup) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jboolean>(RemoteVMCommand::IS_ASSIGNABLE_FROM, sub, sup);
    }
    return env->IsAssignableFrom(sub, sup);
}

jobject RemoteVM::ToReflectedField(jclass cls, jfieldID fieldID, jboolean isStatic) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jclass>(RemoteVMCommand::TO_REFLECTED_FIELD, cls, fieldID, isStatic);
    }
    return local_to_global(env->ToReflectedField(cls, fieldID, isStatic));
}

jint RemoteVM::Throw(jthrowable obj) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jint>(RemoteVMCommand::THROW, obj);
    }
    return env->Throw(obj);
}

jint RemoteVM::ThrowNew(jclass clazz, const std::string &msg) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jint>(RemoteVMCommand::THROW_NEW, clazz, std::string(msg));
    }
    return env->ThrowNew(clazz, msg.c_str());
}

std::string RemoteVM::GetExceptionMessage() const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::string>(RemoteVMCommand::GET_EXCEPTION_MESSAGE);
    }

    std::string result = std::string();
    if (env->ExceptionCheck())
    {
        jthrowable throwable = env->ExceptionOccurred();
        if (throwable)
        {
            env->ExceptionClear();
            jclass clazz = env->GetObjectClass(throwable);
            if (clazz)
            {
                jmethodID getMessage = env->GetMethodID(clazz, "getMessage", "()Ljava/lang/String;");
                jstring message = static_cast<jstring>(env->CallObjectMethod(throwable, getMessage));
                if (message)
                {
                    jsize length = env->GetStringLength(message);
                    if (length > 0)
                    {
                        const char* str = env->GetStringUTFChars(message, nullptr);
                        result = std::string(str, length);
                        env->ReleaseStringUTFChars(message, str);
                    }
                    env->DeleteLocalRef(message);
                }
                env->DeleteLocalRef(clazz);
            }
            env->DeleteLocalRef(throwable);
        }
    }
    return result;
}

void RemoteVM::FatalError(const std::string &msg) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::FATAL_ERROR, msg);
    }
    return env->FatalError(msg.c_str());
}

void RemoteVM::DeleteGlobalRef(jobject gref) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::DELETE_GLOBAL_REF, gref);
    }
    return env->DeleteGlobalRef(gref);
}

jboolean RemoteVM::IsSameObject(jobject obj1, jobject obj2) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jint>(RemoteVMCommand::IS_SAME_OBJECT, obj1, obj2);
    }
    return env->IsSameObject(obj1, obj2);
}

jobject RemoteVM::AllocObject(jclass clazz) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jobject>(RemoteVMCommand::ALLOC_OBJECT, clazz);
    }
    return local_to_global(env->AllocObject(clazz));
}

jobject RemoteVM::NewObject(jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jobject>(RemoteVMCommand::NEW_OBJECT, clazz, methodID, args);
    }
    return local_to_global(env->NewObjectA(clazz, methodID, args.data()));
}

jclass RemoteVM::GetObjectClass(jobject obj) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jclass>(RemoteVMCommand::GET_OBJECT_CLASS, obj);
    }
    return local_to_global(env->GetObjectClass(obj));
}

jboolean RemoteVM::IsInstanceOf(jobject obj, jclass clazz) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jboolean>(RemoteVMCommand::IS_INSTANCE_OF, obj, clazz);
    }
    return env->IsInstanceOf(obj, clazz);
}

jmethodID RemoteVM::GetMethodID(jclass clazz, const std::string &name, const std::string &sig) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jmethodID>(RemoteVMCommand::GET_METHOD_ID, clazz, name, sig);
    }
    return env->GetMethodID(clazz, name.c_str(), sig.c_str());
}

jobject RemoteVM::CallObjectMethod(jobject obj, jmethodID methodID, const std::vector<jvalue> &args) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jobject>(RemoteVMCommand::CALL_OBJECT_METHOD, obj, methodID, args);
    }
    return local_to_global(env->CallObjectMethodA(obj, methodID, args.data()));
}

jboolean RemoteVM::CallBooleanMethod(jobject obj, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jboolean>(RemoteVMCommand::CALL_BOOLEAN_METHOD, obj, methodID, args);
    }
    return env->CallBooleanMethodA(obj, methodID, args.data());
}

jbyte RemoteVM::CallByteMethod(jobject obj, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jbyte>(RemoteVMCommand::CALL_BYTE_METHOD, obj, methodID, args);
    }
    return env->CallByteMethodA(obj, methodID, args.data());
}

jchar RemoteVM::CallCharMethod(jobject obj, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jchar>(RemoteVMCommand::CALL_CHAR_METHOD, obj, methodID, args);
    }
    return env->CallCharMethodA(obj, methodID, args.data());
}

jshort RemoteVM::CallShortMethod(jobject obj, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jshort>(RemoteVMCommand::CALL_SHORT_METHOD, obj, methodID, args);
    }
    return env->CallShortMethodA(obj, methodID, args.data());
}

jint RemoteVM::CallIntMethod(jobject obj, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jint>(RemoteVMCommand::CALL_INT_METHOD, obj, methodID, args);
    }
    return env->CallIntMethodA(obj, methodID, args.data());
}

jlong RemoteVM::CallLongMethod(jobject obj, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jlong>(RemoteVMCommand::CALL_LONG_METHOD, obj, methodID, args);
    }
    return env->CallLongMethodA(obj, methodID, args.data());
}

jfloat RemoteVM::CallFloatMethod(jobject obj, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jfloat>(RemoteVMCommand::CALL_FLOAT_METHOD, obj, methodID, args);
    }
    return env->CallFloatMethodA(obj, methodID, args.data());
}

jdouble RemoteVM::CallDoubleMethod(jobject obj, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jdouble>(RemoteVMCommand::CALL_DOUBLE_METHOD, obj, methodID, args);
    }
    return env->CallDoubleMethodA(obj, methodID, args.data());
}

void RemoteVM::CallVoidMethod(jobject obj, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::CALL_VOID_METHOD, obj, methodID, args);
    }
    return env->CallVoidMethodA(obj, methodID, args.data());
}

jobject RemoteVM::CallNonvirtualObjectMethod(jobject obj, jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jobject>(RemoteVMCommand::CALL_NON_VIRTUAL_OBJECT_METHOD, obj, clazz, methodID, args);
    }
    return local_to_global(env->CallNonvirtualObjectMethodA(obj, clazz, methodID, args.data()));
}

jboolean RemoteVM::CallNonvirtualBooleanMethod(jobject obj, jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jboolean>(RemoteVMCommand::CALL_NON_VIRTUAL_BOOLEAN_METHOD, obj, clazz, methodID, args);
    }
    return env->CallNonvirtualBooleanMethodA(obj, clazz, methodID, args.data());
}

jbyte RemoteVM::CallNonvirtualByteMethod(jobject obj, jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jbyte>(RemoteVMCommand::CALL_NON_VIRTUAL_BYTE_METHOD, obj, clazz, methodID, args);
    }
    return env->CallNonvirtualByteMethodA(obj, clazz, methodID, args.data());
}

jchar RemoteVM::CallNonvirtualCharMethod(jobject obj, jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jchar>(RemoteVMCommand::CALL_NON_VIRTUAL_CHAR_METHOD, obj, clazz, methodID, args);
    }
    return env->CallNonvirtualCharMethodA(obj, clazz, methodID, args.data());
}

jshort RemoteVM::CallNonvirtualShortMethod(jobject obj, jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jshort>(RemoteVMCommand::CALL_NON_VIRTUAL_SHORT_METHOD, obj, clazz, methodID, args);
    }
    return env->CallNonvirtualShortMethodA(obj, clazz, methodID, args.data());
}

jint RemoteVM::CallNonvirtualIntMethod(jobject obj, jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jint>(RemoteVMCommand::CALL_NON_VIRTUAL_INT_METHOD, obj, clazz, methodID, args);
    }
    return env->CallNonvirtualIntMethodA(obj, clazz, methodID, args.data());
}

jlong RemoteVM::CallNonvirtualLongMethod(jobject obj, jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jlong>(RemoteVMCommand::CALL_NON_VIRTUAL_LONG_METHOD, obj, clazz, methodID, args);
    }
    return env->CallNonvirtualLongMethodA(obj, clazz, methodID, args.data());
}

jfloat RemoteVM::CallNonvirtualFloatMethod(jobject obj, jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jfloat>(RemoteVMCommand::CALL_NON_VIRTUAL_FLOAT_METHOD, obj, clazz, methodID, args);
    }
    return env->CallNonvirtualFloatMethodA(obj, clazz, methodID, args.data());
}

jdouble RemoteVM::CallNonvirtualDoubleMethod(jobject obj, jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jdouble>(RemoteVMCommand::CALL_NON_VIRTUAL_DOUBLE_METHOD, obj, clazz, methodID, args);
    }
    return env->CallNonvirtualDoubleMethodA(obj, clazz, methodID, args.data());
}

void RemoteVM::CallNonvirtualVoidMethod(jobject obj, jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::CALL_NON_VIRTUAL_VOID_METHOD, obj, clazz, methodID, args);
    }
    return env->CallNonvirtualVoidMethodA(obj, clazz, methodID, args.data());
}

jfieldID RemoteVM::GetFieldID(jclass clazz, const std::string &name, const std::string &sig) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jfieldID>(RemoteVMCommand::GET_FIELD_ID, clazz, name, sig);
    }
    return env->GetFieldID(clazz, name.c_str(), sig.c_str());
}

jobject RemoteVM::GetObjectField(jobject obj, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jobject>(RemoteVMCommand::GET_OBJECT_FIELD, obj, fieldID);
    }
    return local_to_global(env->GetObjectField(obj, fieldID));
}

jboolean RemoteVM::GetBooleanField(jobject obj, jfieldID fieldID) const noexcept 
{
    if (this->send_command)
    {
        return SendCommand<jboolean>(RemoteVMCommand::GET_BOOLEAN_FIELD, obj, fieldID);
    }
    return env->GetBooleanField(obj, fieldID);
}

jbyte RemoteVM::GetByteField(jobject obj, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jbyte>(RemoteVMCommand::GET_BYTE_FIELD, obj, fieldID);
    }
    return env->GetByteField(obj, fieldID);
}

jchar RemoteVM::GetCharField(jobject obj, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jchar>(RemoteVMCommand::GET_CHAR_FIELD, obj, fieldID);
    }
    return env->GetCharField(obj, fieldID);
}

jshort RemoteVM::GetShortField(jobject obj, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jshort>(RemoteVMCommand::GET_SHORT_FIELD, obj, fieldID);
    }
    return env->GetShortField(obj, fieldID);
}

jint RemoteVM::GetIntField(jobject obj, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jint>(RemoteVMCommand::GET_INT_FIELD, obj, fieldID);
    }
    return env->GetIntField(obj, fieldID);
}

jlong RemoteVM::GetLongField(jobject obj, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jlong>(RemoteVMCommand::GET_LONG_FIELD, obj, fieldID);
    }
    return env->GetLongField(obj, fieldID);
}

jfloat RemoteVM::GetFloatField(jobject obj, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jfloat>(RemoteVMCommand::GET_FLOAT_FIELD, obj, fieldID);
    }
    return env->GetFloatField(obj, fieldID);
}

jdouble RemoteVM::GetDoubleField(jobject obj, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jdouble>(RemoteVMCommand::GET_DOUBLE_FIELD, obj, fieldID);
    }
    return env->GetDoubleField(obj, fieldID);
}

void RemoteVM::SetObjectField(jobject obj, jfieldID fieldID, jobject val) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_OBJECT_FIELD, obj, fieldID, val);
    }
    return env->SetObjectField(obj, fieldID, val);
}

void RemoteVM::SetBooleanField(jobject obj, jfieldID fieldID, jboolean val) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_BOOLEAN_FIELD, obj, fieldID, val);
    }
    return env->SetBooleanField(obj, fieldID, val);
}

void RemoteVM::SetByteField(jobject obj, jfieldID fieldID, jbyte val) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_BYTE_FIELD, obj, fieldID, val);
    }
    return env->SetByteField(obj, fieldID, val);
}

void RemoteVM::SetCharField(jobject obj, jfieldID fieldID, jchar val) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_CHAR_FIELD, obj, fieldID, val);
    }
    return env->SetCharField(obj, fieldID, val);
}

void RemoteVM::SetShortField(jobject obj, jfieldID fieldID, jshort val) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_SHORT_FIELD, obj, fieldID, val);
    }
    return env->SetShortField(obj, fieldID, val);
}

void RemoteVM::SetIntField(jobject obj, jfieldID fieldID, jint val) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_INT_FIELD, obj, fieldID, val);
    }
    return env->SetIntField(obj, fieldID, val);
}

void RemoteVM::SetLongField(jobject obj, jfieldID fieldID, jlong val) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_LONG_FIELD, obj, fieldID, val);
    }
    return env->SetLongField(obj, fieldID, val);
}

void RemoteVM::SetFloatField(jobject obj, jfieldID fieldID, jfloat val) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_FLOAT_FIELD, obj, fieldID, val);
    }
    return env->SetFloatField(obj, fieldID, val);
}

void RemoteVM::SetDoubleField(jobject obj, jfieldID fieldID, jdouble val) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_DOUBLE_FIELD, obj, fieldID, val);
    }
    return env->SetDoubleField(obj, fieldID, val);
}

jmethodID RemoteVM::GetStaticMethodID(jclass clazz, const std::string &name, const std::string &sig) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jmethodID>(RemoteVMCommand::GET_STATIC_METHOD_ID, clazz, name, sig);
    }
    return env->GetStaticMethodID(clazz, name.c_str(), sig.c_str());
}

jobject RemoteVM::CallStaticObjectMethod(jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jobject>(RemoteVMCommand::CALL_STATIC_OBJECT_METHOD, clazz, methodID, args);
    }
    return local_to_global(env->CallStaticObjectMethodA(clazz, methodID, args.data()));
}

jboolean RemoteVM::CallStaticBooleanMethod(jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jboolean>(RemoteVMCommand::CALL_STATIC_BOOLEAN_METHOD, clazz, methodID, args);
    }
    return env->CallStaticBooleanMethodA(clazz, methodID, args.data());
}

jbyte RemoteVM::CallStaticByteMethod(jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jbyte>(RemoteVMCommand::CALL_STATIC_BYTE_METHOD, clazz, methodID, args);
    }
    return env->CallStaticByteMethodA(clazz, methodID, args.data());
}

jchar RemoteVM::CallStaticCharMethod(jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jchar>(RemoteVMCommand::CALL_STATIC_CHAR_METHOD, clazz, methodID, args);
    }
    return env->CallStaticCharMethodA(clazz, methodID, args.data());
}

jshort RemoteVM::CallStaticShortMethod(jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jshort>(RemoteVMCommand::CALL_STATIC_SHORT_METHOD, clazz, methodID, args);
    }
    return env->CallStaticShortMethodA(clazz, methodID, args.data());
}

jint RemoteVM::CallStaticIntMethod(jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jint>(RemoteVMCommand::CALL_STATIC_INT_METHOD, clazz, methodID, args);
    }
    return env->CallStaticIntMethodA(clazz, methodID, args.data());
}

jlong RemoteVM::CallStaticLongMethod(jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jlong>(RemoteVMCommand::CALL_STATIC_LONG_METHOD, clazz, methodID, args);
    }
    return env->CallStaticLongMethodA(clazz, methodID, args.data());
}

jfloat RemoteVM::CallStaticFloatMethod(jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jfloat>(RemoteVMCommand::CALL_STATIC_FLOAT_METHOD, clazz, methodID, args);
    }
    return env->CallStaticFloatMethodA(clazz, methodID, args.data());
}

jdouble RemoteVM::CallStaticDoubleMethod(jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jdouble>(RemoteVMCommand::CALL_STATIC_DOUBLE_METHOD, clazz, methodID, args);
    }
    return env->CallStaticDoubleMethodA(clazz, methodID, args.data());
}

void RemoteVM::CallStaticVoidMethod(jclass clazz, jmethodID methodID, const std::vector<jvalue> &args) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::CALL_STATIC_VOID_METHOD, clazz, methodID, args);
    }
    return env->CallStaticVoidMethodA(clazz, methodID, args.data());
}

jfieldID RemoteVM::GetStaticFieldID(jclass clazz, const std::string &name, const std::string &sig) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jfieldID>(RemoteVMCommand::GET_STATIC_FIELD_ID, clazz, name, sig);
    }
    return env->GetStaticFieldID(clazz, name.c_str(), sig.c_str());
}

jobject RemoteVM::GetStaticObjectField(jclass clazz, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jobject>(RemoteVMCommand::GET_STATIC_OBJECT_FIELD, clazz, fieldID);
    }
    return local_to_global(env->GetStaticObjectField(clazz, fieldID));
}

jboolean RemoteVM::GetStaticBooleanField(jclass clazz, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jboolean>(RemoteVMCommand::GET_STATIC_BOOLEAN_FIELD, clazz, fieldID);
    }
    return env->GetStaticBooleanField(clazz, fieldID);
}

jbyte RemoteVM::GetStaticByteField(jclass clazz, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jbyte>(RemoteVMCommand::GET_STATIC_BYTE_FIELD, clazz, fieldID);
    }
    return env->GetStaticByteField(clazz, fieldID);
}

jchar RemoteVM::GetStaticCharField(jclass clazz, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jchar>(RemoteVMCommand::GET_STATIC_CHAR_FIELD, clazz, fieldID);
    }
    return env->GetStaticCharField(clazz, fieldID);
}

jshort RemoteVM::GetStaticShortField(jclass clazz, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jshort>(RemoteVMCommand::GET_STATIC_SHORT_FIELD, clazz, fieldID);
    }
    return env->GetStaticShortField(clazz, fieldID);
}

jint RemoteVM::GetStaticIntField(jclass clazz, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jint>(RemoteVMCommand::GET_STATIC_INT_FIELD, clazz, fieldID);
    }
    return env->GetStaticIntField(clazz, fieldID);
}

jlong RemoteVM::GetStaticLongField(jclass clazz, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jlong>(RemoteVMCommand::GET_STATIC_LONG_FIELD, clazz, fieldID);
    }
    return env->GetStaticLongField(clazz, fieldID);
}

jfloat RemoteVM::GetStaticFloatField(jclass clazz, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jfloat>(RemoteVMCommand::GET_STATIC_FLOAT_FIELD, clazz, fieldID);
    }
    return env->GetStaticFloatField(clazz, fieldID);
}

jdouble RemoteVM::GetStaticDoubleField(jclass clazz, jfieldID fieldID) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jdouble>(RemoteVMCommand::GET_STATIC_DOUBLE_FIELD, clazz, fieldID);
    }
    return env->GetStaticDoubleField(clazz, fieldID);
}

void RemoteVM::SetStaticObjectField(jclass clazz, jfieldID fieldID, jobject value) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_STATIC_OBJECT_FIELD, clazz, fieldID, value);
    }
    return env->SetStaticObjectField(clazz, fieldID, value);
}

void RemoteVM::SetStaticBooleanField(jclass clazz, jfieldID fieldID, jboolean value) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_STATIC_BOOLEAN_FIELD, clazz, fieldID, value);
    }
    return env->SetStaticBooleanField(clazz, fieldID, value);
}

void RemoteVM::SetStaticByteField(jclass clazz, jfieldID fieldID, jbyte value) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_STATIC_BYTE_FIELD, clazz, fieldID, value);
    }
    return env->SetStaticByteField(clazz, fieldID, value);
}

void RemoteVM::SetStaticCharField(jclass clazz, jfieldID fieldID, jchar value) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_STATIC_CHAR_FIELD, clazz, fieldID, value);
    }
    return env->SetStaticCharField(clazz, fieldID, value);
}

void RemoteVM::SetStaticShortField(jclass clazz, jfieldID fieldID, jshort value) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_STATIC_SHORT_FIELD, clazz, fieldID, value);
    }
    return env->SetStaticShortField(clazz, fieldID, value);
}

void RemoteVM::SetStaticIntField(jclass clazz, jfieldID fieldID, jint value) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_STATIC_INT_FIELD, clazz, fieldID, value);
    }
    return env->SetStaticIntField(clazz, fieldID, value);
}

void RemoteVM::SetStaticLongField(jclass clazz, jfieldID fieldID, jlong value) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_STATIC_LONG_FIELD, clazz, fieldID, value);
    }
    return env->SetStaticLongField(clazz, fieldID, value);
}

void RemoteVM::SetStaticFloatField(jclass clazz, jfieldID fieldID, jfloat value) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_STATIC_FLOAT_FIELD, clazz, fieldID, value);
    }
    return env->SetStaticFloatField(clazz, fieldID, value);
}

void RemoteVM::SetStaticDoubleField(jclass clazz, jfieldID fieldID, jdouble value) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_STATIC_DOUBLE_FIELD, clazz, fieldID, value);
    }
    return env->SetStaticDoubleField(clazz, fieldID, value);
}

jstring RemoteVM::NewString(const std::wstring &unicode) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jstring>(RemoteVMCommand::NEW_STRING, unicode);
    }
    return local_to_global(env->NewString(reinterpret_cast<const jchar*>(unicode.c_str()), static_cast<jsize>(unicode.size())));
}

jsize RemoteVM::GetStringLength(jstring str) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jsize>(RemoteVMCommand::GET_STRING_LENGTH, str);
    }
    return env->GetStringLength(str);
}

std::wstring RemoteVM::GetStringChars(jstring str) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::wstring>(RemoteVMCommand::GET_STRING_CHARS, str);
    }

    const jchar* chars = env->GetStringChars(str, nullptr);
    if (chars)
    {
        jsize length = env->GetStringLength(str);
        if (length > 0)
        {
            std::wstring result = std::wstring(reinterpret_cast<const wchar_t*>(chars), length);
            env->ReleaseStringChars(str, chars);
            return result;
        }

        env->ReleaseStringChars(str, chars);
    }
    return std::wstring();
}

jstring RemoteVM::NewStringUTF(const std::string &utf) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jstring>(RemoteVMCommand::NEW_STRING_UTF, utf);
    }
    return local_to_global(env->NewStringUTF(utf.c_str()));
}

jsize RemoteVM::GetStringUTFLength(jstring str) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jsize>(RemoteVMCommand::GET_STRING_UTF_LENGTH, str);
    }
    return env->GetStringUTFLength(str);
}

std::string RemoteVM::GetStringUTFChars(jstring str) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::string>(RemoteVMCommand::GET_STRING_UTF_CHARS, str);
    }

    const char* chars = env->GetStringUTFChars(str, nullptr);
    if (chars)
    {
        jsize length = env->GetStringUTFLength(str);
        if (length > 0)
        {
            std::string result = std::string(chars, length);
            env->ReleaseStringUTFChars(str, chars);
            return result;
        }

        env->ReleaseStringUTFChars(str, chars);
    }
    return std::string();
}

jsize RemoteVM::GetArrayLength(jarray array) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jsize>(RemoteVMCommand::GET_ARRAY_LENGTH, array);
    }
    return env->GetArrayLength(array);
}

jobjectArray RemoteVM::NewObjectArray(jsize len, jclass clazz, jobject init) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jobjectArray>(RemoteVMCommand::NEW_OBJECT_ARRAY, len, clazz, init);
    }
    return local_to_global(env->NewObjectArray(len, clazz, init));
}

jbooleanArray RemoteVM::NewBooleanArray(jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jbooleanArray>(RemoteVMCommand::NEW_BOOLEAN_ARRAY, len);
    }
    return local_to_global(env->NewBooleanArray(len));
}

jbyteArray RemoteVM::NewByteArray(jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jbyteArray>(RemoteVMCommand::NEW_BYTE_ARRAY, len);
    }
    return local_to_global(env->NewByteArray(len));
}

jcharArray RemoteVM::NewCharArray(jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jcharArray>(RemoteVMCommand::NEW_CHAR_ARRAY, len);
    }
    return local_to_global(env->NewCharArray(len));
}

jshortArray RemoteVM::NewShortArray(jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jshortArray>(RemoteVMCommand::NEW_SHORT_ARRAY, len);
    }
    return local_to_global(env->NewShortArray(len));
}

jintArray RemoteVM::NewIntArray(jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jintArray>(RemoteVMCommand::NEW_INT_ARRAY, len);
    }
    return local_to_global(env->NewIntArray(len));
}

jlongArray RemoteVM::NewLongArray(jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jlongArray>(RemoteVMCommand::NEW_LONG_ARRAY, len);
    }
    return local_to_global(env->NewLongArray(len));
}

jfloatArray RemoteVM::NewFloatArray(jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jfloatArray>(RemoteVMCommand::NEW_FLOAT_ARRAY, len);
    }
    return local_to_global(env->NewFloatArray(len));
}

jdoubleArray RemoteVM::NewDoubleArray(jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jdoubleArray>(RemoteVMCommand::NEW_DOUBLE_ARRAY, len);
    }
    return local_to_global(env->NewDoubleArray(len));
}

std::vector<jobject> RemoteVM::GetObjectArrayElements(jobjectArray array) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jobject>>(RemoteVMCommand::GET_OBJECT_ARRAY_ELEMENTS, array);
    }

    std::vector<jobject> result;
    jsize length = env->GetArrayLength(array);
    result.reserve(length);
    for (jsize i = 0; i < length; ++i)
    {
        jobject local = env->GetObjectArrayElement(array, i);
        result.push_back(local ? env->NewGlobalRef(local) : nullptr);
        env->DeleteLocalRef(local);
    }
    return result;
}

void RemoteVM::SetObjectArrayElements(jobjectArray array, jsize index, const std::vector<jobject> &elements) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_OBJECT_ARRAY_ELEMENTS, array, index, elements);
    }

    for (std::size_t i = 0; i < elements.size(); ++i)
    {
        env->SetObjectArrayElement(array, i + index, elements[i]);
    }
}

std::vector<jboolean> RemoteVM::GetBooleanArrayElements(jbooleanArray array) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jboolean>>(RemoteVMCommand::GET_BOOLEAN_ARRAY_ELEMENTS, array);
    }

    std::vector<jboolean> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        jboolean* items = env->GetBooleanArrayElements(array, nullptr);
        if (items)
        {
            std::memcpy(&elements[0], items, sizeof(jboolean) * elements.size());
            env->ReleaseBooleanArrayElements(array, items, JNI_ABORT);
            return elements;
        }
    }
    return {};
}

std::vector<jbyte> RemoteVM::GetByteArrayElements(jbyteArray array) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jbyte>>(RemoteVMCommand::GET_BYTE_ARRAY_ELEMENTS, array);
    }

    std::vector<jbyte> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        jbyte* items = env->GetByteArrayElements(array, nullptr);
        if (items)
        {
            std::memcpy(&elements[0], items, sizeof(jbyte) * elements.size());
            env->ReleaseByteArrayElements(array, items, JNI_ABORT);
            return elements;
        }
    }
    return {};
}

std::vector<jchar> RemoteVM::GetCharArrayElements(jcharArray array) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jchar>>(RemoteVMCommand::GET_CHAR_ARRAY_ELEMENTS, array);
    }

    std::vector<jchar> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        jchar* items = env->GetCharArrayElements(array, nullptr);
        if (items)
        {
            std::memcpy(&elements[0], items, sizeof(jchar) * elements.size());
            env->ReleaseCharArrayElements(array, items, JNI_ABORT);
            return elements;
        }
    }
    return {};
}

std::vector<jshort> RemoteVM::GetShortArrayElements(jshortArray array) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jshort>>(RemoteVMCommand::GET_SHORT_ARRAY_ELEMENTS, array);
    }

    std::vector<jshort> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        jshort* items = env->GetShortArrayElements(array, nullptr);
        if (items)
        {
            std::memcpy(&elements[0], items, sizeof(jshort) * elements.size());
            env->ReleaseShortArrayElements(array, items, JNI_ABORT);
            return elements;
        }
    }
    return {};
}

std::vector<jint> RemoteVM::GetIntArrayElements(jintArray array) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jint>>(RemoteVMCommand::GET_INT_ARRAY_ELEMENTS, array);
    }

    std::vector<jint> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        jint* items = env->GetIntArrayElements(array, nullptr);
        if (items)
        {
            std::memcpy(&elements[0], items, sizeof(jint) * elements.size());
            env->ReleaseIntArrayElements(array, items, JNI_ABORT);
            return elements;
        }
    }
    return {};
}

std::vector<jlong> RemoteVM::GetLongArrayElements(jlongArray array) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jlong>>(RemoteVMCommand::GET_LONG_ARRAY_ELEMENTS, array);
    }

    std::vector<jlong> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        jlong* items = env->GetLongArrayElements(array, nullptr);
        if (items)
        {
            std::memcpy(&elements[0], items, sizeof(jlong) * elements.size());
            env->ReleaseLongArrayElements(array, items, JNI_ABORT);
            return elements;
        }
    }
    return {};
}

std::vector<jfloat> RemoteVM::GetFloatArrayElements(jfloatArray array) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jfloat>>(RemoteVMCommand::GET_FLOAT_ARRAY_ELEMENTS, array);
    }

    std::vector<jfloat> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        jfloat* items = env->GetFloatArrayElements(array, nullptr);
        if (items)
        {
            std::memcpy(&elements[0], items, sizeof(jfloat) * elements.size());
            env->ReleaseFloatArrayElements(array, items, JNI_ABORT);
            return elements;
        }
    }
    return {};
}

std::vector<jdouble> RemoteVM::GetDoubleArrayElements(jdoubleArray array) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jdouble>>(RemoteVMCommand::GET_DOUBLE_ARRAY_ELEMENTS, array);
    }

    std::vector<jdouble> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        jdouble* items = env->GetDoubleArrayElements(array, nullptr);
        if (items)
        {
            std::memcpy(&elements[0], items, sizeof(jdouble) * elements.size());
            env->ReleaseDoubleArrayElements(array, items, JNI_ABORT);
            return elements;
        }
    }
    return {};
}

std::vector<jboolean> RemoteVM::GetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jboolean>>(RemoteVMCommand::GET_BOOLEAN_ARRAY_REGION, array, start, len);
    }

    std::vector<jboolean> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        env->GetBooleanArrayRegion(array, start, len, &elements[0]);
        return elements;
    }
    return {};
}

std::vector<jbyte> RemoteVM::GetByteArrayRegion(jbyteArray array, jsize start, jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jbyte>>(RemoteVMCommand::GET_BYTE_ARRAY_REGION, array, start, len);
    }

    std::vector<jbyte> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        env->GetByteArrayRegion(array, start, len, &elements[0]);
        return elements;
    }
    return {};
}

std::vector<jchar> RemoteVM::GetCharArrayRegion(jcharArray array, jsize start, jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jchar>>(RemoteVMCommand::GET_CHAR_ARRAY_REGION, array, start, len);
    }

    std::vector<jchar> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        env->GetCharArrayRegion(array, start, len, &elements[0]);
        return elements;
    }
    return {};
}

std::vector<jshort> RemoteVM::GetShortArrayRegion(jshortArray array, jsize start, jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jshort>>(RemoteVMCommand::GET_SHORT_ARRAY_REGION, array, start, len);
    }

    std::vector<jshort> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        env->GetShortArrayRegion(array, start, len, &elements[0]);
        return elements;
    }
    return {};
}

std::vector<jint> RemoteVM::GetIntArrayRegion(jintArray array, jsize start, jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jint>>(RemoteVMCommand::GET_INT_ARRAY_REGION, array, start, len);
    }

    std::vector<jint> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        env->GetIntArrayRegion(array, start, len, &elements[0]);
        return elements;
    }
    return {};
}

std::vector<jlong> RemoteVM::GetLongArrayRegion(jlongArray array, jsize start, jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jlong>>(RemoteVMCommand::GET_LONG_ARRAY_REGION, array, start, len);
    }

    std::vector<jlong> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        env->GetLongArrayRegion(array, start, len, &elements[0]);
        return elements;
    }
    return {};
}

std::vector<jfloat> RemoteVM::GetFloatArrayRegion(jfloatArray array, jsize start, jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jfloat>>(RemoteVMCommand::GET_FLOAT_ARRAY_REGION, array, start, len);
    }

    std::vector<jfloat> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        env->GetFloatArrayRegion(array, start, len, &elements[0]);
        return elements;
    }
    return {};
}

std::vector<jdouble> RemoteVM::GetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<std::vector<jdouble>>(RemoteVMCommand::GET_DOUBLE_ARRAY_REGION, array, start, len);
    }

    std::vector<jdouble> elements;
    elements.resize(env->GetArrayLength(array));
    if (!elements.empty())
    {
        env->GetDoubleArrayRegion(array, start, len, &elements[0]);
        return elements;
    }
    return {};
}

void RemoteVM::SetBooleanArrayRegion(jbooleanArray array, jsize start, const std::vector<jboolean>& buf) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_BOOLEAN_ARRAY_REGION, array, start, buf);
    }
    return env->SetBooleanArrayRegion(array, start, static_cast<jsize>(buf.size()), buf.data());
}

void RemoteVM::SetByteArrayRegion(jbyteArray array, jsize start, const std::vector<jbyte>& buf) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_BYTE_ARRAY_REGION, array, start, buf);
    }
    return env->SetByteArrayRegion(array, start, static_cast<jsize>(buf.size()), buf.data());
}

void RemoteVM::SetCharArrayRegion(jcharArray array, jsize start, const std::vector<jchar>& buf) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_CHAR_ARRAY_REGION, array, start, buf);
    }
    return env->SetCharArrayRegion(array, start, static_cast<jsize>(buf.size()), buf.data());
}

void RemoteVM::SetShortArrayRegion(jshortArray array, jsize start, const std::vector<jshort>& buf) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_SHORT_ARRAY_REGION, array, start, buf);
    }
    return env->SetShortArrayRegion(array, start, static_cast<jsize>(buf.size()), buf.data());
}

void RemoteVM::SetIntArrayRegion(jintArray array, jsize start, const std::vector<jint>& buf) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_INT_ARRAY_REGION, array, start, buf);
    }
    return env->SetIntArrayRegion(array, start, static_cast<jsize>(buf.size()), buf.data());
}

void RemoteVM::SetLongArrayRegion(jlongArray array, jsize start, const std::vector<jlong>& buf) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_LONG_ARRAY_REGION, array, start, buf);
    }
    return env->SetLongArrayRegion(array, start, static_cast<jsize>(buf.size()), buf.data());
}

void RemoteVM::SetFloatArrayRegion(jfloatArray array, jsize start, const std::vector<jfloat>& buf) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_FLOAT_ARRAY_REGION, array, start, buf);
    }
    return env->SetFloatArrayRegion(array, start, static_cast<jsize>(buf.size()), buf.data());
}

void RemoteVM::SetDoubleArrayRegion(jdoubleArray array, jsize start, const std::vector<jdouble>& buf) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<void>(RemoteVMCommand::SET_DOUBLE_ARRAY_REGION, array, start, buf);
    }
    return env->SetDoubleArrayRegion(array, start, static_cast<jsize>(buf.size()), buf.data());
}

jint RemoteVM::MonitorEnter(jobject obj) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jint>(RemoteVMCommand::MONITOR_ENTER, obj);
    }
    return env->MonitorEnter(obj);
}

jint RemoteVM::MonitorExit(jobject obj) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jint>(RemoteVMCommand::MONITOR_EXIT, obj);
    }
    return env->MonitorExit(obj);
}

JavaVM* RemoteVM::GetJavaVM() const noexcept
{
    if (this->send_command)
    {
        return SendCommand<JavaVM*>(RemoteVMCommand::GET_JAVA_VM);
    }

    JavaVM* jvm = nullptr;
    return env->GetJavaVM(&jvm) == 0 ? jvm : nullptr;
}

jobject RemoteVM::NewDirectByteBuffer(void* address, jlong capacity) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jobject>(RemoteVMCommand::NEW_DIRECT_BYTE_BUFFER, address, capacity);
    }
    return local_to_global(env->NewDirectByteBuffer(address, capacity));
}

void* RemoteVM::GetDirectBufferAddress(jobject buf) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jobject>(RemoteVMCommand::GET_DIRECT_BUFFER_ADDRESS, buf);
    }
    return env->GetDirectBufferAddress(buf);
}

jlong RemoteVM::GetDirectBufferCapacity(jobject buf) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jlong>(RemoteVMCommand::GET_DIRECT_BUFFER_CAPACITY, buf);
    }
    return env->GetDirectBufferCapacity(buf);
}

jobjectRefType RemoteVM::GetObjectRefType(jobject obj) const noexcept
{
    if (this->send_command)
    {
        return SendCommand<jobjectRefType>(RemoteVMCommand::GET_OBJECT_REF_TYPE, obj);
    }
    return env->GetObjectRefType(obj);
}

bool RemoteVM::is_remote() const noexcept
{
    return !env && control_center && send_command && get_image_data;
}

void RemoteVM::process_command(void* arguments, void* response) const noexcept
{
    RemoteVMCommand command = Remote_VM_Read<RemoteVMCommand>(arguments);
    switch (command)
    {
        case RemoteVMCommand::ALLOCATE_MEMORY:
        {
            void* result = this->ExecuteCommand(arguments, &RemoteVM::AllocateMemory);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::READ_MEMORY:
        {
            void* source = Remote_VM_Read<void*>(arguments);
            jsize size = Remote_VM_Read<jsize>(arguments);
            bool result = this->ReadMemory(response, source, size);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::WRITE_MEMORY:
        {
            void* destinaton = Remote_VM_Read<void*>(arguments);
            jsize size = Remote_VM_Read<jsize>(arguments);
            bool result = this->WriteMemory(destinaton, arguments, size);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::FREE_MEMORY:
        {
            this->ExecuteCommand(arguments, &RemoteVM::FreeMemory);
        }
            break;

        case RemoteVMCommand::GET_VERSION:
        {
            jint result = this->ExecuteCommand(arguments, &RemoteVM::GetVersion);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::DEFINE_CLASS:
        {
            std::string name = Remote_VM_Read<std::string>(arguments);
            jobject loader = Remote_VM_Read<jobject>(arguments);
            jbyte* buf = Remote_VM_Read<jbyte*>(arguments);
            jsize len = Remote_VM_Read<jsize>(arguments);
            jclass result = this->DefineClass(name.c_str(), loader, buf == nullptr ? static_cast<jbyte*>(arguments) : buf, len);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::FIND_CLASS:
        {
            jobject result = this->ExecuteCommand(arguments, &RemoteVM::FindClass);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::FROM_REFLECTED_METHOD:
        {
            jmethodID result = this->ExecuteCommand(arguments, &RemoteVM::FromReflectedMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::FROM_REFLECTED_FIELD:
        {
            jfieldID result = this->ExecuteCommand(arguments, &RemoteVM::FromReflectedField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::TO_REFLECTED_METHOD:
        {
            jobject result = this->ExecuteCommand(arguments, &RemoteVM::ToReflectedMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_SUPER_CLASS:
        {
            jclass result = this->ExecuteCommand(arguments, &RemoteVM::GetSuperclass);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::IS_ASSIGNABLE_FROM:
        {
            bool result = this->ExecuteCommand(arguments, &RemoteVM::IsAssignableFrom);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::TO_REFLECTED_FIELD:
        {
            jobject result = this->ExecuteCommand(arguments, &RemoteVM::ToReflectedField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::THROW:
        {
            jint result = this->ExecuteCommand(arguments, &RemoteVM::Throw);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::THROW_NEW:
        {
            jint result = this->ExecuteCommand(arguments, &RemoteVM::ThrowNew);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_EXCEPTION_MESSAGE:
        {
            std::string result = this->ExecuteCommand(arguments, &RemoteVM::GetExceptionMessage);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::FATAL_ERROR:
        {
            this->ExecuteCommand(arguments, &RemoteVM::FatalError);
        }
            break;

        case RemoteVMCommand::DELETE_GLOBAL_REF:
        {
            this->ExecuteCommand(arguments, &RemoteVM::DeleteGlobalRef);
        }
            break;

        case RemoteVMCommand::IS_SAME_OBJECT:
        {
            jboolean result = this->ExecuteCommand(arguments, &RemoteVM::IsSameObject);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::ALLOC_OBJECT:
        {
            jobject result = this->ExecuteCommand(arguments, &RemoteVM::AllocObject);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::NEW_OBJECT:
        {
            jobject result = this->ExecuteCommand(arguments, &RemoteVM::NewObject);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_OBJECT_CLASS:
        {
            jclass result = this->ExecuteCommand(arguments, &RemoteVM::GetObjectClass);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::IS_INSTANCE_OF:
        {
            jboolean result = this->ExecuteCommand(arguments, &RemoteVM::IsInstanceOf);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_METHOD_ID:
        {
            jmethodID result = this->ExecuteCommand(arguments, &RemoteVM::GetMethodID);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_OBJECT_METHOD:
        {
            jobject result = this->ExecuteCommand(arguments, &RemoteVM::CallObjectMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_BOOLEAN_METHOD:
        {
            jboolean result = this->ExecuteCommand(arguments, &RemoteVM::CallBooleanMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_BYTE_METHOD:
        {
            jbyte result = this->ExecuteCommand(arguments, &RemoteVM::CallByteMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_CHAR_METHOD:
        {
            jchar result = this->ExecuteCommand(arguments, &RemoteVM::CallCharMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_SHORT_METHOD:
        {
            jshort result = this->ExecuteCommand(arguments, &RemoteVM::CallShortMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_INT_METHOD:
        {
            jint result = this->ExecuteCommand(arguments, &RemoteVM::CallIntMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_LONG_METHOD:
        {
            jlong result = this->ExecuteCommand(arguments, &RemoteVM::CallLongMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_FLOAT_METHOD:
        {
            jfloat result = this->ExecuteCommand(arguments, &RemoteVM::CallFloatMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_DOUBLE_METHOD:
        {
            jdouble result = this->ExecuteCommand(arguments, &RemoteVM::CallDoubleMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_VOID_METHOD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::CallVoidMethod);
        }
            break;

        case RemoteVMCommand::CALL_NON_VIRTUAL_OBJECT_METHOD:
        {
            jobject result = this->ExecuteCommand(arguments, &RemoteVM::CallNonvirtualObjectMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_NON_VIRTUAL_BOOLEAN_METHOD:
        {
            jboolean result = this->ExecuteCommand(arguments, &RemoteVM::CallNonvirtualBooleanMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_NON_VIRTUAL_BYTE_METHOD:
        {
            jbyte result = this->ExecuteCommand(arguments, &RemoteVM::CallNonvirtualByteMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_NON_VIRTUAL_CHAR_METHOD:
        {
            jchar result = this->ExecuteCommand(arguments, &RemoteVM::CallNonvirtualCharMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_NON_VIRTUAL_SHORT_METHOD:
        {
            jshort result = this->ExecuteCommand(arguments, &RemoteVM::CallNonvirtualShortMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_NON_VIRTUAL_INT_METHOD:
        {
            jint result = this->ExecuteCommand(arguments, &RemoteVM::CallNonvirtualIntMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_NON_VIRTUAL_LONG_METHOD:
        {
            jlong result = this->ExecuteCommand(arguments, &RemoteVM::CallNonvirtualLongMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_NON_VIRTUAL_FLOAT_METHOD:
        {
            jfloat result = this->ExecuteCommand(arguments, &RemoteVM::CallNonvirtualFloatMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_NON_VIRTUAL_DOUBLE_METHOD:
        {
            jdouble result = this->ExecuteCommand(arguments, &RemoteVM::CallNonvirtualDoubleMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_NON_VIRTUAL_VOID_METHOD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::CallNonvirtualVoidMethod);
        }
            break;

        case RemoteVMCommand::GET_FIELD_ID:
        {
            jfieldID result = this->ExecuteCommand(arguments, &RemoteVM::GetFieldID);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_OBJECT_FIELD:
        {
            jobject result = this->ExecuteCommand(arguments, &RemoteVM::GetObjectField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_BOOLEAN_FIELD:
        {
            jboolean result = this->ExecuteCommand(arguments, &RemoteVM::GetBooleanField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_BYTE_FIELD:
        {
            jbyte result = this->ExecuteCommand(arguments, &RemoteVM::GetByteField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_CHAR_FIELD:
        {
            jchar result = this->ExecuteCommand(arguments, &RemoteVM::GetCharField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_SHORT_FIELD:
        {
            jshort result = this->ExecuteCommand(arguments, &RemoteVM::GetShortField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_INT_FIELD:
        {
            jint result = this->ExecuteCommand(arguments, &RemoteVM::GetIntField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_LONG_FIELD:
        {
            jlong result = this->ExecuteCommand(arguments, &RemoteVM::GetLongField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_FLOAT_FIELD:
        {
            jfloat result = this->ExecuteCommand(arguments, &RemoteVM::GetFloatField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_DOUBLE_FIELD:
        {
            jdouble result = this->ExecuteCommand(arguments, &RemoteVM::GetDoubleField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::SET_OBJECT_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetObjectField);
        }
            break;

        case RemoteVMCommand::SET_BOOLEAN_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetBooleanField);
        }
            break;

        case RemoteVMCommand::SET_BYTE_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetByteField);
        }
            break;

        case RemoteVMCommand::SET_CHAR_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetCharField);
        }
            break;

        case RemoteVMCommand::SET_SHORT_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetShortField);
        }
            break;

        case RemoteVMCommand::SET_INT_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetIntField);
        }
            break;

        case RemoteVMCommand::SET_LONG_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetLongField);
        }
            break;

        case RemoteVMCommand::SET_FLOAT_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetFloatField);
        }
            break;

        case RemoteVMCommand::SET_DOUBLE_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetDoubleField);
        }
            break;

        case RemoteVMCommand::GET_STATIC_METHOD_ID:
        {
            jmethodID result = this->ExecuteCommand(arguments, &RemoteVM::GetStaticMethodID);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_STATIC_OBJECT_METHOD:
        {
            jobject result = this->ExecuteCommand(arguments, &RemoteVM::CallStaticObjectMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_STATIC_BOOLEAN_METHOD:
        {
            jboolean result = this->ExecuteCommand(arguments, &RemoteVM::CallStaticBooleanMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_STATIC_BYTE_METHOD:
        {
            jbyte result = this->ExecuteCommand(arguments, &RemoteVM::CallStaticByteMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_STATIC_CHAR_METHOD:
        {
            jchar result = this->ExecuteCommand(arguments, &RemoteVM::CallStaticCharMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_STATIC_SHORT_METHOD:
        {
            jshort result = this->ExecuteCommand(arguments, &RemoteVM::CallStaticShortMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_STATIC_INT_METHOD:
        {
            jint result = this->ExecuteCommand(arguments, &RemoteVM::CallStaticIntMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_STATIC_LONG_METHOD:
        {
            jlong result = this->ExecuteCommand(arguments, &RemoteVM::CallStaticLongMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_STATIC_FLOAT_METHOD:
        {
            jfloat result = this->ExecuteCommand(arguments, &RemoteVM::CallStaticFloatMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_STATIC_DOUBLE_METHOD:
        {
            jdouble result = this->ExecuteCommand(arguments, &RemoteVM::CallStaticDoubleMethod);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::CALL_STATIC_VOID_METHOD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::CallStaticVoidMethod);
        }
            break;

        case RemoteVMCommand::GET_STATIC_FIELD_ID:
        {
            jfieldID result = this->ExecuteCommand(arguments, &RemoteVM::GetStaticFieldID);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_STATIC_OBJECT_FIELD:
        {
            jobject result = this->ExecuteCommand(arguments, &RemoteVM::GetStaticObjectField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_STATIC_BOOLEAN_FIELD:
        {
            jboolean result = this->ExecuteCommand(arguments, &RemoteVM::GetStaticBooleanField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_STATIC_BYTE_FIELD:
        {
            jbyte result = this->ExecuteCommand(arguments, &RemoteVM::GetStaticByteField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_STATIC_CHAR_FIELD:
        {
            jchar result = this->ExecuteCommand(arguments, &RemoteVM::GetStaticCharField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_STATIC_SHORT_FIELD:
        {
            jshort result = this->ExecuteCommand(arguments, &RemoteVM::GetStaticShortField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_STATIC_INT_FIELD:
        {
            jint result = this->ExecuteCommand(arguments, &RemoteVM::GetStaticIntField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_STATIC_LONG_FIELD:
        {
            jlong result = this->ExecuteCommand(arguments, &RemoteVM::GetStaticLongField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_STATIC_FLOAT_FIELD:
        {
            jfloat result = this->ExecuteCommand(arguments, &RemoteVM::GetStaticFloatField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_STATIC_DOUBLE_FIELD:
        {
            jdouble result = this->ExecuteCommand(arguments, &RemoteVM::GetStaticDoubleField);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::SET_STATIC_OBJECT_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetStaticObjectField);
        }
            break;

        case RemoteVMCommand::SET_STATIC_BOOLEAN_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetStaticBooleanField);
        }
            break;

        case RemoteVMCommand::SET_STATIC_BYTE_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetStaticByteField);
        }
            break;

        case RemoteVMCommand::SET_STATIC_CHAR_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetStaticCharField);
        }
            break;

        case RemoteVMCommand::SET_STATIC_SHORT_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetStaticShortField);
        }
            break;

        case RemoteVMCommand::SET_STATIC_INT_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetStaticIntField);
        }
            break;

        case RemoteVMCommand::SET_STATIC_LONG_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetStaticLongField);
        }
            break;

        case RemoteVMCommand::SET_STATIC_FLOAT_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetStaticFloatField);
        }
            break;

        case RemoteVMCommand::SET_STATIC_DOUBLE_FIELD:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetStaticDoubleField);
        }
            break;

        case RemoteVMCommand::NEW_STRING:
        {
            jstring result = this->ExecuteCommand(arguments, &RemoteVM::NewString);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_STRING_LENGTH:
        {
            jsize result = this->ExecuteCommand(arguments, &RemoteVM::GetStringLength);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_STRING_CHARS:
        {
            std::wstring result = this->ExecuteCommand(arguments, &RemoteVM::GetStringChars);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::NEW_STRING_UTF:
        {
            jstring result = this->ExecuteCommand(arguments, &RemoteVM::NewStringUTF);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_STRING_UTF_LENGTH:
        {
            jsize result = this->ExecuteCommand(arguments, &RemoteVM::GetStringUTFLength);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_STRING_UTF_CHARS:
        {
            std::string result = this->ExecuteCommand(arguments, &RemoteVM::GetStringUTFChars);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_ARRAY_LENGTH:
        {
            jsize result = this->ExecuteCommand(arguments, &RemoteVM::GetArrayLength);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::NEW_OBJECT_ARRAY:
        {
            jobjectArray result = this->ExecuteCommand(arguments, &RemoteVM::NewObjectArray);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_OBJECT_ARRAY_ELEMENTS:
        {
            std::vector<jobject> result = this->ExecuteCommand(arguments, &RemoteVM::GetObjectArrayElements);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::SET_OBJECT_ARRAY_ELEMENTS:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetObjectArrayElements);
        }
            break;

        case RemoteVMCommand::NEW_BOOLEAN_ARRAY:
        {
            jbooleanArray result = this->ExecuteCommand(arguments, &RemoteVM::NewBooleanArray);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::NEW_BYTE_ARRAY:
        {
            jbyteArray result = this->ExecuteCommand(arguments, &RemoteVM::NewByteArray);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::NEW_CHAR_ARRAY:
        {
            jcharArray result = this->ExecuteCommand(arguments, &RemoteVM::NewCharArray);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::NEW_SHORT_ARRAY:
        {
            jshortArray result = this->ExecuteCommand(arguments, &RemoteVM::NewShortArray);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::NEW_INT_ARRAY:
        {
            jintArray result = this->ExecuteCommand(arguments, &RemoteVM::NewIntArray);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::NEW_LONG_ARRAY:
        {
            jlongArray result = this->ExecuteCommand(arguments, &RemoteVM::NewLongArray);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::NEW_FLOAT_ARRAY:
        {
            jfloatArray result = this->ExecuteCommand(arguments, &RemoteVM::NewFloatArray);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::NEW_DOUBLE_ARRAY:
        {
            jdoubleArray result = this->ExecuteCommand(arguments, &RemoteVM::NewDoubleArray);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_BOOLEAN_ARRAY_ELEMENTS:
        {
            std::vector<jboolean> result = this->ExecuteCommand(arguments, &RemoteVM::GetBooleanArrayElements);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_BYTE_ARRAY_ELEMENTS:
        {
            std::vector<jbyte> result = this->ExecuteCommand(arguments, &RemoteVM::GetByteArrayElements);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_CHAR_ARRAY_ELEMENTS:
        {
            std::vector<jchar> result = this->ExecuteCommand(arguments, &RemoteVM::GetCharArrayElements);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_SHORT_ARRAY_ELEMENTS:
        {
            std::vector<jshort> result = this->ExecuteCommand(arguments, &RemoteVM::GetShortArrayElements);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_INT_ARRAY_ELEMENTS:
        {
            std::vector<jint> result = this->ExecuteCommand(arguments, &RemoteVM::GetIntArrayElements);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_LONG_ARRAY_ELEMENTS:
        {
            std::vector<jlong> result = this->ExecuteCommand(arguments, &RemoteVM::GetLongArrayElements);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_FLOAT_ARRAY_ELEMENTS:
        {
            std::vector<jfloat> result = this->ExecuteCommand(arguments, &RemoteVM::GetFloatArrayElements);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_DOUBLE_ARRAY_ELEMENTS:
        {
            std::vector<jdouble> result = this->ExecuteCommand(arguments, &RemoteVM::GetDoubleArrayElements);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_BOOLEAN_ARRAY_REGION:
        {
            std::vector<jboolean> result = this->ExecuteCommand(arguments, &RemoteVM::GetBooleanArrayRegion);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_BYTE_ARRAY_REGION:
        {
            std::vector<jbyte> result = this->ExecuteCommand(arguments, &RemoteVM::GetByteArrayRegion);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_CHAR_ARRAY_REGION:
        {
            std::vector<jchar> result = this->ExecuteCommand(arguments, &RemoteVM::GetCharArrayRegion);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_SHORT_ARRAY_REGION:
        {
            std::vector<jshort> result = this->ExecuteCommand(arguments, &RemoteVM::GetShortArrayRegion);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_INT_ARRAY_REGION:
        {
            std::vector<jint> result = this->ExecuteCommand(arguments, &RemoteVM::GetIntArrayRegion);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_LONG_ARRAY_REGION:
        {
            std::vector<jlong> result = this->ExecuteCommand(arguments, &RemoteVM::GetLongArrayRegion);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_FLOAT_ARRAY_REGION:
        {
            std::vector<jfloat> result = this->ExecuteCommand(arguments, &RemoteVM::GetFloatArrayRegion);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::GET_DOUBLE_ARRAY_REGION:
        {
            std::vector<jdouble> result = this->ExecuteCommand(arguments, &RemoteVM::GetDoubleArrayRegion);
            Remote_VM_Write(response, result);
        }
            break;

        case RemoteVMCommand::SET_BOOLEAN_ARRAY_REGION:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetBooleanArrayRegion);
        }
            break;

        case RemoteVMCommand::SET_BYTE_ARRAY_REGION:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetByteArrayRegion);
        }
            break;

        case RemoteVMCommand::SET_CHAR_ARRAY_REGION:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetCharArrayRegion);
        }
            break;

        case RemoteVMCommand::SET_SHORT_ARRAY_REGION:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetShortArrayRegion);
        }
            break;

        case RemoteVMCommand::SET_INT_ARRAY_REGION:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetIntArrayRegion);
        }
            break;

        case RemoteVMCommand::SET_LONG_ARRAY_REGION:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetLongArrayRegion);
        }
            break;

        case RemoteVMCommand::SET_FLOAT_ARRAY_REGION:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetFloatArrayRegion);
        }
            break;

        case RemoteVMCommand::SET_DOUBLE_ARRAY_REGION:
        {
            this->ExecuteCommand(arguments, &RemoteVM::SetDoubleArrayRegion);
        }
            break;

        case RemoteVMCommand::MONITOR_ENTER:
        {
            jint result = this->ExecuteCommand(arguments, &RemoteVM::MonitorEnter);
            Remote_VM_Write(arguments, result);
        }
            break;

        case RemoteVMCommand::MONITOR_EXIT:
        {
            jint result = this->ExecuteCommand(arguments, &RemoteVM::MonitorExit);
            Remote_VM_Write(arguments, result);
        }
            break;

        case RemoteVMCommand::GET_JAVA_VM:
        {
            JavaVM* result = this->ExecuteCommand(arguments, &RemoteVM::GetJavaVM);
            Remote_VM_Write(arguments, result);
        }
            break;

        case RemoteVMCommand::NEW_DIRECT_BYTE_BUFFER:
        {
            jobject result = this->ExecuteCommand(arguments, &RemoteVM::NewDirectByteBuffer);
            Remote_VM_Write(arguments, result);
        }
            break;

        case RemoteVMCommand::GET_DIRECT_BUFFER_ADDRESS:
        {
            void* result = this->ExecuteCommand(arguments, &RemoteVM::GetDirectBufferAddress);
            Remote_VM_Write(arguments, result);
        }
            break;

        case RemoteVMCommand::GET_DIRECT_BUFFER_CAPACITY:
        {
            jlong result = this->ExecuteCommand(arguments, &RemoteVM::GetDirectBufferCapacity);
            Remote_VM_Write(arguments, result);
        }
            break;

        case RemoteVMCommand::GET_OBJECT_REF_TYPE:
        {
            jobjectRefType result = this->ExecuteCommand(arguments, &RemoteVM::GetObjectRefType);
            Remote_VM_Write(arguments, result);
        }
            break;
    }
}