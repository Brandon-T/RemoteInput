//
// Created by Brandon on 2023-12-18.
//

#ifndef REMOTEINPUT_PYTHONJAVALIST_HXX
#define REMOTEINPUT_PYTHONJAVALIST_HXX

#include "Python.hxx"

struct PyEIOS;
PyObject* create_java_list(PyEIOS* eios, Py_ssize_t length);

#endif //REMOTEINPUT_PYTHONJAVALIST_HXX
