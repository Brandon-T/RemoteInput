//
// Created by Brandon on 2023-02-13.
//

#ifndef REMOTEINPUT_PYTHONREFLECTAPI_HXX
#define REMOTEINPUT_PYTHONREFLECTAPI_HXX

#include "PythonCommon.hxx"
#include "EIOS.hxx"

// STRUCTURES
struct PyReflectAPI
{
    PyObject_HEAD
    PyEIOS* eios;
};

extern PyType_Spec PyReflectAPI_Spec;

#ifdef __cplusplus
extern "C"
{
#endif

PyObject* Python_ReflectAPI_Reflect_Object(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Reflect_IsSame_Object(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Reflect_InstanceOf(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Release_Object(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Release_Objects(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Reflect_Bool(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Reflect_Char(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Reflect_Byte(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Reflect_Short(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Reflect_Int(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Reflect_Long(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Reflect_Float(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Reflect_Double(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Reflect_String(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;

// MARK: - Array Functions

PyObject* Python_ReflectAPI_Reflect_Array(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Reflect_Array2D(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Reflect_Array3D(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_ReflectAPI_Reflect_Array_Indices(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;

#ifdef __cplusplus
}
#endif

#endif //REMOTEINPUT_PYTHONREFLECTAPI_HXX
