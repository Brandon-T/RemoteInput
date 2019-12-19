#include "JVM.hxx"


JVM::JVM() : env(nullptr), vm(nullptr), createdVM(false), loadedJNI(false)
{
    #if defined(_WIN32) || defined(_WIN64)
    this->module = GetModuleHandleW(L"jvm.dll");
    if (!this->module)
    {
        loadedJNI = true;
        this->module = LoadLibraryW(L"jvm.dll");
    }
    #elif defined(__APPLE__)
    this->loadedJNI = true;
    this->module = dlopen("/Library/Internet Plug-Ins/JavaAppletPlugin.plugin/Contents/Home/lib/server/libjvm.dylib", RTLD_NOW);

    ///Library/Internet Plug-Ins/JavaAppletPlugin.plugin/Contents/Home/lib/server/libjvm.dylib
    ///Library/Java/JavaVirtualMachines/jdk1.8.0_66.jdk/Contents/Home/jre/lib/server/libjvm.dylib
    #else
    this->loadedJNI = true;
    this->module = dlopen("libjvm.so", RTLD_NOW);
    #endif // defined

    if (!this->module)
    {
        return;
    }


    this->createdVM = false;
    jint num_vms = 0;
    const jint max_vms = 1;
    JavaVM* vms[max_vms] = {0};
    if (this->JNI_GetCreatedJavaVMs(vms, max_vms, &num_vms) == JNI_OK)
    {
        for (int i = 0; i < num_vms; ++i)
        {
            if (vms[i] != NULL)
            {
                this->vm = vms[i];
                break;
            }
        }
    }
}

JVM::JVM(JNIEnv* env) : env(env), vm(nullptr), createdVM(false)
{
    this->env->GetJavaVM(&this->vm);
}

JVM::JVM(int argc, const char* argv[]) : env(nullptr), vm(nullptr), createdVM(false)
{
    if (argc)
    {
        Init(argc, argv);
    }
    else
    {
        const char* args[] = {"-Djava.compiler=NONE", "-Djava.class.path=."};
        Init(2, args);
    }
}

JVM::~JVM()
{
    if (this->vm)
    {
        if (this->env)
        {
            this->DetachCurrentThread();
        }

        if (this->createdVM)
        {
            jint (JNICALL *pDestroyJavaVM)() = nullptr;
            #if defined(_WIN32) || defined(_WIN64)
            pDestroyJavaVM = reinterpret_cast<decltype(pDestroyJavaVM)>(GetProcAddress(this->module, "DestroyJavaVM"));
            #else
            pDestroyJavaVM = reinterpret_cast<decltype(pDestroyJavaVM)>(dlsym(this->module, "DestroyJavaVM"));
            #endif // defined

            pDestroyJavaVM();
        }

        this->vm = nullptr;

        if (loadedJNI)
        {
            #if defined(_WIN32) || defined(_WIN64)
            CloseHandle(this->module);
            #else
            dlclose(this->module);
            #endif // defined
        }
        this->module = nullptr;
    }
}

bool JVM::Init(int argc, const char* argv[])
{
    #if defined(_WIN32) || defined(_WIN64)
    this->module = LoadLibraryW(L"jvm.dll"); //C:/Program Files/Java/jre1.8.0_73/bin/server/jvm.dll
    #else
    this->module = dlopen("libjvm.dylib", RTLD_LAZY); ///Library/Java/JavaVirtualMachines/jdk1.8.0_66.jdk/Contents/Home/jre/lib/server/libjvm.dylib
    #endif // defined

    if (!this->module)
    {
        return false;
    }

    bool result = false;
    this->createdVM = false;

    if (argc > 0)
    {
        JavaVMInitArgs jvm_args;
        JavaVMOption options[argc];

        for (int i = 0; i < argc; ++i)
        {
            options[i].optionString = const_cast<char*>(argv[i]);
        }

        JNI_GetDefaultJavaVMInitArgs(&jvm_args);

        jvm_args.version = JNI_VERSION_1_8;
        jvm_args.nOptions = argc;
        jvm_args.options = options;
        jvm_args.ignoreUnrecognized = false;

        result = true;

        jint num_vms = 0;
        const jint max_vms = 5;
        JavaVM* vms[max_vms] = {0};
        if (this->JNI_GetCreatedJavaVMs(vms, max_vms, &num_vms) == JNI_OK)
        {
            for (int i = 0; i < num_vms; ++i)
            {
                if (vms[i] != NULL)
                {
                    this->vm = vms[i];
                    this->AttachCurrentThread();
                    return result;
                }
            }
        }

        this->createdVM = true;
        if (this->JNI_CreateJavaVM(&this->vm, reinterpret_cast<void**>(&this->env), &jvm_args) != JNI_OK)
        {
            result = false;
            this->vm = nullptr;
            this->env = nullptr;
            this->createdVM = false;
        }
    }
    return result;
}

JVM::operator bool()
{
    return this->vm != nullptr;
}

jint JVM::AttachCurrentThread()
{
    return this->vm->AttachCurrentThread(reinterpret_cast<void**>(&this->env), nullptr);
}

jint JVM::AttachCurrentThreadAsDaemon()
{
	return this->vm->AttachCurrentThreadAsDaemon(reinterpret_cast<void**>(&this->env), nullptr);
}

jint JVM::DetachCurrentThread()
{
    this->env = nullptr;
    return this->vm->DetachCurrentThread();
}

jint JVM::GetEnv(void **penv, jint version)
{
    return this->vm->GetEnv(penv, version);
}

jint JVM::JNI_GetDefaultJavaVMInitArgs(void *args)
{
    jint (JNICALL *pJNI_GetDefaultJavaVMInitArgs)(void *args) = nullptr;
    #if defined(_WIN32) || defined(_WIN64)
    pJNI_GetDefaultJavaVMInitArgs = reinterpret_cast<decltype(pJNI_GetDefaultJavaVMInitArgs)>(GetProcAddress(this->module, "JNI_GetDefaultJavaVMInitArgs"));
    #else
    pJNI_GetDefaultJavaVMInitArgs = reinterpret_cast<decltype(pJNI_GetDefaultJavaVMInitArgs)>(dlsym(this->module, "JNI_GetDefaultJavaVMInitArgs"));
    #endif // defined
    return pJNI_GetDefaultJavaVMInitArgs(args);
}

jint JVM::JNI_CreateJavaVM(JavaVM **pvm, void **penv, void *args)
{
    jint (JNICALL *pJNI_CreateJavaVM)(JavaVM **pvm, void **penv, void *args) = nullptr;
    #if defined(_WIN32) || defined(_WIN64)
    pJNI_CreateJavaVM = reinterpret_cast<decltype(pJNI_CreateJavaVM)>(GetProcAddress(this->module, "JNI_CreateJavaVM"));
    #else
    pJNI_CreateJavaVM = reinterpret_cast<decltype(pJNI_CreateJavaVM)>(dlsym(this->module, "JNI_CreateJavaVM"));
    #endif // defined
    return pJNI_CreateJavaVM(pvm, penv, args);
}

jint JVM::JNI_GetCreatedJavaVMs(JavaVM **vm, jsize size, jsize *size2)
{
    jint (JNICALL *pJNI_GetCreatedJavaVMs)(JavaVM **, jsize, jsize *) = nullptr;
    #if defined(_WIN32) || defined(_WIN64)
    pJNI_GetCreatedJavaVMs = reinterpret_cast<decltype(pJNI_GetCreatedJavaVMs)>(GetProcAddress(this->module, "JNI_GetCreatedJavaVMs"));
    #else
    pJNI_GetCreatedJavaVMs = reinterpret_cast<decltype(pJNI_GetCreatedJavaVMs)>(dlsym(this->module, "JNI_GetCreatedJavaVMs"));
    #endif // defined
    return pJNI_GetCreatedJavaVMs(vm, size, size2);
}

jint JVM::GetVersion()
{
    return this->env->GetVersion();
}

jclass JVM::DefineClass(const char *name, jobject loader, const jbyte *buf, jsize len)
{
    return this->env->DefineClass(name, loader, buf, len);
}

jclass JVM::FindClass(const char *name)
{
    return this->env->FindClass(name);
}

jmethodID JVM::FromReflectedMethod(jobject method)
{
    return this->env->FromReflectedMethod(method);
}

jfieldID JVM::FromReflectedField(jobject field)
{
    return this->env->FromReflectedField(field);
}

jobject JVM::ToReflectedMethod(jclass cls, jmethodID methodID, jboolean isStatic)
{
    return this->env->ToReflectedMethod(cls, methodID, isStatic);
}

jclass JVM::GetSuperclass(jclass sub)
{
    return this->env->GetSuperclass(sub);
}
jboolean JVM::IsAssignableFrom(jclass sub, jclass sup)
{
    return this->env->IsAssignableFrom(sub, sup);
}

jobject JVM::ToReflectedField(jclass cls, jfieldID fieldID, jboolean isStatic)
{
    return this->env->ToReflectedField(cls, fieldID, isStatic);
}

jint JVM::Throw(jthrowable obj)
{
    return this->env->Throw(obj);
}

jint JVM::ThrowNew(jclass clazz, const char *msg)
{
    return this->env->ThrowNew(clazz, msg);
}

jthrowable JVM::ExceptionOccurred()
{
    return this->env->ExceptionOccurred();
}

void JVM::ExceptionDescribe()
{
    this->env->ExceptionDescribe();
}

void JVM::ExceptionClear()
{
    this->env->ExceptionClear();
}

void JVM::FatalError(const char *msg)
{
    this->env->FatalError(msg);
}

jint JVM::PushLocalFrame(jint capacity)
{
    return this->env->PushLocalFrame(capacity);
}

jobject JVM::PopLocalFrame(jobject result)
{
    return this->env->PopLocalFrame(result);
}

jobject JVM::NewGlobalRef(jobject lobj)
{
    return this->env->NewGlobalRef(lobj);
}

void JVM::DeleteGlobalRef(jobject gref)
{
    this->env->DeleteGlobalRef(gref);
}

void JVM::DeleteLocalRef(jobject obj)
{
    this->env->DeleteLocalRef(obj);
}

jboolean JVM::IsSameObject(jobject obj1, jobject obj2)
{
    return this->env->IsSameObject(obj1, obj2);
}

jobject JVM::NewLocalRef(jobject ref)
{
    return this->env->NewLocalRef(ref);
}

jint JVM::EnsureLocalCapacity(jint capacity)
{
    return this->env->EnsureLocalCapacity(capacity);
}

jobject JVM::AllocObject(jclass clazz)
{
    return this->env->AllocObject(clazz);
}

jobject JVM::NewObject(jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jobject result = this->env->NewObjectV(clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jobject JVM::NewObjectV(jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->NewObjectV(clazz, methodID, args);
}

jobject JVM::NewObjectA(jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->NewObjectA(clazz, methodID, args);
}

jclass JVM::GetObjectClass(jobject obj)
{
    return this->env->GetObjectClass(obj);
}

jboolean JVM::IsInstanceOf(jobject obj, jclass clazz)
{
    return this->env->IsInstanceOf(obj, clazz);
}

jmethodID JVM::GetMethodID(jclass clazz, const char *name, const char *sig)
{
    return this->env->GetMethodID(clazz, name, sig);
}

jobject JVM::CallObjectMethod(jobject obj, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jobject result = this->env->CallObjectMethodV(obj, methodID, vlist);
    va_end(vlist);
    return result;
}

jobject JVM::CallObjectMethodV(jobject obj, jmethodID methodID, va_list args)
{
    return this->env->CallObjectMethodV(obj, methodID, args);
}

jobject JVM::CallObjectMethodA(jobject obj, jmethodID methodID, const jvalue *args)
{
    return this->env->CallObjectMethodA(obj, methodID, args);
}

jboolean JVM::CallBooleanMethod(jobject obj, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jboolean result = this->env->CallBooleanMethodV(obj, methodID, vlist);
    va_end(vlist);
    return result;
}

jboolean JVM::CallBooleanMethodV(jobject obj, jmethodID methodID, va_list args)
{
    return this->env->CallBooleanMethodV(obj, methodID, args);
}

jboolean JVM::CallBooleanMethodA(jobject obj, jmethodID methodID, const jvalue *args)
{
    return this->env->CallBooleanMethodA(obj, methodID, args);
}

jbyte JVM::CallByteMethod(jobject obj, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jbyte result = this->env->CallByteMethodV(obj, methodID, vlist);
    va_end(vlist);
    return result;
}

jbyte JVM::CallByteMethodV(jobject obj, jmethodID methodID, va_list args)
{
    return this->env->CallByteMethodV(obj, methodID, args);
}

jbyte JVM::CallByteMethodA(jobject obj, jmethodID methodID, const jvalue *args)
{
    return this->env->CallByteMethodA(obj, methodID, args);
}

jchar JVM::CallCharMethod(jobject obj, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jchar result = this->env->CallCharMethodV(obj, methodID, vlist);
    va_end(vlist);
    return result;
}

jchar JVM::CallCharMethodV(jobject obj, jmethodID methodID, va_list args)
{
    return this->env->CallCharMethod(obj, methodID, args);
}

jchar JVM::CallCharMethodA(jobject obj, jmethodID methodID, const jvalue *args)
{
    return this->env->CallCharMethodA(obj, methodID, args);
}

jshort JVM::CallShortMethod(jobject obj, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jshort result = this->env->CallShortMethod(obj, methodID, vlist);
    va_end(vlist);
    return result;
}

jshort JVM::CallShortMethodV(jobject obj, jmethodID methodID, va_list args)
{
    return this->env->CallShortMethodV(obj, methodID, args);
}

jshort JVM::CallShortMethodA(jobject obj, jmethodID methodID, const jvalue *args)
{
    return this->env->CallShortMethodA(obj, methodID, args);
}

jint JVM::CallIntMethod(jobject obj, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jint result = this->env->CallIntMethodV(obj, methodID, vlist);
    va_end(vlist);
    return result;
}

jint JVM::CallIntMethodV(jobject obj, jmethodID methodID, va_list args)
{
    return this->env->CallIntMethodV(obj, methodID, args);
}

jint JVM::CallIntMethodA(jobject obj, jmethodID methodID, const jvalue *args)
{
    return this->env->CallIntMethodA(obj, methodID, args);
}

jlong JVM::CallLongMethod(jobject obj, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jlong result = this->env->CallLongMethodV(obj, methodID, vlist);
    va_end(vlist);
    return result;
}

jlong JVM::CallLongMethodV(jobject obj, jmethodID methodID, va_list args)
{
    return this->env->CallLongMethodV(obj, methodID, args);
}

jlong JVM::CallLongMethodA(jobject obj, jmethodID methodID, const jvalue *args)
{
    return this->env->CallLongMethodA(obj, methodID, args);
}

jfloat JVM::CallFloatMethod(jobject obj, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jfloat result = this->env->CallFloatMethodV(obj, methodID, vlist);
    va_end(vlist);
    return result;
}

jfloat JVM::CallFloatMethodV(jobject obj, jmethodID methodID, va_list args)
{
    return this->env->CallFloatMethodV(obj, methodID, args);
}

jfloat JVM::CallFloatMethodA(jobject obj, jmethodID methodID, const jvalue *args)
{
    return this->env->CallFloatMethodA(obj, methodID, args);
}

jdouble JVM::CallDoubleMethod(jobject obj, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jdouble result = this->env->CallDoubleMethodV(obj, methodID, vlist);
    va_end(vlist);
    return result;
}

jdouble JVM::CallDoubleMethodV(jobject obj, jmethodID methodID, va_list args)
{
    return this->env->CallDoubleMethodV(obj, methodID, args);
}

jdouble JVM::CallDoubleMethodA(jobject obj, jmethodID methodID, const jvalue *args)
{
    return this->env->CallDoubleMethodA(obj, methodID, args);
}

void JVM::CallVoidMethod(jobject obj, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    this->env->CallVoidMethodV(obj, methodID, vlist);
    va_end(vlist);
}

void JVM::CallVoidMethodV(jobject obj, jmethodID methodID, va_list args)
{
    return this->env->CallVoidMethodV(obj, methodID, args);
}

void JVM::CallVoidMethodA(jobject obj, jmethodID methodID, const jvalue *args)
{
    this->env->CallVoidMethodA(obj, methodID, args);
}

jobject JVM::CallNonvirtualObjectMethod(jobject obj, jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jobject result = this->env->CallNonvirtualObjectMethodV(obj, clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jobject JVM::CallNonvirtualObjectMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallNonvirtualObjectMethodV(obj, clazz, methodID, args);
}

jobject JVM::CallNonvirtualObjectMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallNonvirtualObjectMethodA(obj, clazz, methodID, args);
}

jboolean JVM::CallNonvirtualBooleanMethod(jobject obj, jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jboolean result = this->env->CallNonvirtualBooleanMethodV(obj, clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jboolean JVM::CallNonvirtualBooleanMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallNonvirtualBooleanMethodV(obj, clazz, methodID, args);
}

jboolean JVM::CallNonvirtualBooleanMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallNonvirtualBooleanMethodA(obj, clazz, methodID, args);
}

jbyte JVM::CallNonvirtualByteMethod(jobject obj, jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jbyte result = this->env->CallNonvirtualByteMethodV(obj, clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jbyte JVM::CallNonvirtualByteMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallNonvirtualByteMethodV(obj, clazz, methodID, args);
}

jbyte JVM::CallNonvirtualByteMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallNonvirtualByteMethodA(obj, clazz, methodID, args);
}

jchar JVM::CallNonvirtualCharMethod(jobject obj, jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jchar result = this->env->CallNonvirtualCharMethodV(obj, clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jchar JVM::CallNonvirtualCharMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallNonvirtualCharMethodV(obj, clazz, methodID, args);
}

jchar JVM::CallNonvirtualCharMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallNonvirtualCharMethodA(obj, clazz, methodID, args);
}

jshort JVM::CallNonvirtualShortMethod(jobject obj, jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jshort result = this->env->CallNonvirtualShortMethodV(obj, clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jshort JVM::CallNonvirtualShortMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallNonvirtualShortMethodV(obj, clazz, methodID, args);
}

jshort JVM::CallNonvirtualShortMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallNonvirtualShortMethodA(obj, clazz, methodID, args);
}

jint JVM::CallNonvirtualIntMethod(jobject obj, jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jint result = this->env->CallNonvirtualIntMethodV(obj, clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jint JVM::CallNonvirtualIntMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallNonvirtualIntMethodV(obj, clazz, methodID, args);
}

jint JVM::CallNonvirtualIntMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallNonvirtualIntMethodA(obj, clazz, methodID, args);
}

jlong JVM::CallNonvirtualLongMethod(jobject obj, jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jlong result = this->env->CallNonvirtualLongMethodV(obj, clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jlong JVM::CallNonvirtualLongMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallNonvirtualLongMethodV(obj, clazz, methodID, args);
}

jlong JVM::CallNonvirtualLongMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallNonvirtualLongMethodA(obj, clazz, methodID, args);
}

jfloat JVM::CallNonvirtualFloatMethod(jobject obj, jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jfloat result = this->env->CallNonvirtualFloatMethodV(obj, clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jfloat JVM::CallNonvirtualFloatMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallNonvirtualFloatMethodV(obj, clazz, methodID, args);
}

jfloat JVM::CallNonvirtualFloatMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallNonvirtualFloatMethodA(obj, clazz, methodID, args);
}

jdouble JVM::CallNonvirtualDoubleMethod(jobject obj, jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jdouble result = this->env->CallNonvirtualDoubleMethodV(obj, clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jdouble JVM::CallNonvirtualDoubleMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallNonvirtualDoubleMethodV(obj, clazz, methodID, args);
}

jdouble JVM::CallNonvirtualDoubleMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallNonvirtualDoubleMethodA(obj, clazz, methodID, args);
}

void JVM::CallNonvirtualVoidMethod(jobject obj, jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    this->env->CallNonvirtualVoidMethodV(obj, clazz, methodID, vlist);
    va_end(vlist);
}

void JVM::CallNonvirtualVoidMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallNonvirtualVoidMethodV(obj, clazz, methodID, args);
}

void JVM::CallNonvirtualVoidMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args)
{
    this->env->CallNonvirtualVoidMethodA(obj, clazz, methodID, args);
}

jfieldID JVM::GetFieldID(jclass clazz, const char *name, const char *sig)
{
    return this->env->GetFieldID(clazz, name, sig);
}

jobject JVM::GetObjectField(jobject obj, jfieldID fieldID)
{
    return this->env->GetObjectField(obj, fieldID);
}

jboolean JVM::GetBooleanField(jobject obj, jfieldID fieldID)
{
    return this->env->GetBooleanField(obj, fieldID);
}

jbyte JVM::GetByteField(jobject obj, jfieldID fieldID)
{
    return this->env->GetByteField(obj, fieldID);
}

jchar JVM::GetCharField(jobject obj, jfieldID fieldID)
{
    return this->env->GetCharField(obj, fieldID);
}

jshort JVM::GetShortField(jobject obj, jfieldID fieldID)
{
    return this->env->GetShortField(obj, fieldID);
}

jint JVM::GetIntField(jobject obj, jfieldID fieldID)
{
    return this->env->GetIntField(obj, fieldID);
}

jlong JVM::GetLongField(jobject obj, jfieldID fieldID)
{
    return this->env->GetLongField(obj, fieldID);
}

jfloat JVM::GetFloatField(jobject obj, jfieldID fieldID)
{
    return this->env->GetFloatField(obj, fieldID);
}

jdouble JVM::GetDoubleField(jobject obj, jfieldID fieldID)
{
    return this->env->GetDoubleField(obj, fieldID);
}

void JVM::SetObjectField(jobject obj, jfieldID fieldID, jobject val)
{
    this->env->SetObjectField(obj, fieldID, val);
}

void JVM::SetBooleanField(jobject obj, jfieldID fieldID, jboolean val)
{
    this->env->SetBooleanField(obj, fieldID, val);
}

void JVM::SetByteField(jobject obj, jfieldID fieldID, jbyte val)
{
    this->env->SetByteField(obj, fieldID, val);
}

void JVM::SetCharField(jobject obj, jfieldID fieldID, jchar val)
{
    this->env->SetCharField(obj, fieldID, val);
}

void JVM::SetShortField(jobject obj, jfieldID fieldID, jshort val)
{
    this->env->SetShortField(obj, fieldID, val);
}

void JVM::SetIntField(jobject obj, jfieldID fieldID, jint val)
{
    this->env->SetIntField(obj, fieldID, val);
}

void JVM::SetLongField(jobject obj, jfieldID fieldID, jlong val)
{
    this->env->SetLongField(obj, fieldID, val);
}

void JVM::SetFloatField(jobject obj, jfieldID fieldID, jfloat val)
{
    this->env->SetFloatField(obj, fieldID, val);
}

void JVM::SetDoubleField(jobject obj, jfieldID fieldID, jdouble val)
{
    this->env->SetDoubleField(obj, fieldID, val);
}

jmethodID JVM::GetStaticMethodID(jclass clazz, const char *name, const char *sig)
{
    return this->env->GetStaticMethodID(clazz, name, sig);
}

jobject JVM::CallStaticObjectMethod(jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jobject result = this->env->CallStaticObjectMethodV(clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jobject JVM::CallStaticObjectMethodV(jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallStaticObjectMethodV(clazz, methodID, args);
}

jobject JVM::CallStaticObjectMethodA(jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallStaticObjectMethodA(clazz, methodID, args);
}

jboolean JVM::CallStaticBooleanMethod(jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jboolean result = this->env->CallStaticBooleanMethodV(clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jboolean JVM::CallStaticBooleanMethodV(jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallStaticBooleanMethodV(clazz, methodID, args);
}

jboolean JVM::CallStaticBooleanMethodA(jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallStaticBooleanMethodA(clazz, methodID, args);
}

jbyte JVM::CallStaticByteMethod(jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jbyte result = this->env->CallStaticByteMethodV(clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jbyte JVM::CallStaticByteMethodV(jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallStaticByteMethodV(clazz, methodID, args);
}

jbyte JVM::CallStaticByteMethodA(jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallStaticByteMethodA(clazz, methodID, args);
}

jchar JVM::CallStaticCharMethod(jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jchar result = this->env->CallStaticCharMethodV(clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jchar JVM::CallStaticCharMethodV(jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallStaticCharMethodV(clazz, methodID, args);
}

jchar JVM::CallStaticCharMethodA(jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallStaticCharMethodA(clazz, methodID, args);
}

jshort JVM::CallStaticShortMethod(jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jshort result = this->env->CallStaticShortMethodV(clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jshort JVM::CallStaticShortMethodV(jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallStaticShortMethodV(clazz, methodID, args);
}

jshort JVM::CallStaticShortMethodA(jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallStaticShortMethodA(clazz, methodID, args);
}

jint JVM::CallStaticIntMethod(jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jint result = this->env->CallStaticIntMethodV(clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jint JVM::CallStaticIntMethodV(jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallStaticIntMethodV(clazz, methodID, args);
}

jint JVM::CallStaticIntMethodA(jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallStaticIntMethodA(clazz, methodID, args);
}

jlong JVM::CallStaticLongMethod(jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jlong result = this->env->CallStaticLongMethodV(clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jlong JVM::CallStaticLongMethodV(jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallStaticLongMethodV(clazz, methodID, args);
}

jlong JVM::CallStaticLongMethodA(jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallStaticLongMethodA(clazz, methodID, args);
}

jfloat JVM::CallStaticFloatMethod(jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jfloat result = this->env->CallStaticFloatMethodV(clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jfloat JVM::CallStaticFloatMethodV(jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallStaticFloatMethodV(clazz, methodID, args);
}

jfloat JVM::CallStaticFloatMethodA(jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallStaticFloatMethodA(clazz, methodID, args);
}

jdouble JVM::CallStaticDoubleMethod(jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    jdouble result = this->env->CallStaticDoubleMethodV(clazz, methodID, vlist);
    va_end(vlist);
    return result;
}

jdouble JVM::CallStaticDoubleMethodV(jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallStaticDoubleMethodV(clazz, methodID, args);
}

jdouble JVM::CallStaticDoubleMethodA(jclass clazz, jmethodID methodID, const jvalue *args)
{
    return this->env->CallStaticDoubleMethodA(clazz, methodID, args);
}

void JVM::CallStaticVoidMethod(jclass clazz, jmethodID methodID, ...)
{
    va_list vlist;
    va_start(vlist, methodID);
    this->env->CallStaticVoidMethodV(clazz, methodID, vlist);
    va_end(vlist);
}

void JVM::CallStaticVoidMethodV(jclass clazz, jmethodID methodID, va_list args)
{
    return this->env->CallStaticVoidMethodV(clazz, methodID, args);
}

void JVM::CallStaticVoidMethodA(jclass cls, jmethodID methodID, const jvalue *args)
{
    this->env->CallStaticVoidMethodA(cls, methodID, args);
}

jfieldID JVM::GetStaticFieldID(jclass clazz, const char *name, const char *sig)
{
    return this->env->GetStaticFieldID(clazz, name, sig);
}

jobject JVM::GetStaticObjectField(jclass clazz, jfieldID fieldID)
{
    return this->env->GetStaticObjectField(clazz, fieldID);
}

jboolean JVM::GetStaticBooleanField(jclass clazz, jfieldID fieldID)
{
    return this->env->GetStaticBooleanField(clazz, fieldID);
}

jbyte JVM::GetStaticByteField(jclass clazz, jfieldID fieldID)
{
    return this->env->GetStaticByteField(clazz, fieldID);
}

jchar JVM::GetStaticCharField(jclass clazz, jfieldID fieldID)
{
    return this->env->GetStaticCharField(clazz, fieldID);
}

jshort JVM::GetStaticShortField(jclass clazz, jfieldID fieldID)
{
    return this->env->GetStaticShortField(clazz, fieldID);
}

jint JVM::GetStaticIntField(jclass clazz, jfieldID fieldID)
{
    return this->env->GetStaticIntField(clazz, fieldID);
}

jlong JVM::GetStaticLongField(jclass clazz, jfieldID fieldID)
{
    return this->env->GetStaticLongField(clazz, fieldID);
}

jfloat JVM::GetStaticFloatField(jclass clazz, jfieldID fieldID)
{
    return this->env->GetStaticFloatField(clazz, fieldID);
}

jdouble JVM::GetStaticDoubleField(jclass clazz, jfieldID fieldID)
{
    return this->env->GetStaticDoubleField(clazz, fieldID);
}

void JVM::SetStaticObjectField(jclass clazz, jfieldID fieldID, jobject value)
{
    this->env->SetStaticObjectField(clazz, fieldID, value);
}

void JVM::SetStaticBooleanField(jclass clazz, jfieldID fieldID, jboolean value)
{
    this->env->SetStaticBooleanField(clazz, fieldID, value);
}

void JVM::SetStaticByteField(jclass clazz, jfieldID fieldID, jbyte value)
{
    this->env->SetStaticByteField(clazz, fieldID, value);
}

void JVM::SetStaticCharField(jclass clazz, jfieldID fieldID, jchar value)
{
    this->env->SetStaticCharField(clazz, fieldID, value);
}

void JVM::SetStaticShortField(jclass clazz, jfieldID fieldID, jshort value)
{
    this->env->SetStaticShortField(clazz, fieldID, value);
}

void JVM::SetStaticIntField(jclass clazz, jfieldID fieldID, jint value)
{
    this->env->SetStaticIntField(clazz, fieldID, value);
}

void JVM::SetStaticLongField(jclass clazz, jfieldID fieldID, jlong value)
{
    this->env->SetStaticLongField(clazz, fieldID, value);
}

void JVM::SetStaticFloatField(jclass clazz, jfieldID fieldID, jfloat value)
{
    this->env->SetStaticFloatField(clazz, fieldID, value);
}

void JVM::SetStaticDoubleField(jclass clazz, jfieldID fieldID, jdouble value)
{
    this->env->SetStaticDoubleField(clazz, fieldID, value);
}

jstring JVM::NewString(const jchar *unicode, jsize len)
{
    return this->env->NewString(unicode, len);
}

jsize JVM::GetStringLength(jstring str)
{
    return this->env->GetStringLength(str);
}

const jchar *JVM::GetStringChars(jstring str, jboolean *isCopy)
{
    return this->env->GetStringChars(str, isCopy);
}

void JVM::ReleaseStringChars(jstring str, const jchar *chars)
{
    this->env->ReleaseStringChars(str, chars);
}

jstring JVM::NewStringUTF(const char *utf)
{
    return this->env->NewStringUTF(utf);
}

jsize JVM::GetStringUTFLength(jstring str)
{
    return this->env->GetStringUTFLength(str);
}

const char* JVM::GetStringUTFChars(jstring str, jboolean *isCopy)
{
    return this->env->GetStringUTFChars(str, isCopy);
}

void JVM::ReleaseStringUTFChars(jstring str, const char* chars)
{
    this->env->ReleaseStringUTFChars(str, chars);
}


jsize JVM::GetArrayLength(jarray array)
{
    return this->env->GetArrayLength(array);
}

jobjectArray JVM::NewObjectArray(jsize len, jclass clazz, jobject init)
{
    return this->env->NewObjectArray(len, clazz, init);
}

jobject JVM::GetObjectArrayElement(jobjectArray array, jsize index)
{
    return this->env->GetObjectArrayElement(array, index);
}

void JVM::SetObjectArrayElement(jobjectArray array, jsize index, jobject val)
{
    this->env->SetObjectArrayElement(array, index, val);
}

jbooleanArray JVM::NewBooleanArray(jsize len)
{
    return this->env->NewBooleanArray(len);
}

jbyteArray JVM::NewByteArray(jsize len)
{
    return this->env->NewByteArray(len);
}

jcharArray JVM::NewCharArray(jsize len)
{
    return this->env->NewCharArray(len);
}

jshortArray JVM::NewShortArray(jsize len)
{
    return this->env->NewShortArray(len);
}

jintArray JVM::NewIntArray(jsize len)
{
    return this->env->NewIntArray(len);
}

jlongArray JVM::NewLongArray(jsize len)
{
    return this->env->NewLongArray(len);
}

jfloatArray JVM::NewFloatArray(jsize len)
{
    return this->env->NewFloatArray(len);
}

jdoubleArray JVM::NewDoubleArray(jsize len)
{
    return this->env->NewDoubleArray(len);
}

jboolean* JVM::GetBooleanArrayElements(jbooleanArray array, jboolean *isCopy)
{
    return this->env->GetBooleanArrayElements(array, isCopy);
}

jbyte* JVM::GetByteArrayElements(jbyteArray array, jboolean *isCopy)
{
    return this->env->GetByteArrayElements(array, isCopy);
}

jchar* JVM::GetCharArrayElements(jcharArray array, jboolean *isCopy)
{
    return this->env->GetCharArrayElements(array, isCopy);
}

jshort* JVM::GetShortArrayElements(jshortArray array, jboolean *isCopy)
{
    return this->env->GetShortArrayElements(array, isCopy);
}

jint* JVM::GetIntArrayElements(jintArray array, jboolean *isCopy)
{
    return this->env->GetIntArrayElements(array, isCopy);
}

jlong* JVM::GetLongArrayElements(jlongArray array, jboolean *isCopy)
{
    return this->env->GetLongArrayElements(array, isCopy);
}

jfloat* JVM::GetFloatArrayElements(jfloatArray array, jboolean *isCopy)
{
    return this->env->GetFloatArrayElements(array, isCopy);
}

jdouble* JVM::GetDoubleArrayElements(jdoubleArray array, jboolean *isCopy)
{
    return this->env->GetDoubleArrayElements(array, isCopy);
}

void JVM::ReleaseBooleanArrayElements(jbooleanArray array, jboolean *elems, jint mode)
{
    this->env->ReleaseBooleanArrayElements(array, elems, mode);
}

void JVM::ReleaseByteArrayElements(jbyteArray array, jbyte *elems, jint mode)
{
    this->env->ReleaseByteArrayElements(array, elems, mode);
}

void JVM::ReleaseCharArrayElements(jcharArray array, jchar *elems, jint mode)
{
    this->env->ReleaseCharArrayElements(array, elems, mode);
}

void JVM::ReleaseShortArrayElements(jshortArray array, jshort *elems, jint mode)
{
    this->env->ReleaseShortArrayElements(array, elems, mode);
}

void JVM::ReleaseIntArrayElements(jintArray array, jint *elems, jint mode)
{
    this->env->ReleaseIntArrayElements(array, elems, mode);
}

void JVM::ReleaseLongArrayElements(jlongArray array, jlong *elems, jint mode)
{
    this->env->ReleaseLongArrayElements(array, elems, mode);
}

void JVM::ReleaseFloatArrayElements(jfloatArray array, jfloat *elems, jint mode)
{
    this->env->ReleaseFloatArrayElements(array, elems, mode);
}

void JVM::ReleaseDoubleArrayElements(jdoubleArray array, jdouble *elems, jint mode)
{
    this->env->ReleaseDoubleArrayElements(array, elems, mode);
}

void JVM::GetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len, jboolean *buf)
{
    this->env->GetBooleanArrayRegion(array, start, len, buf);
}

void JVM::GetByteArrayRegion(jbyteArray array, jsize start, jsize len, jbyte *buf)
{
    this->env->GetByteArrayRegion(array, start, len, buf);
}

void JVM::GetCharArrayRegion(jcharArray array, jsize start, jsize len, jchar *buf)
{
    this->env->GetCharArrayRegion(array, start, len, buf);
}

void JVM::GetShortArrayRegion(jshortArray array, jsize start, jsize len, jshort *buf)
{
    this->env->GetShortArrayRegion(array, start, len, buf);
}

void JVM::GetIntArrayRegion(jintArray array, jsize start, jsize len, jint *buf)
{
    this->env->GetIntArrayRegion(array, start, len, buf);
}

void JVM::GetLongArrayRegion(jlongArray array, jsize start, jsize len, jlong *buf)
{
    this->env->GetLongArrayRegion(array, start, len, buf);
}

void JVM::GetFloatArrayRegion(jfloatArray array, jsize start, jsize len, jfloat *buf)
{
    this->env->GetFloatArrayRegion(array, start, len, buf);
}

void JVM::GetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len, jdouble *buf)
{
    this->env->GetDoubleArrayRegion(array, start, len, buf);
}

void JVM::SetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len, const jboolean *buf)
{
    this->env->SetBooleanArrayRegion(array, start, len, buf);
}

void JVM::SetByteArrayRegion(jbyteArray array, jsize start, jsize len, const jbyte *buf)
{
    this->env->SetByteArrayRegion(array, start, len, buf);
}

void JVM::SetCharArrayRegion(jcharArray array, jsize start, jsize len, const jchar *buf)
{
    this->env->SetCharArrayRegion(array, start, len, buf);
}

void JVM::SetShortArrayRegion(jshortArray array, jsize start, jsize len, const jshort *buf)
{
    this->env->SetShortArrayRegion(array, start, len, buf);
}

void JVM::SetIntArrayRegion(jintArray array, jsize start, jsize len, const jint *buf)
{
    this->env->SetIntArrayRegion(array, start, len, buf);
}

void JVM::SetLongArrayRegion(jlongArray array, jsize start, jsize len, const jlong *buf)
{
    this->env->SetLongArrayRegion(array, start, len, buf);
}

void JVM::SetFloatArrayRegion(jfloatArray array, jsize start, jsize len, const jfloat *buf)
{
    this->env->SetFloatArrayRegion(array, start, len, buf);
}

void JVM::SetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len, const jdouble *buf)
{
    this->env->SetDoubleArrayRegion(array, start, len, buf);
}

jint JVM::RegisterNatives(jclass clazz, const JNINativeMethod *methods, jint nMethods)
{
    return this->env->RegisterNatives(clazz, methods, nMethods);
}

jint JVM::UnregisterNatives(jclass clazz)
{
    return this->env->UnregisterNatives(clazz);
}

jint JVM::MonitorEnter(jobject obj)
{
    return this->env->MonitorEnter(obj);
}

jint JVM::MonitorExit(jobject obj)
{
    return this->env->MonitorExit(obj);
}

jint JVM::GetJavaVM(JavaVM **vm)
{
    return this->env->GetJavaVM(vm);
}

void JVM::GetStringRegion(jstring str, jsize start, jsize len, jchar *buf)
{
    this->env->GetStringRegion(str, start, len, buf);
}

void JVM::GetStringUTFRegion(jstring str, jsize start, jsize len, char *buf)
{
    this->env->GetStringUTFRegion(str, start, len, buf);
}

void* JVM::GetPrimitiveArrayCritical(jarray array, jboolean *isCopy)
{
    return this->env->GetPrimitiveArrayCritical(array, isCopy);
}

void JVM::ReleasePrimitiveArrayCritical(jarray array, void *carray, jint mode)
{
    this->env->ReleasePrimitiveArrayCritical(array, carray, mode);
}

const jchar* JVM::GetStringCritical(jstring string, jboolean *isCopy)
{
    return this->env->GetStringCritical(string, isCopy);
}

void JVM::ReleaseStringCritical(jstring string, const jchar *cstring)
{
    this->env->ReleaseStringCritical(string, cstring);
}

jweak JVM::NewWeakGlobalRef(jobject obj)
{
    return this->env->NewWeakGlobalRef(obj);
}

void JVM::DeleteWeakGlobalRef(jweak ref)
{
    this->env->DeleteWeakGlobalRef(ref);
}

jboolean JVM::ExceptionCheck()
{
    return this->env->ExceptionCheck();
}

jobject JVM::NewDirectByteBuffer(void* address, jlong capacity)
{
    return this->env->NewDirectByteBuffer(address, capacity);
}

void* JVM::GetDirectBufferAddress(jobject buf)
{
    return this->env->GetDirectBufferAddress(buf);
}

jlong JVM::GetDirectBufferCapacity(jobject buf)
{
    return this->env->GetDirectBufferCapacity(buf);
}

jobjectRefType JVM::GetObjectRefType(jobject obj)
{
    return this->env->GetObjectRefType(obj);
}
