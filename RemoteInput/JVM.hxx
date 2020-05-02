#ifndef JVM_HXX_INCLUDED
#define JVM_HXX_INCLUDED

#include <jni.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <dlfcn.h>
#endif // defined

#include <stdexcept>
#include <vector>
#include <string>

class JVM
{
private:
    JNIEnv* env;
    JavaVM* vm;
    bool createdVM;
    bool loadedJNI;
    #if defined(_WIN32) || defined(_WIN64)
    HMODULE module;
    #else
    void* module;
    #endif // defined

    bool Init(int argc, const char* argv[]) noexcept;
    jint JNI_GetDefaultJavaVMInitArgs(void *args) const noexcept;
    jint JNI_CreateJavaVM(JavaVM **pvm, void **penv, void *args) const noexcept;
    jint JNI_GetCreatedJavaVMs(JavaVM **, jsize, jsize *) const noexcept;


public:
    JVM() noexcept;
    JVM(JNIEnv* env) noexcept;
    JVM(int argc, const char* argv[]) noexcept;
    ~JVM() noexcept;

    operator bool() const noexcept;
    JavaVM* getVM() const noexcept;
    JNIEnv* getENV() const noexcept;


    jint AttachCurrentThread() noexcept;
	jint AttachCurrentThreadAsDaemon() noexcept;
    jint DetachCurrentThread() noexcept;
    jint GetEnv(void **penv, jint version) const noexcept;

    jint GetVersion() const noexcept;
    jclass DefineClass(const char *name, jobject loader, const jbyte *buf, jsize len) const noexcept;
    jclass FindClass(const char *name) const noexcept;
    jmethodID FromReflectedMethod(jobject method) const noexcept;
    jfieldID FromReflectedField(jobject field) const noexcept;
    jobject ToReflectedMethod(jclass cls, jmethodID methodID, jboolean isStatic) const noexcept;
    jclass GetSuperclass(jclass sub) const noexcept;
    jboolean IsAssignableFrom(jclass sub, jclass sup) const noexcept;
    jobject ToReflectedField(jclass cls, jfieldID fieldID, jboolean isStatic) const noexcept;
    jint Throw(jthrowable obj) const noexcept;
    jint ThrowNew(jclass clazz, const char *msg) const noexcept;
    jthrowable ExceptionOccurred() const noexcept;
    void ExceptionDescribe() const noexcept;
    void ExceptionClear() const noexcept;
    void FatalError(const char *msg) const noexcept;
    jint PushLocalFrame(jint capacity) const noexcept;
    jobject PopLocalFrame(jobject result) const noexcept;
    jobject NewGlobalRef(jobject lobj) const noexcept;
    void DeleteGlobalRef(jobject gref) const noexcept;
    void DeleteLocalRef(jobject obj) const noexcept;
    jboolean IsSameObject(jobject obj1, jobject obj2) const noexcept;
    jobject NewLocalRef(jobject ref) const noexcept;
    jint EnsureLocalCapacity(jint capacity) const noexcept;
    jobject AllocObject(jclass clazz) const noexcept;
    jobject NewObject(jclass clazz, jmethodID methodID, ...) const noexcept;
    jobject NewObjectV(jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jobject NewObjectA(jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jclass GetObjectClass(jobject obj) const noexcept;
    jboolean IsInstanceOf(jobject obj, jclass clazz) const noexcept;
    jmethodID GetMethodID(jclass clazz, const char *name, const char *sig) const noexcept;
    jobject CallObjectMethod(jobject obj, jmethodID methodID, ...) const noexcept;
    jobject CallObjectMethodV(jobject obj, jmethodID methodID, va_list args) const noexcept;
    jobject CallObjectMethodA(jobject obj, jmethodID methodID, const jvalue *args) const noexcept;
    jboolean CallBooleanMethod(jobject obj, jmethodID methodID, ...) const noexcept;
    jboolean CallBooleanMethodV(jobject obj, jmethodID methodID, va_list args) const noexcept;
    jboolean CallBooleanMethodA(jobject obj, jmethodID methodID, const jvalue *args) const noexcept;
    jbyte CallByteMethod(jobject obj, jmethodID methodID, ...) const noexcept;
    jbyte CallByteMethodV(jobject obj, jmethodID methodID, va_list args) const noexcept;
    jbyte CallByteMethodA(jobject obj, jmethodID methodID, const jvalue *args) const noexcept;
    jchar CallCharMethod(jobject obj, jmethodID methodID, ...) const noexcept;
    jchar CallCharMethodV(jobject obj, jmethodID methodID, va_list) const noexcept;
    jchar CallCharMethodA(jobject obj, jmethodID methodID, const jvalue *args) const noexcept;
    jshort CallShortMethod(jobject obj, jmethodID methodID, ...) const noexcept;
    jshort CallShortMethodV(jobject obj, jmethodID methodID, va_list args) const noexcept;
    jshort CallShortMethodA(jobject obj, jmethodID methodID, const jvalue *args) const noexcept;
    jint CallIntMethod(jobject obj, jmethodID methodID, ...) const noexcept;
    jint CallIntMethodV(jobject obj, jmethodID methodID, va_list args) const noexcept;
    jint CallIntMethodA(jobject obj, jmethodID methodID, const jvalue *args) const noexcept;
    jlong CallLongMethod(jobject obj, jmethodID methodID, ...) const noexcept;
    jlong CallLongMethodV(jobject obj, jmethodID methodID, va_list args) const noexcept;
    jlong CallLongMethodA(jobject obj, jmethodID methodID, const jvalue *args) const noexcept;
    jfloat CallFloatMethod(jobject obj, jmethodID methodID, ...) const noexcept;
    jfloat CallFloatMethodV(jobject obj, jmethodID methodID, va_list args) const noexcept;
    jfloat CallFloatMethodA(jobject obj, jmethodID methodID, const jvalue *args) const noexcept;
    jdouble CallDoubleMethod(jobject obj, jmethodID methodID, ...) const noexcept;
    jdouble CallDoubleMethodV(jobject obj, jmethodID methodID, va_list args) const noexcept;
    jdouble CallDoubleMethodA(jobject obj, jmethodID methodID, const jvalue *args) const noexcept;
    void CallVoidMethod(jobject obj, jmethodID methodID, ...) const noexcept;
    void CallVoidMethodV(jobject obj, jmethodID methodID, va_list args) const noexcept;
    void CallVoidMethodA(jobject obj, jmethodID methodID, const jvalue *args) const noexcept;
    jobject CallNonvirtualObjectMethod(jobject obj, jclass clazz, jmethodID methodID, ...) const noexcept;
    jobject CallNonvirtualObjectMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jobject CallNonvirtualObjectMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jboolean CallNonvirtualBooleanMethod(jobject obj, jclass clazz, jmethodID methodID, ...) const noexcept;
    jboolean CallNonvirtualBooleanMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jboolean CallNonvirtualBooleanMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jbyte CallNonvirtualByteMethod(jobject obj, jclass clazz, jmethodID methodID, ...) const noexcept;
    jbyte CallNonvirtualByteMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jbyte CallNonvirtualByteMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jchar CallNonvirtualCharMethod(jobject obj, jclass clazz, jmethodID methodID, ...) const noexcept;
    jchar CallNonvirtualCharMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jchar CallNonvirtualCharMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jshort CallNonvirtualShortMethod(jobject obj, jclass clazz, jmethodID methodID, ...) const noexcept;
    jshort CallNonvirtualShortMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jshort CallNonvirtualShortMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jint CallNonvirtualIntMethod(jobject obj, jclass clazz, jmethodID methodID, ...) const noexcept;
    jint CallNonvirtualIntMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jint CallNonvirtualIntMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jlong CallNonvirtualLongMethod(jobject obj, jclass clazz, jmethodID methodID, ...) const noexcept;
    jlong CallNonvirtualLongMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jlong CallNonvirtualLongMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jfloat CallNonvirtualFloatMethod(jobject obj, jclass clazz, jmethodID methodID, ...) const noexcept;
    jfloat CallNonvirtualFloatMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jfloat CallNonvirtualFloatMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jdouble CallNonvirtualDoubleMethod(jobject obj, jclass clazz, jmethodID methodID, ...) const noexcept;
    jdouble CallNonvirtualDoubleMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jdouble CallNonvirtualDoubleMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    void CallNonvirtualVoidMethod(jobject obj, jclass clazz, jmethodID methodID, ...) const noexcept;
    void CallNonvirtualVoidMethodV(jobject obj, jclass clazz, jmethodID methodID, va_list args) const noexcept;
    void CallNonvirtualVoidMethodA(jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jfieldID GetFieldID(jclass clazz, const char *name, const char *sig) const noexcept;
    jobject GetObjectField(jobject obj, jfieldID fieldID) const noexcept;
    jboolean GetBooleanField(jobject obj, jfieldID fieldID) const noexcept;
    jbyte GetByteField(jobject obj, jfieldID fieldID) const noexcept;
    jchar GetCharField(jobject obj, jfieldID fieldID) const noexcept;
    jshort GetShortField(jobject obj, jfieldID fieldID) const noexcept;
    jint GetIntField(jobject obj, jfieldID fieldID) const noexcept;
    jlong GetLongField(jobject obj, jfieldID fieldID) const noexcept;
    jfloat GetFloatField(jobject obj, jfieldID fieldID) const noexcept;
    jdouble GetDoubleField(jobject obj, jfieldID fieldID) const noexcept;
    void SetObjectField(jobject obj, jfieldID fieldID, jobject val) const noexcept;
    void SetBooleanField(jobject obj, jfieldID fieldID, jboolean val) const noexcept;
    void SetByteField(jobject obj, jfieldID fieldID, jbyte val) const noexcept;
    void SetCharField(jobject obj, jfieldID fieldID, jchar val) const noexcept;
    void SetShortField(jobject obj, jfieldID fieldID, jshort val) const noexcept;
    void SetIntField(jobject obj, jfieldID fieldID, jint val) const noexcept;
    void SetLongField(jobject obj, jfieldID fieldID, jlong val) const noexcept;
    void SetFloatField(jobject obj, jfieldID fieldID, jfloat val) const noexcept;
    void SetDoubleField(jobject obj, jfieldID fieldID, jdouble val) const noexcept;
    jmethodID GetStaticMethodID(jclass clazz, const char *name, const char *sig) const noexcept;
    jobject CallStaticObjectMethod(jclass clazz, jmethodID methodID, ...) const noexcept;
    jobject CallStaticObjectMethodV(jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jobject CallStaticObjectMethodA(jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jboolean CallStaticBooleanMethod(jclass clazz, jmethodID methodID, ...) const noexcept;
    jboolean CallStaticBooleanMethodV(jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jboolean CallStaticBooleanMethodA(jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jbyte CallStaticByteMethod(jclass clazz, jmethodID methodID, ...) const noexcept;
    jbyte CallStaticByteMethodV(jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jbyte CallStaticByteMethodA(jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jchar CallStaticCharMethod(jclass clazz, jmethodID methodID, ...) const noexcept;
    jchar CallStaticCharMethodV(jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jchar CallStaticCharMethodA(jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jshort CallStaticShortMethod(jclass clazz, jmethodID methodID, ...) const noexcept;
    jshort CallStaticShortMethodV(jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jshort CallStaticShortMethodA(jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jint CallStaticIntMethod(jclass clazz, jmethodID methodID, ...) const noexcept;
    jint CallStaticIntMethodV(jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jint CallStaticIntMethodA(jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jlong CallStaticLongMethod(jclass clazz, jmethodID methodID, ...) const noexcept;
    jlong CallStaticLongMethodV(jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jlong CallStaticLongMethodA(jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jfloat CallStaticFloatMethod(jclass clazz, jmethodID methodID, ...) const noexcept;
    jfloat CallStaticFloatMethodV(jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jfloat CallStaticFloatMethodA(jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    jdouble CallStaticDoubleMethod(jclass clazz, jmethodID methodID, ...) const noexcept;
    jdouble CallStaticDoubleMethodV(jclass clazz, jmethodID methodID, va_list args) const noexcept;
    jdouble CallStaticDoubleMethodA(jclass clazz, jmethodID methodID, const jvalue *args) const noexcept;
    void CallStaticVoidMethod(jclass cls, jmethodID methodID, ...) const noexcept;
    void CallStaticVoidMethodV(jclass cls, jmethodID methodID, va_list args) const noexcept;
    void CallStaticVoidMethodA(jclass cls, jmethodID methodID, const jvalue *args) const noexcept;
    jfieldID GetStaticFieldID(jclass clazz, const char *name, const char *sig) const noexcept;
    jobject GetStaticObjectField(jclass clazz, jfieldID fieldID) const noexcept;
    jboolean GetStaticBooleanField(jclass clazz, jfieldID fieldID) const noexcept;
    jbyte GetStaticByteField(jclass clazz, jfieldID fieldID) const noexcept;
    jchar GetStaticCharField(jclass clazz, jfieldID fieldID) const noexcept;
    jshort GetStaticShortField(jclass clazz, jfieldID fieldID) const noexcept;
    jint GetStaticIntField(jclass clazz, jfieldID fieldID) const noexcept;
    jlong GetStaticLongField(jclass clazz, jfieldID fieldID) const noexcept;
    jfloat GetStaticFloatField(jclass clazz, jfieldID fieldID) const noexcept;
    jdouble GetStaticDoubleField(jclass clazz, jfieldID fieldID) const noexcept;
    void SetStaticObjectField(jclass clazz, jfieldID fieldID, jobject value) const noexcept;
    void SetStaticBooleanField(jclass clazz, jfieldID fieldID, jboolean value) const noexcept;
    void SetStaticByteField(jclass clazz, jfieldID fieldID, jbyte value) const noexcept;
    void SetStaticCharField(jclass clazz, jfieldID fieldID, jchar value) const noexcept;
    void SetStaticShortField(jclass clazz, jfieldID fieldID, jshort value) const noexcept;
    void SetStaticIntField(jclass clazz, jfieldID fieldID, jint value) const noexcept;
    void SetStaticLongField(jclass clazz, jfieldID fieldID, jlong value) const noexcept;
    void SetStaticFloatField(jclass clazz, jfieldID fieldID, jfloat value) const noexcept;
    void SetStaticDoubleField(jclass clazz, jfieldID fieldID, jdouble value) const noexcept;
    jstring NewString(const jchar *unicode, jsize len) const noexcept;
    jsize GetStringLength(jstring str) const noexcept;
    const jchar *GetStringChars(jstring str, jboolean *isCopy) const noexcept;
    void ReleaseStringChars(jstring str, const jchar *chars) const noexcept;
    jstring NewStringUTF(const char *utf) const noexcept;
    jsize GetStringUTFLength(jstring str) const noexcept;
    const char* GetStringUTFChars(jstring str, jboolean *isCopy) const noexcept;
    void ReleaseStringUTFChars(jstring str, const char* chars) const noexcept;
    jsize GetArrayLength(jarray array) const noexcept;
    jobjectArray NewObjectArray(jsize len, jclass clazz, jobject init) const noexcept;
    jobject GetObjectArrayElement(jobjectArray array, jsize index) const noexcept;
    void SetObjectArrayElement(jobjectArray array, jsize index, jobject val) const noexcept;
    jbooleanArray NewBooleanArray(jsize len) const noexcept;
    jbyteArray NewByteArray(jsize len) const noexcept;
    jcharArray NewCharArray(jsize len) const noexcept;
    jshortArray NewShortArray(jsize len) const noexcept;
    jintArray NewIntArray(jsize len) const noexcept;
    jlongArray NewLongArray(jsize len) const noexcept;
    jfloatArray NewFloatArray(jsize len) const noexcept;
    jdoubleArray NewDoubleArray(jsize len) const noexcept;
    jboolean* GetBooleanArrayElements(jbooleanArray array, jboolean *isCopy) const noexcept;
    jbyte* GetByteArrayElements(jbyteArray array, jboolean *isCopy) const noexcept;
    jchar* GetCharArrayElements(jcharArray array, jboolean *isCopy) const noexcept;
    jshort* GetShortArrayElements(jshortArray array, jboolean *isCopy) const noexcept;
    jint* GetIntArrayElements(jintArray array, jboolean *isCopy) const noexcept;
    jlong* GetLongArrayElements(jlongArray array, jboolean *isCopy) const noexcept;
    jfloat* GetFloatArrayElements(jfloatArray array, jboolean *isCopy) const noexcept;
    jdouble* GetDoubleArrayElements(jdoubleArray array, jboolean *isCopy) const noexcept;
    void ReleaseBooleanArrayElements(jbooleanArray array, jboolean *elems, jint mode) const noexcept;
    void ReleaseByteArrayElements(jbyteArray array, jbyte *elems, jint mode) const noexcept;
    void ReleaseCharArrayElements(jcharArray array, jchar *elems, jint mode) const noexcept;
    void ReleaseShortArrayElements(jshortArray array, jshort *elems, jint mode) const noexcept;
    void ReleaseIntArrayElements(jintArray array, jint *elems, jint mode) const noexcept;
    void ReleaseLongArrayElements(jlongArray array, jlong *elems, jint mode) const noexcept;
    void ReleaseFloatArrayElements(jfloatArray array, jfloat *elems, jint mode) const noexcept;
    void ReleaseDoubleArrayElements(jdoubleArray array, jdouble *elems, jint mode) const noexcept;
    void GetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len, jboolean *buf) const noexcept;
    void GetByteArrayRegion(jbyteArray array, jsize start, jsize len, jbyte *buf) const noexcept;
    void GetCharArrayRegion(jcharArray array, jsize start, jsize len, jchar *buf) const noexcept;
    void GetShortArrayRegion(jshortArray array, jsize start, jsize len, jshort *buf) const noexcept;
    void GetIntArrayRegion(jintArray array, jsize start, jsize len, jint *buf) const noexcept;
    void GetLongArrayRegion(jlongArray array, jsize start, jsize len, jlong *buf) const noexcept;
    void GetFloatArrayRegion(jfloatArray array, jsize start, jsize len, jfloat *buf) const noexcept;
    void GetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len, jdouble *buf) const noexcept;
    void SetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len, const jboolean *buf) const noexcept;
    void SetByteArrayRegion(jbyteArray array, jsize start, jsize len, const jbyte *buf) const noexcept;
    void SetCharArrayRegion(jcharArray array, jsize start, jsize len, const jchar *buf) const noexcept;
    void SetShortArrayRegion(jshortArray array, jsize start, jsize len, const jshort *buf) const noexcept;
    void SetIntArrayRegion(jintArray array, jsize start, jsize len, const jint *buf) const noexcept;
    void SetLongArrayRegion(jlongArray array, jsize start, jsize len, const jlong *buf) const noexcept;
    void SetFloatArrayRegion(jfloatArray array, jsize start, jsize len, const jfloat *buf) const noexcept;
    void SetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len, const jdouble *buf) const noexcept;
    jint RegisterNatives(jclass clazz, const JNINativeMethod *methods, jint nMethods) const noexcept;
    jint UnregisterNatives(jclass clazz) const noexcept;
    jint MonitorEnter(jobject obj) const noexcept;
    jint MonitorExit(jobject obj) const noexcept;
    jint GetJavaVM(JavaVM **vm) const noexcept;
    void GetStringRegion(jstring str, jsize start, jsize len, jchar *buf) const noexcept;
    void GetStringUTFRegion(jstring str, jsize start, jsize len, char *buf) const noexcept;
    void* GetPrimitiveArrayCritical(jarray array, jboolean *isCopy) const noexcept;
    void ReleasePrimitiveArrayCritical(jarray array, void *carray, jint mode) const noexcept;
    const jchar* GetStringCritical(jstring string, jboolean *isCopy) const noexcept;
    void ReleaseStringCritical(jstring string, const jchar *cstring) const noexcept;
    jweak NewWeakGlobalRef(jobject obj) const noexcept;
    void DeleteWeakGlobalRef(jweak ref) const noexcept;
    jboolean ExceptionCheck() const noexcept;
    jobject NewDirectByteBuffer(void* address, jlong capacity) const noexcept;
    void* GetDirectBufferAddress(jobject buf) const noexcept;
    jlong GetDirectBufferCapacity(jobject buf) const noexcept;
    jobjectRefType GetObjectRefType(jobject obj) const noexcept;
};

#endif // JVM_HXX_INCLUDED
