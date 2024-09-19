//
// Created by Brandon on 2023-12-18.
//

#ifndef REMOTEINPUT_PYTHONJAVALIST_HXX
#define REMOTEINPUT_PYTHONJAVALIST_HXX

#if !defined(USE_PYBIND11)
#include "Python.hxx"

struct PyEIOS;
PyObject* create_java_list(PyEIOS* eios, Py_ssize_t length);
#endif

#endif //REMOTEINPUT_PYTHONJAVALIST_HXX
