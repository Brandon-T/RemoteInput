//
//  PythonPlugin.cxx
//  RemoteInput
//
//  Created by Brandon on 2020-11-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#include "PythonPlugin.hxx"
#include "PythonEIOS.hxx"
#include "PythonJavaObject.hxx"
#include "PythonJavaList.hxx"
#include "Python.hxx"

#include <unordered_map>
#include <memory>

std::unique_ptr<Python> python;

// MODULE

static struct PyMethodDef RemoteInputMethods[] =
{
        //{"load_module", nullptr, METH_VARARGS, "Documentation"},
        {nullptr} /* SENTINEL */
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef RemoteInputModule =
{
        PyModuleDef_HEAD_INIT,
        "remote_input",
        "Remote Input Module",
        -1,
        RemoteInputMethods,
        nullptr,
        nullptr,
        nullptr,
        nullptr
};

void PrintPythonVersionInfo()
{
    #if HAS_PYTHON_VERSION(0x030B1000)
    fprintf(stdout, "RUNNING WITH: %zu.%zu.%zu\n", Py_Version >> 24 & 0xFF, Py_Version >> 16 & 0xFF, Py_Version >> 8 & 0xFF);
    fprintf(stdout, "COMPILED WITH: %d.%d.%d\n", PY_MAJOR_VERSION, PY_MINOR_VERSION, PY_MICRO_VERSION);
    #else
    unsigned long Py_Version = 0;

    // Py_Initialize();
    PyObject* sys_import = python->PyUnicode_FromString("sys");
    if (sys_import)
    {
        PyObject* sys_module = python->PyImport_Import(sys_import);
        if (sys_module)
        {
            PyObject* sys_module_info = python->PyModule_GetDict(sys_module);
            if (sys_module_info)
            {
                PyObject* hex_version = python->PyDict_GetItemString(sys_module_info, "hexversion");
                if (hex_version)
                {
                    Py_Version = python->PyLong_AsUnsignedLong(hex_version);
                }
            }
            (python->Py_XDECREF)(sys_module);
        }
        (python->Py_XDECREF)(sys_import);
    }
    // Py_Finalize();

    if (Py_Version > 0)
    {
        fprintf(stdout, "Running with Python: %lu.%lu.%lu\n", Py_Version >> 24 & 0xFF, Py_Version >> 16 & 0xFF, Py_Version >> 8 & 0xFF);
    }
    else
    {
        fprintf(stdout, "RUNNING WITH: %s\n", python->Py_GetVersion());
    }

    fprintf(stdout, "Compiled with Python: %d.%d.%d\n", PY_MAJOR_VERSION, PY_MINOR_VERSION, PY_MICRO_VERSION);
    #endif

    fflush(stdout);
}

bool RegisterEnum(PyObject* module, const std::string &enum_name, const std::unordered_map<std::string, std::int32_t> &enumeration)
{
    PyObject* enum_module = python->PyImport_ImportModule("enum");
    if (!enum_module)
    {
        return false;
    }

    PyObject* constants = python->PyDict_New();
    for (const auto& it : enumeration)
    {
        python->PyDict_SetItemString(constants, it.first.c_str(), python->PyLong_FromLong(it.second));
    }

    PyObject* enum_class = python->PyObject_CallMethod(enum_module, "IntEnum", "sO", enum_name.c_str(), constants);
    (python->Py_CLEAR)(constants);
    (python->Py_CLEAR)(enum_module);

    if (python->PyModule_AddObject(module, enum_name.c_str(), enum_class) < 0)
    {
        (python->Py_CLEAR)(enum_class);
        return false;
    }
    return true;
}

bool RegisterPythonType(PyObject* module, PyTypeObject* type_object, const char* type_name)
{
    if (python->PyType_Ready(type_object) < 0)
    {
        return false;
    }

    (python->Py_INCREF)(reinterpret_cast<PyObject*>(type_object));
    if (python->PyModule_AddObject(module, type_name, reinterpret_cast<PyObject*>(type_object)) < 0)
    {
        (python->Py_DECREF)(reinterpret_cast<PyObject*>(type_object));
        return false;
    }
    return true;
}

//PyMODINIT_FUNC MODINIT(remote_input)()
PyObject* PyInit_remote_input()
{
    python = std::make_unique<Python>();
    if (!python->IsPythonLoaded())
    {
        fprintf(stderr, "Cannot Find libpython3\n");
        return nullptr;
    }

    #if defined(DEBUG)
    PrintPythonVersionInfo();
    #endif

    PyObject* module = (python->PyModule_Create2)(&RemoteInputModule, PYTHON_ABI_VERSION);

    if (!RegisterPythonType(module, PyEIOS_Type(), "EIOS"))
    {
        return nullptr;
    }

    if (!RegisterPythonType(module, PyJavaObject_Type(), "JavaObject"))
    {
        return nullptr;
    }

    if (!RegisterPythonType(module, PyJavaArray_Type(), "JavaArray"))
    {
        return nullptr;
    }

    RegisterEnum(module, "ImageFormat", {
        {"BGR_BGRA", static_cast<std::int32_t>(ImageFormat::BGR_BGRA)},
        {"BGRA", static_cast<std::int32_t>(ImageFormat::BGRA)},
        {"RGBA", static_cast<std::int32_t>(ImageFormat::RGBA)},
        {"ARGB", static_cast<std::int32_t>(ImageFormat::ARGB)},
        {"ABGR", static_cast<std::int32_t>(ImageFormat::ABGR)},
    });

    RegisterEnum(module, "ReflectType", {
        {"BOOLEAN", static_cast<std::int32_t>(ReflectionType::BOOL)},
        {"CHAR", static_cast<std::int32_t>(ReflectionType::CHAR)},
        {"BYTE", static_cast<std::int32_t>(ReflectionType::BYTE)},
        {"SHORT", static_cast<std::int32_t>(ReflectionType::SHORT)},
        {"INT", static_cast<std::int32_t>(ReflectionType::INT)},
        {"LONG", static_cast<std::int32_t>(ReflectionType::LONG)},
        {"FLOAT", static_cast<std::int32_t>(ReflectionType::FLOAT)},
        {"DOUBLE", static_cast<std::int32_t>(ReflectionType::DOUBLE)},
        {"STRING", static_cast<std::int32_t>(ReflectionType::STRING)},
        {"OBJECT", static_cast<std::int32_t>(ReflectionType::OBJECT)},
        {"ARRAY", static_cast<std::int32_t>(ReflectionType::ARRAY)},
    });

    fprintf(stderr, "LOADED!\n");
    return module;
}
#else
PyMODINIT_FUNC MODINIT(remote_input)()
{
    fprintf(stderr, "LOADED!\n");

    PyObject* PyEIOS_Type = PyType_FromSpec(&PyEIOS_Spec);
    if (PyType_Ready(reinterpret_cast<PyTypeObject*>(PyEIOS_Type)) < 0)
    {
        return nullptr;
    }

    PyObject* PyJavaObject_Type = PyType_FromSpec(&PyJavaObject_Spec);
    if (PyType_Ready(reinterpret_cast<PyTypeObject*>(PyJavaObject_Type)) < 0)
    {
        return nullptr;
    }

    PyObject* module = PyModule_Create(&RemoteInputModule);

    Py_INCREF(PyEIOS_Type);
    if (PyModule_AddObject(module, "EIOS", PyEIOS_Type))
    {
        Py_DECREF(PyEIOS_Type);
        return nullptr;
    }

    Py_INCREF(PyJavaObject_Type);
    if (PyModule_AddObject(module, "JavaObject", PyJavaObject_Type))
    {
        Py_DECREF(PyJavaObject_Type);
        return nullptr;
    }

    return module;

    //PyModule_New("remote_input");
    PyObject* module = Py_InitModule("remote_input", RemoteInputMethods);
}
#endif

