//
// Created by Brandon on 2022-09-21.
//

#ifndef REMOTEINPUT_PYTHONJAVAOBJECT_HXX
#define REMOTEINPUT_PYTHONJAVAOBJECT_HXX

#include "PythonCommon.hxx"
#include "JNI_Common.hxx"

#ifdef __cplusplus
extern "C"
{
#endif

PyObject* Python_Reflect_Object(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_IsSame_Object(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_InstanceOf(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Bool(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Char(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Byte(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Short(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Int(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Long(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Float(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Double(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_String(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_Reflect_Array(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_JavaObject_Release_Object(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept;

#ifdef __cplusplus
}
#endif

#endif //REMOTEINPUT_PYTHONJAVAOBJECT_HXX
