//
// Created by Brandon on 2023-12-10.
//

#include "PythonJavaArray.hxx"
#include "NativePlugin.hxx"

int PyJavaArray_Clear(PyObject* object)
{
    PyJavaArray* py_java_array = reinterpret_cast<PyJavaArray*>(object);
    (python->Py_CLEAR)(reinterpret_cast<PyObject*>(py_java_array->eios));
    py_java_array->eios = nullptr;
    py_java_array->array = nullptr;
    return 0;
}

void PyJavaArray_Dealloc(PyObject* object)
{
    PyJavaArray* py_java_object = reinterpret_cast<PyJavaArray *>(object);
    if (py_java_object->eios && py_java_object->array)
    {
        Reflect_Release_Object(PythonUnwrapEIOS(py_java_object->eios), py_java_object->array);
    }

    // PyObject_GC_UnTrack(object);
    PyJavaArray_Clear(object);
    //PyObject_Del(object);  // NO GC!
    python->PyObject_Free(object);
}

PyMemberDef PyJavaArray_Members[] = {
        {nullptr}  /* Sentinel */
};

PyGetSetDef PyJavaArray_PropertyMembers[] = {
        {(char*)"size", [](PyObject* object, void *closure) -> PyObject* {
            return object ? python->PyLong_FromLong(reinterpret_cast<PyJavaArray*>(object)->size) : python->PyLong_FromLong(0);
        }, nullptr, PyDoc_STR("Array Size"), nullptr},

        {nullptr}  /* Sentinel */
};

PyMethodDef PyJavaArray_Methods[] = {
        {(char*)"get_1d",          PYTHON_FASTCALL(Python_JavaArray_Get1D), METH_FASTCALL | METH_KEYWORDS,      (char*)""},
        {(char*)"get_2d",          PYTHON_FASTCALL(Python_JavaArray_Get2D), METH_FASTCALL | METH_KEYWORDS,      (char*)""},
        {(char*)"get_3d",          PYTHON_FASTCALL(Python_JavaArray_Get3D), METH_FASTCALL | METH_KEYWORDS,      (char*)""},
        {(char*)"get_4d",          PYTHON_FASTCALL(Python_JavaArray_Get4D), METH_FASTCALL | METH_KEYWORDS,      (char*)""},
        {nullptr}  /* Sentinel */
};

PyType_Slot PyJavaArray_Slots[] = {
        {Py_tp_init, nullptr},
        {Py_tp_members, PyJavaArray_Members},
        {Py_tp_methods, PyJavaArray_Methods},
        {Py_tp_getset, PyJavaArray_PropertyMembers},
        {Py_tp_dealloc, reinterpret_cast<void*>(&PyJavaArray_Dealloc)},
        {Py_tp_clear, reinterpret_cast<void*>(&PyJavaArray_Clear)},
        {Py_tp_doc, static_cast<void*>(const_cast<char*>(PyDoc_STR("JavaArray Structure")))},
        {Py_tp_setattr, nullptr},
        {0, NULL},
};

PyType_Spec PyJavaArray_Spec {
        "remote_input.JavaArray",
        sizeof(PyJavaArray),
        0,
        #if HAS_PYTHON_VERSION(0x030A0000)
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_IMMUTABLETYPE | Py_TPFLAGS_DISALLOW_INSTANTIATION,
        #else
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        #endif
        PyJavaArray_Slots
};

// MARK: EXPORTS

PyObject* read_array_type(PyJavaArray* object, void* array_buffer, ReflectionType type, std::size_t length);

PyTypeObject* PyJavaArray_Type() noexcept
{
    static PyTypeObject* type = reinterpret_cast<PyTypeObject*>(python->PyType_FromSpec(&PyJavaArray_Spec));
    return type;
}

template<typename T>
typename std::enable_if<std::is_same<T, jarray>::value, T>::type from_python_object(PyJavaArray* object)
{
    return PythonUnwrapJavaArray(object);
}

PyObject* python_create_object(PyJavaArray* self, jobject object) noexcept
{
    return reinterpret_cast<PyObject*>(PythonWrapJavaObject(self->eios, object));
}

PyObject* python_create_array(PyJavaArray* self, jarray object, std::size_t size) noexcept
{
    return reinterpret_cast<PyObject*>(PythonWrapJavaArray(self->eios, object, size));
}

EIOS* python_get_eios(PyJavaArray* object)
{
    return PythonGetEIOS(reinterpret_cast<PyObject*>(object))->native_eios;
}

PyObject* Python_JavaArray_Get1D(PyJavaArray* self, PyObject* args[], Py_ssize_t args_length, PyObject* kwnames) noexcept
{
    ReflectionType type = ReflectionType::OBJECT;
    PyObject* index_object = nullptr;
    PyObject* length_object = nullptr;

    static const char* kwlist[] = {"type", "index", "length", nullptr};
    if (!python->PyArg_ParseTupleAndKeywords(reinterpret_cast<PyObject*>(args),
                                             kwnames, "i|N|N", const_cast<char**>(kwlist),
                                             &type, &index_object, &length_object))
    {
        return nullptr;
    }

    EIOS* eios = python_get_eios(self);
    jarray array = from_python_object<jarray>(self);
    std::size_t index = 0;
    std::size_t length = 1;

    if (index_object)
    {
        index = from_python_object<std::size_t>(index_object);
    }

    if (length_object)
    {
        length = from_python_object<std::size_t>(length_object);
    }

    length = length == 0 ? self->size : std::min(length, self->size);
    if (length == 0 || !array || index > length)
    {
        Reflect_Release_Object(eios, array);
        (python->Py_INCREF)(python->Py_GetNone_Object());
        return python->Py_GetNone_Object();
    }

    void* array_buffer = Reflect_Array_Index(eios, array, type, index, length);
    PyObject* result = read_array_type(self, array_buffer, type, length);
    return result;
}

PyObject* Python_JavaArray_Get2D(PyJavaArray* self, PyObject* args[], Py_ssize_t args_length, PyObject* kwnames) noexcept
{
    ReflectionType type = ReflectionType::OBJECT;
    PyObject* x_object = nullptr;
    PyObject* y_object = nullptr;

    static const char* kwlist[] = {"type", "x", "y", nullptr};
    if (!python->PyArg_ParseTupleAndKeywords(reinterpret_cast<PyObject*>(args),
                                             kwnames, "i|NN", const_cast<char**>(kwlist),
                                             &type, &x_object, &y_object))
    {
        return nullptr;
    }

    EIOS* eios = python_get_eios(self);
    jarray array = from_python_object<jarray>(self);

    // Array[x][y]
    if (x_object && y_object)
    {
        std::size_t x = from_python_object<std::size_t>(x_object);
        std::size_t y = from_python_object<std::size_t>(y_object);

        void* array_buffer = Reflect_Array_Index2D(eios, array, type, x, y, 1);
        PyObject* result = read_array_type(self, array_buffer, type, 1);
        return result;
    }

    // Array[][]
    jarray x_buffer = static_cast<jarray>(Reflect_Array_Index(eios, array, ReflectionType::ARRAY, 0, self->size));

    PyObject* x_list = python->PyList_New(self->size);
    for (std::size_t i = 0; i < self->size; ++i)
    {
        std::size_t y_length = Reflect_Array_Size(eios, x_buffer + i);
        void* y_buffer = Reflect_Array_Index(eios, x_buffer + i, type, 0, y_length);
        python->PyList_SetItem(x_list, i, read_array_type(self, y_buffer, type, y_length));
    }
    return x_list;
}

PyObject* Python_JavaArray_Get3D(PyJavaArray* self, PyObject* args[], Py_ssize_t args_length, PyObject* kwnames) noexcept
{
    ReflectionType type = ReflectionType::OBJECT;
    PyObject* x_object = nullptr;
    PyObject* y_object = nullptr;
    PyObject* z_object = nullptr;

    static const char* kwlist[] = {"type", "x", "y", "z", nullptr};
    if (!python->PyArg_ParseTupleAndKeywords(reinterpret_cast<PyObject*>(args),
                                             kwnames, "i|NNN", const_cast<char**>(kwlist),
                                             &type, &x_object, &y_object, &z_object))
    {
        return nullptr;
    }

    EIOS* eios = python_get_eios(self);
    jarray array = from_python_object<jarray>(self);

    // Array[x][y][z]
    if (x_object && y_object && z_object)
    {
        std::size_t x = from_python_object<std::size_t>(x_object);
        std::size_t y = from_python_object<std::size_t>(y_object);
        std::size_t z = from_python_object<std::size_t>(z_object);

        void* array_buffer = Reflect_Array_Index3D(eios, array, type, x, y, z, 1);
        PyObject* result = read_array_type(self, array_buffer, type, 1);
        return result;
    }

    // Array[][][]
    jarray x_buffer = static_cast<jarray>(Reflect_Array_Index(eios, array, ReflectionType::ARRAY, 0, self->size));

    PyObject* x_list = python->PyList_New(self->size);
    for (std::size_t i = 0; i < self->size; ++i)
    {
        std::size_t y_length = Reflect_Array_Size(eios, x_buffer + i);
        jarray y_buffer = static_cast<jarray>(Reflect_Array_Index(eios, x_buffer + i, ReflectionType::ARRAY, 0, y_length));

        PyObject* y_list = python->PyList_New(y_length);
        for (std::size_t j = 0; j < y_length; ++j)
        {
            std::size_t z_length =  Reflect_Array_Size(eios, y_buffer + j);
            void* z_buffer = Reflect_Array_Index(eios, y_buffer + j, type, 0, z_length);
            python->PyList_SetItem(y_list, j, read_array_type(self, z_buffer, type, z_length));
        }

        python->PyList_SetItem(x_list, i, y_list);
    }
    return x_list;
}

PyObject* Python_JavaArray_Get4D(PyJavaArray* self, PyObject* args[], Py_ssize_t args_length, PyObject* kwnames) noexcept
{
    ReflectionType type = ReflectionType::OBJECT;
    PyObject* x_object = nullptr;
    PyObject* y_object = nullptr;
    PyObject* z_object = nullptr;
    PyObject* w_object = nullptr;

    static const char* kwlist[] = {"type", "x", "y", "z", "w", nullptr};
    if (!python->PyArg_ParseTupleAndKeywords(reinterpret_cast<PyObject*>(args),
                                             kwnames, "i|NNNN", const_cast<char**>(kwlist),
                                             &type, &x_object, &y_object, &z_object, &w_object))
    {
        return nullptr;
    }

    EIOS* eios = python_get_eios(self);
    jarray array = from_python_object<jarray>(self);

    // Array[x][y][z][w]
    if (x_object && y_object && z_object && w_object)
    {
        std::size_t x = from_python_object<std::size_t>(x_object);
        std::size_t y = from_python_object<std::size_t>(y_object);
        std::size_t z = from_python_object<std::size_t>(z_object);
        std::size_t w = from_python_object<std::size_t>(z_object);

        void* array_buffer = Reflect_Array_Index4D(eios, array, type, x, y, z, w, 1);
        PyObject* result = read_array_type(self, array_buffer, type, 1);
        return result;
    }

    // Array[][][][]
    jarray x_buffer = static_cast<jarray>(Reflect_Array_Index(eios, array, ReflectionType::ARRAY, 0, self->size));

    PyObject* x_list = python->PyList_New(self->size);
    for (std::size_t i = 0; i < self->size; ++i)
    {
        std::size_t y_length = Reflect_Array_Size(eios, x_buffer + i);
        jarray y_buffer = static_cast<jarray>(Reflect_Array_Index(eios, x_buffer + i, ReflectionType::ARRAY, 0, y_length));

        PyObject* y_list = python->PyList_New(y_length);
        for (std::size_t j = 0; j < y_length; ++j)
        {
            std::size_t z_length =  Reflect_Array_Size(eios, y_buffer + j);
            jarray z_buffer = static_cast<jarray>(Reflect_Array_Index(eios, y_buffer + j, ReflectionType::ARRAY, 0, z_length));

            PyObject* z_list = python->PyList_New(z_length);
            for (std::size_t k = 0; k < z_length; ++k)
            {
                std::size_t w_length =  Reflect_Array_Size(eios, z_buffer + k);
                void* w_buffer = Reflect_Array_Index(eios, z_buffer + k, type, 0, z_length);
                python->PyList_SetItem(z_list, k, read_array_type(self, w_buffer, type, w_length));
            }

            python->PyList_SetItem(y_list, j, read_array_type(self, z_buffer, type, z_length));
        }

        python->PyList_SetItem(x_list, i, y_list);
    }
    return x_list;
}

template<typename T>
PyObject* read_array_type(PyJavaArray* object, void* array, ReflectionType type, std::size_t length)
{
    if constexpr(std::is_same<T, std::string>::value)
    {
        auto read_string = [](void* &ptr) noexcept -> std::string {
            std::size_t length = *static_cast<std::size_t*>(ptr);
            ptr = static_cast<std::size_t*>(ptr) + 1;

            if (length > 0)
            {
                std::string result = std::string(reinterpret_cast<const char*>(ptr), length);
                ptr = static_cast<char*>(ptr) + (result.length() * sizeof(char));
                ptr = static_cast<char*>(ptr) + 1;
                return result;
            }
            return std::string();
        };

        PyObject* list = python->PyList_New(length);
        for (std::size_t i = 0; i < length; ++i)
        {
            python->PyList_SetItem(list, i, to_python_object(read_string(array)));
        }
        return list;
    }
    else if constexpr(std::is_same<T, bool>::value)
    {
        auto read_bool = [](void* &ptr) noexcept -> bool {
            bool result = *static_cast<bool*>(ptr);
            ptr = static_cast<char*>(ptr) + sizeof(bool);
            return result;
        };

        PyObject* list = python->PyList_New(length);
        for (std::size_t i = 0; i < length; ++i)
        {
            python->PyList_SetItem(list, i, to_python_object(read_bool(array)));
        }
        return list;
    }
    else if constexpr(std::is_same<T, jobject>::value)
    {
        auto read_object = [](void* &ptr) noexcept -> jobject {
            jobject result = *static_cast<jobject*>(ptr);
            ptr = static_cast<char*>(ptr) + sizeof(jobject);
            return result;
        };

        PyObject* list = python->PyList_New(length);
        for (std::size_t i = 0; i < length; ++i)
        {
            python->PyList_SetItem(list, i, python_create_object(object, read_object(array)));
        }
        return list;
    }
    else if constexpr(std::is_same<T, jarray>::value)
    {
        auto read_array = [](void* &ptr) noexcept -> jarray {
            jarray result = *static_cast<jarray*>(ptr);
            ptr = static_cast<char*>(ptr) + sizeof(jarray);
            return result;
        };

        PyObject* list = python->PyList_New(length);
        for (std::size_t i = 0; i < length; ++i)
        {
            python->PyList_SetItem(list, i, python_create_array(object, read_array(array)));
        }
        return list;
    }
    else
    {
        std::vector<T> result;
        std::size_t element_size = ControlCenter::reflect_size_for_type(type);
        result.resize(length);

        std::memcpy(&result[0], array, length * element_size);
        return to_python_array(result);

//        std::size_t element_size = ControlCenter::reflect_size_for_type(type);
//        PyObject* array_module = python->PyImport_ImportModule("array");
//        PyObject* array_type = python->PyObject_GetAttrString(array_module, "array");
//        (python->Py_DECREF)(array_module);
//
//        PyObject* result = python->PyObject_CallFunction(array_type, "s[l]", "l", 0);
//        (python->Py_DECREF)(array_type);
//
//        PyObject *pysize = python->PyLong_FromSsize_t(length);
//        PyObject *py_array = python->PyNumber_Multiply(result, pysize);
//        (python->Py_DECREF)(pysize);
//
//        PyObject *buffer_info = python->PyObject_CallMethod(py_array, "buffer_info", "");
//        PyObject* pyaddr = python->PyTuple_GetItem(buffer_info, 0);
//        void *addr = python->PyLong_AsVoidPtr(pyaddr);
//
//        std::memcpy(addr, array, length * element_size);
//        return py_array;
    }
}

PyObject* read_array_type(PyJavaArray* object, void* array_buffer, ReflectionType type, std::size_t length)
{
    switch(type)
    {
        case ReflectionType::CHAR: return read_array_type<char>(object, array_buffer, type, length);
        case ReflectionType::BYTE: return read_array_type<std::uint8_t>(object, array_buffer, type, length);
        case ReflectionType::BOOL: return read_array_type<bool>(object, array_buffer, type, length);
        case ReflectionType::SHORT: return read_array_type<std::int16_t>(object, array_buffer, type, length);
        case ReflectionType::INT: return read_array_type<std::int32_t>(object, array_buffer, type, length);
        case ReflectionType::LONG: return read_array_type<std::int64_t>(object, array_buffer, type, length);
        case ReflectionType::FLOAT: return read_array_type<float>(object, array_buffer, type, length);
        case ReflectionType::DOUBLE: return read_array_type<double>(object, array_buffer, type, length);
        case ReflectionType::STRING: return read_array_type<std::string>(object, array_buffer, type, length);
        case ReflectionType::OBJECT: return read_array_type<jobject>(object, array_buffer, type, length);
        case ReflectionType::ARRAY: return read_array_type<jobject>(object, array_buffer, type, length);
        default: (python->Py_INCREF)(python->Py_GetNone_Object()); return python->Py_GetNone_Object();
    }
}