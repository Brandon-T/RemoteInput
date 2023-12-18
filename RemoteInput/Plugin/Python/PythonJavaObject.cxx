//
// Created by Brandon on 2022-09-21.
//

#include "PythonJavaObject.hxx"
#include "NativePlugin.hxx"

int PyJavaObject_Clear(PyObject* object)
{
    PyJavaObject* py_java_object = reinterpret_cast<PyJavaObject*>(object);
    if (py_java_object->eios)
    {
        (python->Py_CLEAR)(reinterpret_cast<PyObject*>(py_java_object->eios));
        py_java_object->eios = nullptr;
        py_java_object->object = nullptr;
    }
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
        {(char*)"eios", [](PyObject* object, void *closure) -> PyObject* {
            if (object)
            {
                PyObject* eios = reinterpret_cast<PyObject*>(PythonGetEIOS(object));
                (python->Py_INCREF)(eios);
                return eios;
            }

            (python->Py_INCREF)(python->Py_GetNone_Object());
            return python->Py_GetNone_Object();
        }, nullptr, PyDoc_STR("EIOS Object"), nullptr},

        {nullptr}  /* Sentinel */
};

PyMethodDef PyJavaObject_Methods[] = {
        {(char*)"reflect_object",            PYTHON_FASTCALL(Python_Reflect_Object), METH_FASTCALL,      (char*)""},
        {(char*)"is_same_object",            PYTHON_FASTCALL(Python_Reflect_IsSame_Object), METH_FASTCALL,      (char*)""},
        {(char*)"is_instance_of",            PYTHON_FASTCALL(Python_Reflect_InstanceOf), METH_FASTCALL,      (char*)""},
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
        {(char*)"release",            PYTHON_FASTCALL(Python_JavaObject_Release_Object), METH_FASTCALL,      (char*)""},
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
    PyEIOS* eios = PythonGetEIOS(reinterpret_cast<PyObject*>(self));
    return reinterpret_cast<PyObject*>(PythonWrapJavaObject(eios, object));
}

PyObject* Python_Reflect_Object(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS* eios = python_get_eios(self);
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::string desc = from_python_object<std::string>(args[2]);
    jobject result = eios->control_center->reflect_object({object, cls, field, desc});
    return python_create_object(self, result);
}

PyObject* Python_Reflect_IsSame_Object(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS* eios = python_get_eios(self);
    jobject a = from_python_object<jobject>(self);
    jobject b = from_python_object<jobject>(args[0]);
    jboolean result = eios->control_center->reflect_is_objects_equal(a, b);
    return to_python_object(result);
}

PyObject* Python_Reflect_InstanceOf(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS* eios = python_get_eios(self);
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    jboolean result = eios->control_center->reflect_instance_of(object, cls);
    return to_python_object(result);
}

PyObject* Python_Reflect_Bool(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS* eios = python_get_eios(self);
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    jboolean result = eios->control_center->reflect_boolean({object, cls, field, "Z"});
    return to_python_object(result);
}

PyObject* Python_Reflect_Char(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS* eios = python_get_eios(self);
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    char result = eios->control_center->reflect_char({object, cls, field, "C"});
    return to_python_object(result);
}

PyObject* Python_Reflect_Byte(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS* eios = python_get_eios(self);
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::uint8_t result = eios->control_center->reflect_byte({object, cls, field, "B"});
    return to_python_object(result);
}

PyObject* Python_Reflect_Short(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS* eios = python_get_eios(self);
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::int16_t result = eios->control_center->reflect_short({object, cls, field, "S"});
    return to_python_object(result);
}

PyObject* Python_Reflect_Int(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS* eios = python_get_eios(self);
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::int32_t result = eios->control_center->reflect_int({object, cls, field, "I"});
    return to_python_object(result);
}

PyObject* Python_Reflect_Long(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS* eios = python_get_eios(self);
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::int64_t result = eios->control_center->reflect_long({object, cls, field, "J"});
    return to_python_object(result);
}

PyObject* Python_Reflect_Float(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS* eios = python_get_eios(self);
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    float result = eios->control_center->reflect_float({object, cls, field, "F"});
    return to_python_object(result);
}

PyObject* Python_Reflect_Double(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS* eios = python_get_eios(self);
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    double result = eios->control_center->reflect_double({object, cls, field, "D"});
    return to_python_object(result);
}

PyObject* Python_Reflect_String(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS* eios = python_get_eios(self);
    jobject object = from_python_object<jobject>(self);
    std::string cls = from_python_object<std::string>(args[0]);
    std::string field = from_python_object<std::string>(args[1]);
    std::string result = eios->control_center->reflect_string({object, cls, field, "Ljava/lang/String;"});
    return to_python_object(result);
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
    jarray array = eios->control_center->reflect_array({object, cls, field, desc}, &array_size);
    return reinterpret_cast<PyObject*>(PythonWrapJavaArray(PythonGetEIOS(reinterpret_cast<PyObject*>(self)), array, array_size));
}

PyObject* Python_JavaObject_Release_Object(PyJavaObject* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    if (self->eios && self->object)
    {
        EIOS* eios = python_get_eios(self);
        jobject object = from_python_object<jobject>(self);
        eios->control_center->reflect_release_object(object);
        PyJavaObject_Clear(reinterpret_cast<PyObject*>(self));
    }

    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}