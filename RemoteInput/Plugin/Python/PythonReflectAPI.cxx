//
// Created by Brandon on 2023-02-13.
//

#include "PythonReflectAPI.hxx"
#include "PythonEIOS.hxx"
#include "PythonJavaObject.hxx"
#include "PythonReflectCommon.hxx"

#include <sstream>
#include <ios>
#include <iomanip>

int PyReflectAPI_Clear(PyObject* object)
{
    PyReflectAPI* py_reflect_api = reinterpret_cast<PyReflectAPI*>(object);
    (python->Py_CLEAR)(reinterpret_cast<PyObject*>(py_reflect_api->eios));
    py_reflect_api->eios = nullptr;
    return 0;
}

void PyReflectAPI_Dealloc(PyObject* object)
{
    // PyObject_GC_UnTrack(object);
    PyReflectAPI_Clear(object);
    //PyObject_Del(object);  // NO GC!
    python->PyObject_Free(object);
}

PyObject* PyReflectAPI_Str(PyObject* object)
{
    PyReflectAPI* py_reflect_api = reinterpret_cast<PyReflectAPI*>(object);
    std::ostringstream stream;

    std::ios state(nullptr);
    state.copyfmt(stream);

    stream << std::setfill('0') << std::uppercase << std::hex;
    stream << "ReflectAPI(";
    stream << "0x" << reinterpret_cast<std::uintptr_t>(object);
    stream << "): ";
    stream.copyfmt(state);

    stream << "{"<<"\n";
    stream<< "    eios: " << py_reflect_api->eios << "\n";
    stream<< "}";

    std::string result = stream.str();
    return python->PyUnicode_FromStringAndSize(result.c_str(), result.size());
}

PyMemberDef PyReflectAPI_Members[] = {
        {nullptr}  /* Sentinel */
};

PyGetSetDef PyReflectAPI_PropertyMembers[] = {
        {nullptr}  /* Sentinel */
};

#if defined(METH_FASTCALL)
PyMethodDef PyReflectAPI_Methods[] = {
        {(char*)"reflect_object",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Object), METH_FASTCALL,      (char*)""},
        {(char*)"is_same_object",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_IsSame_Object), METH_FASTCALL,      (char*)""},
        {(char*)"is_instance_of",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_InstanceOf), METH_FASTCALL,      (char*)""},
        {(char*)"release_object",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Release_Object), METH_FASTCALL,      (char*)""},
        {(char*)"release_objects",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Release_Objects), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_bool",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Bool), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_char",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Char), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_byte",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Byte), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_short",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Short), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_int",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Int), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_long",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Long), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_float",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Float), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_double",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Double), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_string",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_String), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_array",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Array), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_array2d",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Array2D), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_array3d",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Array3D), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_array_indices",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Array_Indices), METH_FASTCALL,      (char*)""},
        {nullptr}  /* Sentinel */
};
#else
PyMethodDef PyReflectAPI_Methods[] = {
        {(char*)"reflect_object",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Object), METH_VARARGS,      (char*)""},
        {(char*)"is_same_object",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_IsSame_Object), METH_VARARGS,      (char*)""},
        {(char*)"is_instance_of",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_InstanceOf), METH_VARARGS,      (char*)""},
        {(char*)"release_object",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Release_Object), METH_VARARGS,      (char*)""},
        {(char*)"release_objects",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Release_Objects), METH_VARARGS,      (char*)""},
        {(char*)"reflect_bool",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Bool), METH_VARARGS,      (char*)""},
        {(char*)"reflect_char",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Char), METH_VARARGS,      (char*)""},
        {(char*)"reflect_byte",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Byte), METH_VARARGS,      (char*)""},
        {(char*)"reflect_short",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Short), METH_VARARGS,      (char*)""},
        {(char*)"reflect_int",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Int), METH_VARARGS,      (char*)""},
        {(char*)"reflect_long",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Long), METH_VARARGS,      (char*)""},
        {(char*)"reflect_float",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Float), METH_VARARGS,      (char*)""},
        {(char*)"reflect_double",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Double), METH_VARARGS,      (char*)""},
        {(char*)"reflect_string",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_String), METH_VARARGS,      (char*)""},
        {(char*)"reflect_array",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Array), METH_VARARGS,      (char*)""},
        {(char*)"reflect_array2d",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Array2D), METH_VARARGS,      (char*)""},
        {(char*)"reflect_array3d",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Array3D), METH_VARARGS,      (char*)""},
        {(char*)"reflect_array_indices",            reinterpret_cast<PyCFunction>(Python_ReflectAPI_Reflect_Array_Indices), METH_VARARGS,      (char*)""},
        {nullptr}  /* Sentinel */
};
#endif

PyType_Slot PyReflectAPI_Slots[] = {
        {Py_tp_init, nullptr},
        {Py_tp_members, PyReflectAPI_Members},
        {Py_tp_methods, PyReflectAPI_Methods},
        {Py_tp_getset, PyReflectAPI_PropertyMembers},
        {Py_tp_dealloc, reinterpret_cast<void*>(&PyReflectAPI_Dealloc)},
        {Py_tp_clear, reinterpret_cast<void*>(&PyReflectAPI_Clear)},
        {Py_tp_str, reinterpret_cast<void*>(&PyReflectAPI_Str)},
        {Py_tp_doc, static_cast<void*>(const_cast<char*>(PyDoc_STR("Reflection API Structure")))},
        {0, NULL},
};

PyType_Spec PyReflectAPI_Spec {
        "remote_input.ReflectAPI",
        sizeof(PyReflectAPI),
        0,
        #if HAS_PYTHON_VERSION(0x030A0000)
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_IMMUTABLETYPE | Py_TPFLAGS_DISALLOW_INSTANTIATION,
        #else
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        #endif
        PyReflectAPI_Slots
};

// MARK: EXPORTS

PyReflectAPI* PythonWrapReflectAPI(PyEIOS* eios) noexcept
{
    PyTypeObject* PyReflectAPIType = reinterpret_cast<PyTypeObject*>(python->PyType_FromSpec(&PyReflectAPI_Spec));
    PyReflectAPI* reflect_api = (python->PyObject_New<PyReflectAPI>)(PyReflectAPIType);
    reflect_api->eios = eios;
    (python->Py_INCREF)(reinterpret_cast<PyObject*>(eios));
    return reflect_api;
}

PyReflectAPI* PythonUnwrapReflectAPI(PyObject* api) noexcept
{
    return reinterpret_cast<PyReflectAPI*>(api);
}

PyEIOS* PythonUnwrapEIOS(PyReflectAPI* api) noexcept
{
    return api->eios;
}

PyObject* Python_ReflectAPI_Reflect_Object(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 3)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (class, field, description), %zus given.", 3, args_length);
        return nullptr;
    }
    #endif

    PyObject* static_args[] = {python->Py_GetNone_Object(), args[0], args[1], args[2]};
    return Python_Reflect_Object(self, static_args, args_length + 1);
}

PyObject* Python_ReflectAPI_Reflect_IsSame_Object(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    return Python_Reflect_IsSame_Object(self, args, args_length);
}

PyObject* Python_ReflectAPI_Reflect_InstanceOf(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    return Python_Reflect_InstanceOf(self, args, args_length);
}

PyObject* Python_ReflectAPI_Release_Object(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    return Python_Reflect_Release_Object(self, args, args_length);
}

PyObject* Python_ReflectAPI_Release_Objects(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    return Python_Reflect_Release_Objects(self, args, args_length);
}

PyObject* Python_ReflectAPI_Reflect_Bool(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 3)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (class, field, description), %zus given.", 3, args_length);
        return nullptr;
    }
    #endif

    PyObject* static_args[] = {python->Py_GetNone_Object(), args[0], args[1], args[2]};
    return Python_Reflect_Bool(self, static_args, args_length + 1);
}

PyObject* Python_ReflectAPI_Reflect_Char(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 3)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (class, field, description), %zus given.", 3, args_length);
        return nullptr;
    }
    #endif

    PyObject* static_args[] = {python->Py_GetNone_Object(), args[0], args[1], args[2]};
    return Python_Reflect_Char(self, static_args, args_length + 1);
}

PyObject* Python_ReflectAPI_Reflect_Byte(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 3)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (class, field, description), %zus given.", 3, args_length);
        return nullptr;
    }
    #endif

    PyObject* static_args[] = {python->Py_GetNone_Object(), args[0], args[1], args[2]};
    return Python_Reflect_Byte(self, static_args, args_length + 1);
}

PyObject* Python_ReflectAPI_Reflect_Short(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 3)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (class, field, description), %zus given.", 3, args_length);
        return nullptr;
    }
    #endif

    PyObject* static_args[] = {python->Py_GetNone_Object(), args[0], args[1], args[2]};
    return Python_Reflect_Short(self, static_args, args_length + 1);
}

PyObject* Python_ReflectAPI_Reflect_Int(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 3)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (class, field, description), %zus given.", 3, args_length);
        return nullptr;
    }
    #endif

    PyObject* static_args[] = {python->Py_GetNone_Object(), args[0], args[1], args[2]};
    return Python_Reflect_Int(self, static_args, args_length + 1);
}

PyObject* Python_ReflectAPI_Reflect_Long(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 3)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (class, field, description), %zus given.", 3, args_length);
        return nullptr;
    }
    #endif

    PyObject* static_args[] = {python->Py_GetNone_Object(), args[0], args[1], args[2]};
    return Python_Reflect_Long(self, static_args, args_length + 1);
}

PyObject* Python_ReflectAPI_Reflect_Float(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 3)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (class, field, description), %zus given.", 3, args_length);
        return nullptr;
    }
    #endif

    PyObject* static_args[] = {python->Py_GetNone_Object(), args[0], args[1], args[2]};
    return Python_Reflect_Float(self, static_args, args_length + 1);
}

PyObject* Python_ReflectAPI_Reflect_Double(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 3)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (class, field, description), %zus given.", 3, args_length);
        return nullptr;
    }
    #endif

    PyObject* static_args[] = {python->Py_GetNone_Object(), args[0], args[1], args[2]};
    return Python_Reflect_Double(self, static_args, args_length + 1);
}

PyObject* Python_ReflectAPI_Reflect_String(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 3)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (class, field, description), %zus given.", 3, args_length);
        return nullptr;
    }
    #endif

    PyObject* static_args[] = {python->Py_GetNone_Object(), args[0], args[1], args[2]};
    return Python_Reflect_String(self, static_args, args_length + 1);
}

// MARK: - Array Functions

PyObject* Python_ReflectAPI_Reflect_Array(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 6)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (class, field, description, type, index, length), %zus given.", 6, args_length);
        return nullptr;
    }
    #endif

    PyObject* static_args[] = {python->Py_GetNone_Object(), args[0], args[1], args[2], args[3], args[4], args[5]};
    return Python_Reflect_Array(self, static_args, args_length + 1);
}

PyObject* Python_ReflectAPI_Reflect_Array2D(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 7)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (class, field, description, type, x, y, length), %zus given.", 7, args_length);
        return nullptr;
    }
    #endif

    PyObject* static_args[] = {python->Py_GetNone_Object(), args[0], args[1], args[2], args[3], args[4], args[5], args[6]};
    return Python_Reflect_Array2D(self, static_args, args_length + 1);
}

PyObject* Python_ReflectAPI_Reflect_Array3D(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 8)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (class, field, description, type, x, y, z, length), %zus given.", 8, args_length);
        return nullptr;
    }
    #endif

    PyObject* static_args[] = {python->Py_GetNone_Object(), args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]};
    return Python_Reflect_Array3D(self, static_args, args_length + 1);
}

PyObject* Python_ReflectAPI_Reflect_Array_Indices(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 5)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (class, field, description, type, indices), %zus given.", 5, args_length);
        return nullptr;
    }
    #endif

    PyObject* static_args[] = {python->Py_GetNone_Object(), args[0], args[1], args[2], args[3], args[4]};
    return Python_Reflect_Array_Indices(self, static_args, args_length + 1);
}

