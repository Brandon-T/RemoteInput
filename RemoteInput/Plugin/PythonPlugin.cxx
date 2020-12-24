//
//  PythonPlugin.cxx
//  RemoteInput
//
//  Created by Brandon on 2020-11-09.
//  Copyright © 2020 XIO. All rights reserved.
//

#include "PythonPlugin.hxx"

#if defined(_WIN32) || defined(_WIN64)
#include "structmember.h"
#elif defined(__linux__) && (defined(__x86_64__) || defined(__i386__))
#include "structmember.h"
#elif defined(__APPLE__)
#include <Python/structmember.h>
#elif defined(__aarch64__) || defined(__arm__)
#include "structmember.h"
#endif

// STRUCTURES
typedef struct {
    PyObject_HEAD
    std::int32_t pid;
    std::int32_t width;
    std::int32_t height;
    EIOS* native_eios;
} PyEIOS;

static PyMemberDef PyEIOSMembers[] = {
        {(char*)"pid", T_INT, offsetof(PyEIOS, pid), READONLY, (char*)"Process ID"},
        {(char*)"width", T_INT, offsetof(PyEIOS, width), READONLY, (char*)"Client Width"},
        {(char*)"height", T_INT, offsetof(PyEIOS, height), READONLY, (char*)"Client Height"},
        {nullptr}  /* Sentinel */
};

static PyGetSetDef PyEIOSPropertyMembers[] = {
        {(char*)"pid", [](PyObject* object, void *closure) -> PyObject* {
            return object ? PyLong_FromLong(reinterpret_cast<EIOS*>(object)->pid) : PyLong_FromLong(-1);
        }, nullptr, (char*)"Process ID", nullptr},

        {(char*)"width", [](PyObject* object, void *closure) -> PyObject* {
            return object ? PyLong_FromLong(reinterpret_cast<EIOS*>(object)->width) : PyLong_FromLong(0);
        }, nullptr, (char*)"Client Width", nullptr},

        {(char*)"height", [](PyObject* object, void *closure) -> PyObject* {
            return object ? PyLong_FromLong(reinterpret_cast<EIOS*>(object)->height) : PyLong_FromLong(0);
        }, nullptr, (char*)"Client Height", nullptr},

        {nullptr}  /* Sentinel */
};

static PyMethodDef PyEIOSMethods[] = {
        {(char*)"Inject", PYTHON_EIOS_Inject, METH_STATIC | METH_VARARGS, (char*)"Injects into a process by name"},
        {(char*)"Inject_PID", PYTHON_EIOS_Inject_PID, METH_STATIC | METH_VARARGS, (char*)"Injects into a process by pid"},
        {(char*)"GetEIOS", PYTHON_Reflect_GetEIOS, METH_STATIC | METH_VARARGS, (char*)"Returns an existing EIOS for the specified pid"},
        {(char*)"Reflect_Object", PYTHON_Reflect_Object, METH_VARARGS, (char*)""},
        {nullptr}  /* Sentinel */
};

#if __cplusplus >= 202002L  //C++20
static PyTypeObject PyEIOSType = {
        PyVarObject_HEAD_INIT(nullptr, 0)
        .tp_name = "remoteinput.EIOS",
        .tp_basicsize = sizeof(PyEIOS),
        .tp_itemsize = 0,
        .tp_dealloc = [](PyObject* object){
            PyEIOS* py_eios = reinterpret_cast<PyEIOS*>(object);
            EIOS_ReleaseTarget(py_eios->native_eios);
            PyObject_GC_UnTrack(object);
            Py_TYPE(object)->tp_clear(object);
            PyObject_Del(object);
        },
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
        .tp_doc = "EIOS Structure",
        .tp_traverse = [](PyObject* object, visitproc visit, void* arg) -> int {
            //Py_VISIT(object->other_py_object);
            return 0;
        },
        .tp_clear = [](PyObject* object) -> int {
            PyEIOS* py_eios = reinterpret_cast<PyEIOS*>(object);
            py_eios->pid = -1;
            py_eios->width = 0;
            py_eios->height = 0;
            py_eios->native_eios = nullptr;
            return 0;
        },
        .tp_methods = PyEIOSMethods,
        .tp_members = PyEIOSMembers,
        .tp_getset = PyEIOSPropertyMembers,
        .tp_new = PyType_GenericNew,
};
#else
static PyTypeObject PyEIOSType = {
        PyVarObject_HEAD_INIT(nullptr, 0)
        "remoteinput.EIOS",
        sizeof(PyEIOS),
        0,
        [](PyObject* object){
            PyEIOS* py_eios = reinterpret_cast<PyEIOS*>(object);
            EIOS_ReleaseTarget(py_eios->native_eios);
            PyObject_GC_UnTrack(object);
            Py_TYPE(object)->tp_clear(object);
            PyObject_Del(object);
        },
        0,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
        "EIOS Structure",
        [](PyObject* object, visitproc visit, void* arg) -> int {
            //Py_VISIT(object->other_py_object);
            return 0;
        },
        [](PyObject* object) -> int {
            PyEIOS* py_eios = reinterpret_cast<PyEIOS*>(object);
            py_eios->pid = -1;
            py_eios->width = 0;
            py_eios->height = 0;
            py_eios->native_eios = nullptr;
            return 0;
        },
        nullptr,
        0,
        nullptr,
        nullptr,
        PyEIOSMethods,
        PyEIOSMembers,
        PyEIOSPropertyMembers,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        0,
        nullptr,
        nullptr,
        PyType_GenericNew,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        0,
        nullptr
};
#endif

typedef struct {
    PyObject_HEAD
    PyObject* eios;
    jobject object;
} PyJavaObject;

static PyMemberDef PyJavaObjectMembers[] = {
    {nullptr}  /* Sentinel */
};

#if __cplusplus >= 202002L  //C++20
static PyTypeObject PyJavaObjectType = {
        PyVarObject_HEAD_INIT(nullptr, 0)
        .tp_name = "remoteinput.JavaObject",
        .tp_basicsize = sizeof(PyJavaObject),
        .tp_itemsize = 0,
        .tp_dealloc = [](PyObject* object){
            PyJavaObject* py_java_object = reinterpret_cast<PyJavaObject*>(object);
            if (py_java_object->eios && py_java_object->object)
            {
                Reflect_Release_Object(reinterpret_cast<PyEIOS *>(py_java_object->eios)->native_eios, py_java_object->object);
            }
            PyObject_GC_UnTrack(object);
            Py_TYPE(object)->tp_clear(object);
            PyObject_Del(object);
        },
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
        .tp_doc = "Java Object",
        .tp_traverse = [](PyObject* object, visitproc visit, void* arg) -> int {
            PyJavaObject* py_java_object = reinterpret_cast<PyJavaObject*>(object);
            if (py_java_object->eios)
            {
                Py_VISIT(py_java_object->eios);
            }
            return 0;
        },
        .tp_clear = [](PyObject* object) -> int {
            PyJavaObject* py_java_object = reinterpret_cast<PyJavaObject*>(object);
            Py_CLEAR(py_java_object->eios);
            py_java_object->eios = nullptr;
            py_java_object->object = nullptr;
            return 0;
        },
        .tp_members = PyJavaObjectMembers,
        .tp_getset = nullptr,
        .tp_new = PyType_GenericNew,
};
#else
static PyTypeObject PyJavaObjectType = {
        PyVarObject_HEAD_INIT(nullptr, 0)
        "remoteinput.JavaObject",
        sizeof(PyJavaObject),
        0,
        [](PyObject* object){
            PyJavaObject* py_java_object = reinterpret_cast<PyJavaObject*>(object);
            if (py_java_object->eios && py_java_object->object)
            {
                Reflect_Release_Object(reinterpret_cast<PyEIOS *>(py_java_object->eios)->native_eios, py_java_object->object);
            }
            PyObject_GC_UnTrack(object);
            Py_TYPE(object)->tp_clear(object);
            PyObject_Del(object);
        },
        0,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
        "Java Object",
        [](PyObject* object, visitproc visit, void* arg) -> int {
            PyJavaObject* py_java_object = reinterpret_cast<PyJavaObject*>(object);
            if (py_java_object->eios)
            {
                Py_VISIT(py_java_object->eios);
            }
            return 0;
        },
        [](PyObject* object) -> int {
            PyJavaObject* py_java_object = reinterpret_cast<PyJavaObject*>(object);
            Py_CLEAR(py_java_object->eios);
            py_java_object->eios = nullptr;
            py_java_object->object = nullptr;
            return 0;
        },
        nullptr,
        0,
        nullptr,
        nullptr,
        nullptr,
        PyJavaObjectMembers,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        0,
        nullptr,
        nullptr,
        PyType_GenericNew,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        0,
        nullptr
};
#endif

// HELPERS

PyObject* PythonWrapObject(PyObject* eios, jobject object)
{
    if (object)
    {
        PyJavaObject* py_java_object = PyObject_New(PyJavaObject, &PyJavaObjectType);
        py_java_object->eios = eios;
        py_java_object->object = object;
        Py_INCREF(eios);
    }
    Py_RETURN_NONE;
}

PyObject* PythonWrapEIOS(EIOS* eios)
{
    if (eios)
    {
        PyEIOS* py_eios = PyObject_New(PyEIOS, &PyEIOSType);
        py_eios->pid = eios->pid;
        py_eios->width = eios->width;
        py_eios->height = eios->height;
        py_eios->native_eios = eios;
    }
    Py_RETURN_NONE;
}

jobject PythonUnwrapObject(PyObject* object)
{
    return object ? reinterpret_cast<PyJavaObject*>(object)->object : nullptr;
}

EIOS* PythonUnwrapEIOS(PyObject* object)
{
    return object ? reinterpret_cast<PyEIOS*>(object)->native_eios : nullptr;
}

template<class T, class... Ts>
struct is_same_of : std::bool_constant<(std::is_same<typename std::remove_cv<T>::type, typename std::remove_cv<Ts>::type>::value || ...)> { };

template<typename T>
std::string python_get_argument_format()
{
    if constexpr(std::is_same<PyObject**, T>::value)
    {
        return "O";
    }
    else if constexpr(std::is_same<const char**, T>::value)
    {
        return "s";
    }
    else if constexpr(std::is_same<std::int32_t*, T>::value)
    {
        return "i";
    }
    else if constexpr(std::is_same<char*, T>::value)
    {
        return "c";
    }
    else if constexpr(std::is_same<Py_ssize_t*, T>::value)
    {
        return "n";
    }
    else
    {
        static_assert("Invalid Python Type");
    }
}

template <typename T, typename... Args>
std::string python_argument_format()
{
    if constexpr (!sizeof...(Args))
    {
        return python_get_argument_format<T>();
    }
    else
    {
        return python_get_argument_format<T>() + python_argument_format<Args...>();
    }
}

template<typename T>
PyObject* to_python_object(const T& value)
{
    if constexpr(is_same_of<T, bool, char, std::int8_t, std::int16_t, std::int32_t>::value)
    {
        return PyLong_FromLong(value);
    }
    else if constexpr(is_same_of<T, std::int64_t>::value)
    {
        return PyLong_FromLongLong(value);
    }
    else if constexpr(is_same_of<T, std::uint8_t, std::uint16_t, std::uint32_t>::value)
    {
        return PyLong_FromUnsignedLong(value);
    }
    else if constexpr(is_same_of<T, std::uint64_t>::value)
    {
        return PyLong_FromUnsignedLongLong(value);
    }
    else if constexpr(is_same_of<T, float, double>::value)
    {
        return PyFloat_FromDouble(value);
    }
    else if constexpr(is_same_of<T, std::string>::value)
    {
        return PyUnicode_FromStringAndSize(value.c_str(), value.size());
    }
    else if constexpr(is_same_of<T, const char*>::value)
    {
        return PyUnicode_FromString(value);
    }
    else
    {
        return nullptr;
    }
}

template<typename R, typename... Args>
PyObject* call_python_function(PyObject* args, R(*fn)(Args...))
{
    std::string format = python_get_argument_format<Args...>();
    std::tuple<PyObject*, const char*, Args...> python_arguments = std::tuple_cat(std::make_tuple(args, format.c_str()), std::tuple<Args...>());

    bool parsed = std::apply([](PyObject* object, const char* format, auto&... args) -> bool {
        return PyArg_ParseTuple(object, format, &args...);
    }, python_arguments);

    if (parsed)
    {
        auto args_tuple = apply([](auto first, auto second, auto... tail) {
            return std::make_tuple(tail...);
        }, args);

        return to_python_object<R>(std::apply(fn, args_tuple));
    }
    return nullptr;
}

std::vector<PyJavaObject*> JObjectArrayToPythonArray(PyObject* eios, std::vector<jobject> objects)
{
    std::vector<PyJavaObject*> result;
    for (const auto& item : objects)
    {
        result.push_back(reinterpret_cast<PyJavaObject*>(PythonWrapObject(eios, item)));
    }
    return result;
}

std::vector<PyJavaObject*> JObjectArrayFromPythonArray(PyObject* object, std::vector<jobject>* objects, bool &error)
{
    std::vector<PyJavaObject*> result;
    PyObject *iter = PyObject_GetIter(object);
    if (!iter)
    {
        error = true;
        return {};
    }

    while (true)
    {
        if (PyObject* next = PyIter_Next(iter))
        {
            if (!PyObject_TypeCheck(next, &PyJavaObjectType))
            {
                error = true;
                return {};
            }
            result.push_back(reinterpret_cast<PyJavaObject*>(next));
            if (objects)
            {
                objects->push_back(reinterpret_cast<PyJavaObject*>(next)->object);
            }
            continue;
        }
        break;
    }
    return result;
}

void PythonReleaseJavaObjects(std::vector<PyJavaObject*> &objects)
{
    for (auto& item : objects)
    {
        Py_DECREF(item->eios);
        item->eios = nullptr;
        item->object = nullptr;
        item = nullptr;
    }
}


// EXPORTS

PyObject* PYTHON_EIOS_Inject(PyObject* self, PyObject* args) noexcept
{
//    const char* process_name = nullptr;
//    if (!PythonParseArguments(args, &process_name))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }
//
//    EIOS_Inject(process_name);
    Py_RETURN_NONE;
}

PyObject* PYTHON_EIOS_Inject_PID(PyObject* self, PyObject* args) noexcept
{
//    std::int32_t pid = -1;
//    if (!PythonParseArguments(args, &pid))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }
//
//    EIOS_Inject_PID(pid);
    Py_RETURN_NONE;
}

PyObject* PYTHON_Reflect_GetEIOS(PyObject* self, PyObject* args) noexcept
{
    std::int32_t pid = -1;
//    if (!PythonParseArguments(args, &pid))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    return PythonWrapEIOS(Reflect_GetEIOS(pid));
}

PyObject* PYTHON_Reflect_Object(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = self;
    PyObject* object = nullptr;

    const char* cls = nullptr;
    const char* field = nullptr;
    const char* desc = nullptr;

//    if (!PythonParseArguments(args, &object, &cls, &field, &desc))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    return PythonWrapObject(eios, Reflect_Object(PythonUnwrapEIOS(eios), PythonUnwrapObject(object), cls, field, desc));
}

PyObject* PYTHON_Reflect_IsSame_Object(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* first = nullptr;
    PyObject* second = nullptr;

//    if (!PythonParseArguments(args, &eios, &first, &second))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    return PyBool_FromLong(Reflect_IsSame_Object(PythonUnwrapEIOS(eios), PythonUnwrapObject(first), PythonUnwrapObject(second)));
}

PyObject* PYTHON_Reflect_InstanceOf(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;
    const char* cls = nullptr;

//    if (!PythonParseArguments(args, &eios, &object, &cls))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    return PyBool_FromLong(Reflect_InstanceOf(PythonUnwrapEIOS(eios), PythonUnwrapObject(object), cls));
}

PyObject* PYTHON_Release_Object(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;

//    if (!PythonParseArguments(args, &eios, &object))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    Reflect_Release_Object(PythonUnwrapEIOS(eios), PythonUnwrapObject(object));
    Py_RETURN_NONE;
}

PyObject* PYTHON_Release_Objects(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* objects = nullptr;

//    if (!PythonParseArguments(args, &eios, &objects))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    bool error = false;
    std::vector<jobject> native_java_objects;
    std::vector<PyJavaObject*> java_objects = JObjectArrayFromPythonArray(objects, &native_java_objects, error);
    if (error)
    {
        PyErr_SetString(PyExc_RuntimeError, "Invalid Array of Objects\n");
        return nullptr;
    }

    Reflect_Release_Objects(PythonUnwrapEIOS(eios), native_java_objects.data(), native_java_objects.size());
    PythonReleaseJavaObjects(java_objects);
    Py_RETURN_NONE;
}

PyObject* PYTHON_Reflect_Bool(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;
    const char* cls = nullptr;
    const char* field = nullptr;
    const char* desc = nullptr;

//    if (!PythonParseArguments(args, &eios, &object, &cls, &field, &desc))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    return PyBool_FromLong(Reflect_Bool(PythonUnwrapEIOS(eios), PythonUnwrapObject(object), cls, field, desc));
}

PyObject* PYTHON_Reflect_Char(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;
    const char* cls = nullptr;
    const char* field = nullptr;
    const char* desc = nullptr;

//    if (!PythonParseArguments(args, &eios, &object, &cls, &field, &desc))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    char c = Reflect_Char(PythonUnwrapEIOS(eios), PythonUnwrapObject(object), cls, field, desc);
    return PyLong_FromLong(c);
    //return PyUnicode_FromStringAndSize(&c, 1);
}

PyObject* PYTHON_Reflect_Byte(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;
    const char* cls = nullptr;
    const char* field = nullptr;
    const char* desc = nullptr;

//    if (!PythonParseArguments(args, &eios, &object, &cls, &field, &desc))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    std::uint8_t b = Reflect_Byte(PythonUnwrapEIOS(eios), PythonUnwrapObject(object), cls, field, desc);
    return PyLong_FromLong(b);
}

PyObject* PYTHON_Reflect_Short(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;
    const char* cls = nullptr;
    const char* field = nullptr;
    const char* desc = nullptr;

//    if (!PythonParseArguments(args, &eios, &object, &cls, &field, &desc))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    std::int16_t i = Reflect_Short(PythonUnwrapEIOS(eios), PythonUnwrapObject(object), cls, field, desc);
    return PyLong_FromLong(i);
}

PyObject* PYTHON_Reflect_Int(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;
    const char* cls = nullptr;
    const char* field = nullptr;
    const char* desc = nullptr;

//    if (!PythonParseArguments(args, &eios, &object, &cls, &field, &desc))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    std::int32_t i = Reflect_Int(PythonUnwrapEIOS(eios), PythonUnwrapObject(object), cls, field, desc);
    return PyLong_FromLong(i);
}

PyObject* PYTHON_Reflect_Long(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;
    const char* cls = nullptr;
    const char* field = nullptr;
    const char* desc = nullptr;

//    if (!PythonParseArguments(args, &eios, &object, &cls, &field, &desc))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    std::int64_t i = Reflect_Long(PythonUnwrapEIOS(eios), PythonUnwrapObject(object), cls, field, desc);
    return PyLong_FromLongLong(i);
}

PyObject* PYTHON_Reflect_Float(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;
    const char* cls = nullptr;
    const char* field = nullptr;
    const char* desc = nullptr;

//    if (!PythonParseArguments(args, &eios, &object, &cls, &field, &desc))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    float f = Reflect_Float(PythonUnwrapEIOS(eios), PythonUnwrapObject(object), cls, field, desc);
    return PyFloat_FromDouble(f);
}

PyObject* PYTHON_Reflect_Double(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;
    const char* cls = nullptr;
    const char* field = nullptr;
    const char* desc = nullptr;

//    if (!PythonParseArguments(args, &eios, &object, &cls, &field, &desc))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    double d = Reflect_Double(PythonUnwrapEIOS(eios), PythonUnwrapObject(object), cls, field, desc);
    return PyFloat_FromDouble(d);
}

PyObject* PYTHON_Reflect_String(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;
    const char* cls = nullptr;
    const char* field = nullptr;
    const char* desc = nullptr;

//    if (!PythonParseArguments(args, &eios, &object, &cls, &field, &desc))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    if (eios)
    {
        ReflectionHook hook{PythonUnwrapObject(object), cls, field, desc};
        std::string result = PythonUnwrapEIOS(eios)->control_center->reflect_string(hook);
        return PyUnicode_FromStringAndSize(result.c_str(), result.size());
    }
    return nullptr;
}

PyObject* PYTHON_Reflect_Array(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;
    const char* cls = nullptr;
    const char* field = nullptr;
    const char* desc = nullptr;

//    if (!PythonParseArguments(args, &eios, &object, &cls, &field, &desc))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    return PythonWrapObject(eios, Reflect_Array(PythonUnwrapEIOS(eios), PythonUnwrapObject(object), cls, field, desc));
}

PyObject* PYTHON_Reflect_Array_With_Size(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;
    const char* cls = nullptr;
    const char* field = nullptr;
    const char* desc = nullptr;

//    if (!PythonParseArguments(args, &eios, &object, &cls, &field, &desc))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    std::size_t size = 0;
    jarray arr = Reflect_Array_With_Size(PythonUnwrapEIOS(eios), PythonUnwrapObject(object), &size, cls, field, desc);
    return PyTuple_Pack(2, PythonWrapObject(eios, arr), PyLong_FromSize_t(size));
}

PyObject* PYTHON_Reflect_Array_Size(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;

//    if (!PythonParseArguments(args, &eios, &object))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }
    return PyLong_FromSize_t(Reflect_Array_Size(PythonUnwrapEIOS(eios), static_cast<jarray>(PythonUnwrapObject(object))));
}

PyObject* PYTHON_Reflect_Array_Index(PyObject* self, PyObject* args) noexcept
{
    PyObject* eios = nullptr;
    PyObject* object = nullptr;
    PyObject* type = nullptr;
    Py_ssize_t index = 0;
    Py_ssize_t length = 0;

//    if (!PythonParseArguments(args, &eios, &object, &type, &index, &length))
//    {
//        PyErr_SetString(PyExc_RuntimeError, "Fail to parse arguments\n");
//        return nullptr;
//    }

    //ReflectionArrayType type, std::size_t index, std::size_t length
    if (eios)
    {
        //return eios->control_center->reflect_array_index(array, type, index, length);
    }
    return nullptr;
}

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
        "remoteinput",
        "Remote Input Module",
        -1,
        RemoteInputMethods,
        nullptr,
        nullptr,
        nullptr,
        nullptr
};

PyMODINIT_FUNC MODINIT(remote_input)()
{
    fprintf(stderr, "LOADED!\n");
    if (PyType_Ready(&PyEIOSType) < 0 || PyType_Ready(&PyJavaObjectType) < 0)
    {
        return nullptr;
    }

    PyObject* module = PyModule_Create(&RemoteInputModule);
    PyModule_AddObject(module, "EIOS", reinterpret_cast<PyObject*>(&PyEIOSType));
    PyModule_AddObject(module, "JavaObject", reinterpret_cast<PyObject*>(&PyJavaObjectType));
    return module;
}
#else
PyMODINIT_FUNC MODINIT(remote_input)()
{
    if (PyType_Ready(&PyEIOSType) < 0)
    {
        return;
    }

    if (PyType_Ready(&PyJavaObjectType) < 0)
    {
        return;
    }

    //PyModule_New("remote_input");
    PyObject* module = Py_InitModule("remote_input", RemoteInputMethods);
    PyModule_AddObject(module, "EIOS", reinterpret_cast<PyObject*>(&PyEIOSType));
    PyModule_AddObject(module, "JavaObject", reinterpret_cast<PyObject*>(&PyJavaObjectType));
}
#endif

