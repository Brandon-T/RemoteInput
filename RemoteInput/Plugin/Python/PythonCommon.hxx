//
// Created by Brandon on 2022-09-21.
//

#ifndef REMOTEINPUT_PYTHONCOMMON_HXX
#define REMOTEINPUT_PYTHONCOMMON_HXX

#if defined(USE_PYBIND11)
    #include <memory>
    #include <pybind11/pybind11.h>
    #include <pybind11/stl.h>
#else
    #include <tuple>
    #include <functional>

    #include "PythonMacros.hxx"
    #include "TypeTraits.hxx"
#endif

#include "JNI_Common.hxx"
#include "EIOS.hxx"
#include "PythonGCQueue.hxx"

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
    #if !defined(USE_PYBIND11)
    PyObject_HEAD
    #endif

    std::int32_t pid;
    EIOS* native_eios;
    std::shared_ptr<PyGCQueue> gc_queue;
};

struct PyJavaObject
{
    #if defined(USE_PYBIND11)
    std::shared_ptr<PyEIOS> eios;
    jobject object;
    #else
    PyObject_HEAD
    PyEIOS* eios;
    jobject object;
    #endif
};

struct PyJavaArray
{
    #if defined(USE_PYBIND11)
    std::shared_ptr<PyEIOS> eios;
    jarray array;
    std::size_t size;
    #else
    PyObject_HEAD
    PyEIOS* eios;
    jarray array;
    std::size_t size;
    #endif
};

#if defined(USE_PYBIND11)
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
pybind11::object python_create_eios(EIOS* eios) noexcept;
pybind11::object python_create_object(const std::shared_ptr<PyEIOS>& self, jobject object) noexcept;
pybind11::object python_create_object(const std::shared_ptr<PyJavaObject>& self, jobject object) noexcept;
pybind11::object python_create_object(const std::shared_ptr<PyJavaArray>& self, jobject object) noexcept;
pybind11::object python_create_array(const std::shared_ptr<PyEIOS>& self, jarray array, std::size_t array_size) noexcept;
pybind11::object python_create_array(const std::shared_ptr<PyJavaObject>& self, jarray array, std::size_t array_size) noexcept;
pybind11::object python_create_array(const std::shared_ptr<PyJavaArray>& self, jarray array, std::size_t array_size) noexcept;
#else
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
#endif

#endif //REMOTEINPUT_PYTHONCOMMON_HXX
