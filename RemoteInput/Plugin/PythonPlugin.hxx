//
//  PythonPlugin.hxx
//  RemoteInput
//
//  Created by Brandon on 2020-11-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#ifndef REMOTEINPUT_PYTHONPLUGIN_HXX
#define REMOTEINPUT_PYTHONPLUGIN_HXX

#include "Plugin.hxx"

//On Windows, Python is really badly implemented, so we must include `math.h` and not `cmath` to get the _hpyot symbol
#include <math.h>
#include <Python.h>

#if PY_MAJOR_VERSION < 3
# define MODINIT(name)  init ## name
#else
# define MODINIT(name)  PyInit_ ## name
#endif

#ifdef __cplusplus
extern "C"
{
#endif

PyObject* PYTHON_EIOS_Inject(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_EIOS_Inject_PID(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_GetEIOS(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_Object(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_IsSame_Object(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_InstanceOf(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Release_Object(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Release_Objects(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_Bool(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_Char(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_Byte(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_Short(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_Int(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_Long(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_Float(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_Double(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_String(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_Array(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_Array_With_Size(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_Array_Size(PyObject* self, PyObject* args) noexcept;
PyObject* PYTHON_Reflect_Array_Index(PyObject* self, PyObject* args) noexcept;

#if defined(_WIN32) || defined(_WIN64)
PyMODINIT_FUNC MODINIT(remote_input)();
#else
EXPORT PyMODINIT_FUNC MODINIT(remote_input)();
#endif

#ifdef __cplusplus
}
#endif

#endif //REMOTEINPUT_PYTHONPLUGIN_HXX
