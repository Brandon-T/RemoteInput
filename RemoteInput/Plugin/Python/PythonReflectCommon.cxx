//
// Created by Brandon on 2023-02-13.
//

#include "PythonReflectCommon.hxx"
#include "PythonEIOS.hxx"
#include "PythonJavaObject.hxx"
#include "NativePlugin.hxx"
#include <cmath>

PyObject* Python_Reflect_Object(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 4)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 4, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'field: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[3], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'description: string'", error.c_str());
        return nullptr;
    }
    #endif

    jobject object = from_python_object<jobject>(args[0]);
    std::string cls = from_python_object<std::string>(args[1]);
    std::string field = from_python_object<std::string>(args[2]);
    std::string desc = from_python_object<std::string>(args[3]);
    jobject result = Reflect_Object(PythonUnwrapEIOS(PythonUnwrapEIOS(self)), object, cls.c_str(), field.c_str(), desc.c_str());
    return PythonWrapJavaObject(reinterpret_cast<PyReflectAPI*>(self), result);
}

PyObject* Python_Reflect_IsSame_Object(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 2)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 2, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'a: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<jobject>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'b: JavaObject'", error.c_str());
        return nullptr;
    }
    #endif

    jobject a = PythonUnwrapJavaObject(args[0]);
    jobject b = PythonUnwrapJavaObject(args[1]);
    jboolean result = Reflect_IsSame_Object(PythonUnwrapEIOS(PythonUnwrapEIOS(self)), a, b);
    return to_python_object(result);
}

PyObject* Python_Reflect_InstanceOf(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 2)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 2, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: String'", error.c_str());
        return nullptr;
    }
    #endif

    jobject object = PythonUnwrapJavaObject(args[0]);
    std::string cls = from_python_object<std::string>(args[1]);
    jboolean result = Reflect_InstanceOf(PythonUnwrapEIOS(PythonUnwrapEIOS(self)), object, cls.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Release_Object(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }
    #endif

    jobject object = PythonUnwrapJavaObject(args[0]);
    Reflect_Release_Object(PythonUnwrapEIOS(PythonUnwrapEIOS(self)), object);

    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_Reflect_Release_Objects(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<std::vector<jobject>>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'objects: List<JavaObject>'", error.c_str());
        return nullptr;
    }
    #endif

    std::vector<jobject> array = from_python_array<jobject>(args[0]);
    Reflect_Release_Objects(PythonUnwrapEIOS(PythonUnwrapEIOS(self)), &array[0], array.size());
    python->PyList_SetSlice(args[0], 0, python->PyList_Size(args[0]), nullptr);

    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_Reflect_Bool(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 4)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 4, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'field: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[3], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'description: string'", error.c_str());
        return nullptr;
    }
    #endif

    jobject object = from_python_object<jobject>(args[0]);
    std::string cls = from_python_object<std::string>(args[1]);
    std::string field = from_python_object<std::string>(args[2]);
    std::string desc = from_python_object<std::string>(args[3]);
    jboolean result = Reflect_Bool(PythonUnwrapEIOS(PythonUnwrapEIOS(self)), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Char(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 4)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 4, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'field: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[3], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'description: string'", error.c_str());
        return nullptr;
    }
    #endif

    jobject object = from_python_object<jobject>(args[0]);
    std::string cls = from_python_object<std::string>(args[1]);
    std::string field = from_python_object<std::string>(args[2]);
    std::string desc = from_python_object<std::string>(args[3]);
    char result = Reflect_Char(PythonUnwrapEIOS(PythonUnwrapEIOS(self)), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Byte(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 4)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 4, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'field: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[3], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'description: string'", error.c_str());
        return nullptr;
    }
    #endif

    jobject object = from_python_object<jobject>(args[0]);
    std::string cls = from_python_object<std::string>(args[1]);
    std::string field = from_python_object<std::string>(args[2]);
    std::string desc = from_python_object<std::string>(args[3]);
    std::uint8_t result = Reflect_Byte(PythonUnwrapEIOS(PythonUnwrapEIOS(self)), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Short(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 4)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 4, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'field: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[3], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'description: string'", error.c_str());
        return nullptr;
    }
    #endif

    jobject object = from_python_object<jobject>(args[0]);
    std::string cls = from_python_object<std::string>(args[1]);
    std::string field = from_python_object<std::string>(args[2]);
    std::string desc = from_python_object<std::string>(args[3]);
    std::int16_t result = Reflect_Short(PythonUnwrapEIOS(PythonUnwrapEIOS(self)), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Int(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 4)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 4, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'field: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[3], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'description: string'", error.c_str());
        return nullptr;
    }
    #endif

    jobject object = from_python_object<jobject>(args[0]);
    std::string cls = from_python_object<std::string>(args[1]);
    std::string field = from_python_object<std::string>(args[2]);
    std::string desc = from_python_object<std::string>(args[3]);
    std::int32_t result = Reflect_Int(PythonUnwrapEIOS(PythonUnwrapEIOS(self)), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Long(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 4)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 4, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'field: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[3], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'description: string'", error.c_str());
        return nullptr;
    }
    #endif

    jobject object = from_python_object<jobject>(args[0]);
    std::string cls = from_python_object<std::string>(args[1]);
    std::string field = from_python_object<std::string>(args[2]);
    std::string desc = from_python_object<std::string>(args[3]);
    std::int64_t result = Reflect_Long(PythonUnwrapEIOS(PythonUnwrapEIOS(self)), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Float(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 4)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 4, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'field: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[3], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'description: string'", error.c_str());
        return nullptr;
    }
    #endif

    jobject object = from_python_object<jobject>(args[0]);
    std::string cls = from_python_object<std::string>(args[1]);
    std::string field = from_python_object<std::string>(args[2]);
    std::string desc = from_python_object<std::string>(args[3]);
    float result = Reflect_Float(PythonUnwrapEIOS(PythonUnwrapEIOS(self)), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_Double(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 4)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 4, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'field: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[3], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'description: string'", error.c_str());
        return nullptr;
    }
    #endif

    jobject object = from_python_object<jobject>(args[0]);
    std::string cls = from_python_object<std::string>(args[1]);
    std::string field = from_python_object<std::string>(args[2]);
    std::string desc = from_python_object<std::string>(args[3]);
    double result = Reflect_Double(PythonUnwrapEIOS(PythonUnwrapEIOS(self)), object, cls.c_str(), field.c_str(), desc.c_str());
    return to_python_object(result);
}

PyObject* Python_Reflect_String(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 4)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 4, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'field: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[3], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'description: string'", error.c_str());
        return nullptr;
    }
    #endif

    jobject object = from_python_object<jobject>(args[0]);
    std::string cls = from_python_object<std::string>(args[1]);
    std::string field = from_python_object<std::string>(args[2]);
    std::string desc = from_python_object<std::string>(args[3]);
    EIOS* eios = PythonUnwrapEIOS(PythonUnwrapEIOS(self));

    if (eios)
    {
        ReflectionHook hook{object, cls, field, desc};
        return to_python_object(eios->control_center->reflect_string(hook));
    }

    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

// MARK: - Array Functions

template<typename T>
PyObject* read_array_type(PyReflectAPI* api, void* array, ReflectionType type, std::size_t length)
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

        std::size_t element_size = ControlCenter::reflect_size_for_type(type);
        std::vector<std::string> result;
        result.reserve(length);

        for (std::size_t i = 0; i < length; ++i)
        {
            result.push_back(read_string(array));
        }

        return to_python_array(result);
    }
    else if constexpr(std::is_same<T, bool>::value)
    {
        auto read_bool = [](void* &ptr) noexcept -> bool {
            bool result = *static_cast<bool*>(ptr);
            ptr = static_cast<char*>(ptr) + sizeof(bool);
            return result;
        };

        std::vector<T> result;
        result.resize(length);
        for (std::size_t i = 0; i < length; ++i)
        {
            result[i] = read_bool(array);
        }
        return to_python_array(result);
    }
    else if constexpr(std::is_same<T, jobject>::value)
    {
        std::vector<T> result;
        result.resize(length);
        std::size_t element_size = ControlCenter::reflect_size_for_type(type);

        std::memcpy(&result[0], array, length * element_size);

        PyObject* list = python->PyList_New(result.size());
        for (std::size_t i = 0; i < result.size(); ++i)
        {
            python->PyList_SetItem(list, i, PythonWrapJavaObject(api, result[i]));
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

PyObject* read_array_type(PyReflectAPI* api, void* array_buffer, ReflectionType type, std::size_t length)
{
    switch(type)
    {
        case ReflectionType::CHAR: return read_array_type<char>(api, array_buffer, type, length);
        case ReflectionType::BYTE: return read_array_type<std::uint8_t>(api, array_buffer, type, length);
        case ReflectionType::BOOL: return read_array_type<bool>(api, array_buffer, type, length);
        case ReflectionType::SHORT: return read_array_type<std::int16_t>(api, array_buffer, type, length);
        case ReflectionType::INT: return read_array_type<std::int32_t>(api, array_buffer, type, length);
        case ReflectionType::LONG: return read_array_type<std::int64_t>(api, array_buffer, type, length);
        case ReflectionType::FLOAT: return read_array_type<float>(api, array_buffer, type, length);
        case ReflectionType::DOUBLE: return read_array_type<double>(api, array_buffer, type, length);
        case ReflectionType::STRING: return read_array_type<std::string>(api, array_buffer, type, length);
        case ReflectionType::OBJECT: return read_array_type<jobject>(api, array_buffer, type, length);
        default: (python->Py_INCREF)(python->Py_GetNone_Object()); return python->Py_GetNone_Object();
    }
}

PyObject* Python_Reflect_Array(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 7)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 7, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'field: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[3], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'description: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<typedef std::underlying_type<ReflectionType>::type>(args[4], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'type: ReflectType'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::size_t>(args[5], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'index: size_t'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::size_t>(args[6], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'size: size_t'", error.c_str());
        return nullptr;
    }
    #endif

    EIOS* eios = PythonUnwrapEIOS(PythonUnwrapEIOS(self));
    jobject object = from_python_object<jobject>(args[0]);

    std::string cls = from_python_object<std::string>(args[1]);
    std::string field = from_python_object<std::string>(args[2]);
    std::string desc = from_python_object<std::string>(args[3]);
    ReflectionType type = static_cast<ReflectionType>(from_python_object<std::int32_t>(args[4]));
    std::size_t index = from_python_object<std::size_t>(args[5]);
    std::size_t length = from_python_object<std::size_t>(args[6]);

    std::size_t array_size = 0;
    jarray array = Reflect_Array_With_Size(eios, object, &array_size, cls.c_str(), field.c_str(), desc.c_str());
    if (array_size == 0 || !array || index > array_size)
    {
        Reflect_Release_Object(eios, array);

        (python->Py_INCREF)(python->Py_GetNone_Object());
        return python->Py_GetNone_Object();
    }

    length = length == 0 ? array_size : std::min(length, array_size);

    void* array_buffer = Reflect_Array_Index(eios, array, type, index, length);
    PyObject* result = read_array_type(self, array_buffer, type, length);
    Reflect_Release_Object(eios, array);
    return result;
}

PyObject* Python_Reflect_Array2D(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 8)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 8, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'field: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[3], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'description: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<typedef std::underlying_type<ReflectionType>::type>(args[4], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'type: ReflectType'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::size_t>(args[5], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'x: size_t'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::size_t>(args[6], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'y: size_t'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::size_t>(args[7], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'size: size_t'", error.c_str());
        return nullptr;
    }
    #endif

    EIOS* eios = PythonUnwrapEIOS(PythonUnwrapEIOS(self));
    jobject object = from_python_object<jobject>(args[0]);

    std::string cls = from_python_object<std::string>(args[1]);
    std::string field = from_python_object<std::string>(args[2]);
    std::string desc = from_python_object<std::string>(args[3]);
    ReflectionType type = static_cast<ReflectionType>(from_python_object<std::int32_t>(args[4]));
    std::size_t x = from_python_object<std::size_t>(args[5]);
    std::size_t y = from_python_object<std::size_t>(args[6]);
    std::size_t length = from_python_object<std::size_t>(args[7]);

    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_Reflect_Array3D(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 9)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 9, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'field: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[3], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'description: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<typedef std::underlying_type<ReflectionType>::type>(args[4], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'type: ReflectType'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::size_t>(args[5], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'x: size_t'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::size_t>(args[6], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'y: size_t'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::size_t>(args[7], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'z: size_t'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::size_t>(args[8], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'size: size_t'", error.c_str());
        return nullptr;
    }
    #endif

    EIOS* eios = PythonUnwrapEIOS(PythonUnwrapEIOS(self));
    jobject object = from_python_object<jobject>(args[0]);

    std::string cls = from_python_object<std::string>(args[1]);
    std::string field = from_python_object<std::string>(args[2]);
    std::string desc = from_python_object<std::string>(args[3]);
    ReflectionType type = static_cast<ReflectionType>(from_python_object<std::int32_t>(args[4]));
    std::size_t x = from_python_object<std::size_t>(args[5]);
    std::size_t y = from_python_object<std::size_t>(args[6]);
    std::size_t z = from_python_object<std::size_t>(args[7]);
    std::size_t length = from_python_object<std::size_t>(args[8]);

    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_Reflect_Array_Indices(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 6)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 6, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<jobject>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'object: JavaObject'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'class: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'field: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::string>(args[3], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'description: string'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<typedef std::underlying_type<ReflectType>::type>(args[4], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'type: ReflectType'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<std::vector<std::int32_t>>(args[5], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'index: List<UInt32>'", error.c_str());
        return nullptr;
    }
    #endif

    EIOS* eios = PythonUnwrapEIOS(PythonUnwrapEIOS(self));
    jobject object = from_python_object<jobject>(args[0]);

    std::string cls = from_python_object<std::string>(args[1]);
    std::string field = from_python_object<std::string>(args[2]);
    std::string desc = from_python_object<std::string>(args[3]);
    ReflectionType type = static_cast<ReflectionType>(from_python_object<std::int32_t>(args[4]));
    std::vector<std::int32_t> indices = from_python_array<std::int32_t>(args[5]);

    jarray array = Reflect_Array(eios, object, cls.c_str(), field.c_str(), desc.c_str());
    void* array_buffer = Reflect_Array_Indices(eios, array, type, &indices[0], indices.size());
    Reflect_Release_Object(eios, array);
    return read_array_type(self, array_buffer, type, indices.size());
}

// MARK: - Array2D Functions

PyObject* Python_Reflect_Array_Indices2D(PyReflectAPI* self, PyObject* args[], Py_ssize_t args_length) noexcept
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