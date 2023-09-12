//
// Created by Brandon on 2023-02-13.
//

#ifndef REMOTEINPUT_PYTHONREFLECTCOMMON_HXX
#define REMOTEINPUT_PYTHONREFLECTCOMMON_HXX

#include "PythonCommon.hxx"
#include "EIOS.hxx"

// STRUCTURES

#ifdef __cplusplus
extern "C"
{
#endif

PyObject* Python_Reflect_Object(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_IsSame_Object(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_InstanceOf(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Release_Object(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Release_Objects(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Bool(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Char(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Byte(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Short(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Int(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Long(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Float(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Double(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_String(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;

// MARK: - Array Functions

PyObject* Python_Reflect_Array(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Array2D(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Array3D(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Array_Size(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Array_Index_Size(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Array_Indices(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept;

#ifdef __cplusplus
}
#endif

#endif //REMOTEINPUT_PYTHONREFLECTCOMMON_HXX
