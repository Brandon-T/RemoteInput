//
// Created by Brandon on 2022-09-21.
//

#include "PythonJavaObject.hxx"
#include "NativePlugin.hxx"

int PyJavaObject_Clear(PyObject* object)
{
    PyJavaObject* py_java_object = reinterpret_cast<PyJavaObject*>(object);
    (python->Py_CLEAR)(reinterpret_cast<PyObject*>(py_java_object->eios));
    py_java_object->eios = nullptr;
    py_java_object->object = nullptr;
    return 0;
}

void PyJavaObject_Dealloc(PyObject* object)
{
    PyJavaObject* py_java_object = reinterpret_cast<PyJavaObject *>(object);
    if (py_java_object->eios && py_java_object->object)
    {
        Reflect_Release_Object(PythonUnwrapEIOS(py_java_object->eios), py_java_object->object);
    }

    // PyObject_GC_UnTrack(object);
    PyJavaObject_Clear(object);
    //PyObject_Del(object);  // NO GC!
    python->PyObject_Free(object);
}

PyMemberDef PyJavaObject_Members[] = {
        {nullptr}  /* Sentinel */
};

PyGetSetDef PyJavaObject_PropertyMembers[] = {
        {nullptr}  /* Sentinel */
};

PyMethodDef PyJavaObject_Methods[] = {
        {(char*)"reflect_object",            PYTHON_FASTCALL(Python_Reflect_Object), METH_FASTCALL,      (char*)""},
        {(char*)"is_same_object",            PYTHON_FASTCALL(Python_Reflect_IsSame_Object), METH_FASTCALL,      (char*)""},
        {(char*)"is_instance_of",            PYTHON_FASTCALL(Python_Reflect_InstanceOf), METH_FASTCALL,      (char*)""},
        //{(char*)"release_object",            PYTHON_FASTCALL(Python_Reflect_Release_Object), METH_FASTCALL,      (char*)""},
        //{(char*)"release_objects",            PYTHON_FASTCALL(Python_Reflect_Release_Objects), METH_FASTCALL | METH_STATIC,      (char*)""},
        {(char*)"reflect_bool",            PYTHON_FASTCALL(Python_Reflect_Bool), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_char",            PYTHON_FASTCALL(Python_Reflect_Char), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_byte",            PYTHON_FASTCALL(Python_Reflect_Byte), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_short",            PYTHON_FASTCALL(Python_Reflect_Short), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_int",            PYTHON_FASTCALL(Python_Reflect_Int), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_long",            PYTHON_FASTCALL(Python_Reflect_Long), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_float",            PYTHON_FASTCALL(Python_Reflect_Float), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_double",            PYTHON_FASTCALL(Python_Reflect_Double), METH_FASTCALL,      (char*)""},
        {(char*)"reflect_string",            PYTHON_FASTCALL(Python_Reflect_String), METH_FASTCALL,      (char*)""},

        {(char*)"reflect_array",            PYTHON_FASTCALL(Python_Reflect_Array), METH_FASTCALL,      (char*)""},
        {nullptr}  /* Sentinel */
};

PyType_Slot PyJavaObject_Slots[] = {
        {Py_tp_init, nullptr},
        {Py_tp_members, PyJavaObject_Members},
        {Py_tp_methods, PyJavaObject_Methods},
        {Py_tp_getset, PyJavaObject_PropertyMembers},
        {Py_tp_dealloc, reinterpret_cast<void*>(&PyJavaObject_Dealloc)},
        {Py_tp_clear, reinterpret_cast<void*>(&PyJavaObject_Clear)},
        {Py_tp_doc, static_cast<void*>(const_cast<char*>(PyDoc_STR("JavaObject Structure")))},
        {Py_tp_setattr, nullptr},
        {0, NULL},
};

PyType_Spec PyJavaObject_Spec {
    "remote_input.JavaObject",
    sizeof(PyJavaObject),
    0,
    #if HAS_PYTHON_VERSION(0x030A0000)
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_IMMUTABLETYPE | Py_TPFLAGS_DISALLOW_INSTANTIATION,
    #else
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    #endif
    PyJavaObject_Slots
};

// MARK: EXPORTS

PyTypeObject* PyJavaObject_Type() noexcept
{
    static PyTypeObject* type = reinterpret_cast<PyTypeObject*>(python->PyType_FromSpec(&PyJavaObject_Spec));
    return type;
}

// MARK: Functions

template<typename T>
typename std::enable_if<std::is_same<T, jobject>::value, T>::type from_python_object(PyJavaObject* object)
{
    return PythonUnwrapJavaObject(object);
}

EIOS* python_get_eios(PyJavaObject* object)
{
    return PythonGetEIOS(reinterpret_cast<PyObject*>(object))->native_eios;
}

PyObject* python_create_object(PyJavaObject* self, jobject object) noexcept
{
    return reinterpret_cast<PyObject*>(PythonWrapJavaObject(self->eios, object));
}

PyObject* Python_Reflect_Object(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::string desc = from_python_object<std::string>(args[2]);
    jobject result = Reflect_Object(python_get_eios(self), object, cls.c_str(), field.c_str(), desc.c_str());
    return python_create_object(self, result);
}

PyObject* Python_Reflect_IsSame_Object(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    jobject a = from_python_object<jobject>(self);
    jobject b = from_python_object<jobject>(args[0]);
    jboolean result = Reflect_IsSame_Object(python_get_eios(self), a, b);
    return to_python_object(result);
}

PyObject* Python_Reflect_InstanceOf(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    jboolean result = Reflect_InstanceOf(python_get_eios(self), object, cls.c_str());
    return to_python_object(result);
}

PyObject* Python_JavaObject_Release_Object(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    jobject object = from_python_object<jobject>(self);
    Reflect_Release_Object(python_get_eios(self), object);

    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_JavaObject_Release_Objects(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    std::vector<jobject> array = from_python_array<jobject>(args[0]);
    Reflect_Release_Objects(python_get_eios(self), &array[0], array.size());
    python->PyList_SetSlice(args[0], 0, python->PyList_Size(args[0]), nullptr);

    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_Reflect_Bool(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::string desc = from_python_object<std::string>(args[2]);
    jboolean result = Reflect_Bool(python_get_eios(self), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Char(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::string desc = from_python_object<std::string>(args[2]);
    char result = Reflect_Char(python_get_eios(self), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Byte(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::string desc = from_python_object<std::string>(args[2]);
    std::uint8_t result = Reflect_Byte(python_get_eios(self), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Short(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::string desc = from_python_object<std::string>(args[2]);
    std::int16_t result = Reflect_Short(python_get_eios(self), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Int(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::string desc = from_python_object<std::string>(args[2]);
    std::int32_t result = Reflect_Int(python_get_eios(self), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Long(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::string desc = from_python_object<std::string>(args[2]);
    std::int64_t result = Reflect_Long(python_get_eios(self), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Float(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::string desc = from_python_object<std::string>(args[2]);
    float result = Reflect_Float(python_get_eios(self), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Double(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::string desc = from_python_object<std::string>(args[2]);
    double result = Reflect_Double(python_get_eios(self), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_String(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS* eios = python_get_eios(self);
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::string desc = from_python_object<std::string>(args[2]);

    ReflectionHook hook{object, cls, field, desc};
    return to_python_object(eios->control_center->reflect_string(hook));
}

// MARK: - Array Functions

PyObject* Python_Reflect_Array(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS* eios = python_get_eios(self);
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::string desc = from_python_object<std::string>(args[2]);

    std::size_t array_size = 0;
    jarray array = Reflect_Array_With_Size(eios, object, &array_size, cls.c_str(), field.c_str(), desc.c_str());
    return reinterpret_cast<PyObject*>(PythonWrapJavaArray(PythonGetEIOS(reinterpret_cast<PyObject*>(self)), array, array_size));
}