//
// Created by Brandon on 2022-09-21.
//

#ifndef REMOTEINPUT_PYTHONCOMMON_HXX
#define REMOTEINPUT_PYTHONCOMMON_HXX

// On Windows, Python is really badly implemented,
// so we must include `math.h` and not `cmath` to get the _hpyot symbol
#include <math.h>
#include <Python.h>
#include "object.h"

#if defined(_WIN32) || defined(_WIN64)
#include "structmember.h"
#elif defined(__linux__) && (defined(__x86_64__) || defined(__i386__))
#include "structmember.h"
#elif defined(__APPLE__)
#if __has_include(<Python/structmember.h>)
#include <Python/structmember.h>  /* Python.framework */
#else
#include "structmember.h"
#endif
#elif defined(__aarch64__) || defined(__arm__)
#include "structmember.h"
#endif

#include <tuple>
#include <functional>
#include <string>

#include "JNI_Common.hxx"
#include "TypeTraits.hxx"
#include "TypeTraits_Functional.hxx"
#include "EIOS.hxx"

// STRUCTURES
enum class PyRemoteInputType
{
    EIOS,
    JAVA_OBJECT,
    JAVA_ARRAY,
    OTHER
};

struct PyEIOS
{
    PyObject_HEAD
    std::int32_t pid;
    std::int32_t width;
    std::int32_t height;
    EIOS* native_eios;
};

struct PyJavaObject
{
    PyObject_HEAD
    PyEIOS* eios;
    jobject object;
};

struct PyJavaArray
{
    PyObject_HEAD
    PyEIOS* eios;
    jarray array;
    std::size_t size;
};

extern PyTypeObject* PyEIOS_Type() noexcept;
extern PyTypeObject* PyJavaObject_Type() noexcept;
extern PyTypeObject* PyJavaArray_Type() noexcept;

PyRemoteInputType GetObjectType(PyObject* object) noexcept;

PyObject* PythonWrapEIOS(EIOS* eios) noexcept;
EIOS* PythonUnwrapEIOS(PyEIOS* eios) noexcept;
PyEIOS* PythonGetEIOS(PyObject* object);

PyJavaObject* PythonWrapJavaObject(PyEIOS* eios, jobject object) noexcept;
jobject PythonUnwrapJavaObject(PyJavaObject* object) noexcept;

PyJavaArray* PythonWrapJavaArray(PyEIOS* eios, jarray array, std::size_t size) noexcept;
jarray PythonUnwrapJavaArray(PyJavaArray* array) noexcept;

template<typename T>
T from_python_object(PyObject* object);

template<typename T>
std::vector<T> from_python_array(PyObject* object);

template<typename T>
PyObject* to_python_object(const T& value);

template<typename T>
PyObject* to_python_array(const std::vector<T>& value);

#include "PythonCommon_Templates.hxx"

#endif //REMOTEINPUT_PYTHONCOMMON_HXX
