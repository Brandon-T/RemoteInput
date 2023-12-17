//
// Created by Brandon on 2023-12-10.
//

#ifndef REMOTEINPUT_PYTHONJAVAARRAY_HXX
#define REMOTEINPUT_PYTHONJAVAARRAY_HXX

#include "PythonCommon.hxx"
#include "JNI_Common.hxx"

#ifdef __cplusplus
extern "C"
{
#endif

PyObject* Python_JavaArray_Get1D(PyJavaArray* self, PyObject* args[], Py_ssize_t args_length, PyObject* kwnames) noexcept;
PyObject* Python_JavaArray_Get2D(PyJavaArray* self, PyObject* args[], Py_ssize_t args_length, PyObject* kwnames) noexcept;
PyObject* Python_JavaArray_Get3D(PyJavaArray* self, PyObject* args[], Py_ssize_t args_length, PyObject* kwnames) noexcept;
PyObject* Python_JavaArray_Get4D(PyJavaArray* self, PyObject* args[], Py_ssize_t args_length, PyObject* kwnames) noexcept;


#ifdef __cplusplus
}
#endif

#endif //REMOTEINPUT_PYTHONJAVAARRAY_HXX
