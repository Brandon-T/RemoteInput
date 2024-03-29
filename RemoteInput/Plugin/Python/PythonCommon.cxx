//
// Created by Brandon on 2022-09-21.
//

#include "PythonCommon.hxx"

PyRemoteInputType GetObjectType(PyObject* object) noexcept
{
    if ((python->Py_IS_TYPE)(object, PyEIOS_Type()))
    {
        return PyRemoteInputType::EIOS;
    }

    if ((python->Py_IS_TYPE)(object, PyJavaObject_Type()))
    {
        return PyRemoteInputType::JAVA_OBJECT;
    }

    if ((python->Py_IS_TYPE)(object, PyJavaArray_Type()))
    {
        return PyRemoteInputType::JAVA_ARRAY;
    }

    return PyRemoteInputType::OTHER;
}

PyObject* PythonWrapEIOS(EIOS* eios) noexcept
{
    if (eios)
    {
        PyEIOS* py_eios = (python->PyObject_New<PyEIOS>)(PyEIOS_Type());
        py_eios->pid = eios->pid;
        py_eios->native_eios = eios;
        return reinterpret_cast<PyObject*>(py_eios);
    }

    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

EIOS* PythonUnwrapEIOS(PyEIOS* eios) noexcept
{
    if (eios)
    {
        return eios->native_eios;
    }
    return nullptr;
}

PyEIOS* PythonGetEIOS(PyObject* object)
{
    PyRemoteInputType type = GetObjectType(object);
    switch (type)
    {
        case PyRemoteInputType::EIOS:
            return reinterpret_cast<PyEIOS*>(object);

        case PyRemoteInputType::JAVA_OBJECT:
            return reinterpret_cast<PyJavaObject*>(object)->eios;

        case PyRemoteInputType::JAVA_ARRAY:
            return reinterpret_cast<PyJavaArray*>(object)->eios;

        default:
            return nullptr;
    }
}

PyJavaObject* PythonWrapJavaObject(PyEIOS* eios, jobject object) noexcept
{
    if (object)
    {
        PyTypeObject* PyJavaObjectType = PyJavaObject_Type();
        PyJavaObject* py_java_object = (python->PyObject_New<PyJavaObject>)(PyJavaObjectType);
        py_java_object->eios = eios;
        py_java_object->object = object;
        (python->Py_INCREF)(reinterpret_cast<PyObject*>(eios));
        return py_java_object;
    }

    (python->Py_INCREF)(python->Py_GetNone_Object());
    return reinterpret_cast<PyJavaObject*>(python->Py_GetNone_Object());
}

jobject PythonUnwrapJavaObject(PyJavaObject* object) noexcept
{
    if (!object)
    {
        return nullptr;
    }

    if (GetObjectType(reinterpret_cast<PyObject*>(object)) != PyRemoteInputType::JAVA_OBJECT)
    {
        return nullptr;
    }

    return object->object;
}

PyJavaArray* PythonWrapJavaArray(PyEIOS* eios, jarray array, std::size_t size) noexcept
{
    if (array)
    {
        PyTypeObject* PyJavaArrayType = PyJavaArray_Type();
        PyJavaArray* py_java_array = (python->PyObject_New<PyJavaArray>)(PyJavaArrayType);
        py_java_array->eios = eios;
        py_java_array->array = array;
        py_java_array->size = size;
        (python->Py_INCREF)(reinterpret_cast<PyObject*>(eios));
        return py_java_array;
    }

    (python->Py_INCREF)(python->Py_GetNone_Object());
    return reinterpret_cast<PyJavaArray*>(python->Py_GetNone_Object());
}

jarray PythonUnwrapJavaArray(PyJavaArray* array) noexcept
{
    if (!array)
    {
        return nullptr;
    }

    if (GetObjectType(reinterpret_cast<PyObject*>(array)) != PyRemoteInputType::JAVA_ARRAY)
    {
        return nullptr;
    }

    return array->array;
}