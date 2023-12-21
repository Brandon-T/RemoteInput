//
// Created by Brandon on 2022-09-21.
//

#include "PythonEIOS.hxx"
#include "NativePlugin.hxx"
#include "PythonJavaObject.hxx"

#include <sstream>
#include <ios>
#include <iomanip>
#include <stack>

int PyEIOS_Clear(PyObject* object)
{
    PyEIOS* py_eios = reinterpret_cast<PyEIOS*>(object);
    py_eios->pid = -1;
    py_eios->native_eios = nullptr;
    return 0;
}

void PyEIOS_Dealloc(PyObject* object)
{
    PyEIOS* py_eios = reinterpret_cast<PyEIOS*>(object);

    EIOS_ReleaseTarget(py_eios->native_eios);
    // PyObject_GC_UnTrack(object);
    PyEIOS_Clear(object);
    //PyObject_Del(object);  // NO GC!
    python->PyObject_Free(object);
}

PyObject* PyEIOS_Str(PyObject* object)
{
    PyEIOS* py_eios = reinterpret_cast<PyEIOS*>(object);
    std::ostringstream stream;

    std::ios state(nullptr);
    state.copyfmt(stream);

    stream << std::setfill('0') << std::uppercase << std::hex;
    stream << "EIOS(";
    stream << "0x" << reinterpret_cast<std::uintptr_t>(object);
    stream << "): ";
    stream.copyfmt(state);

    stream << "{"<<"\n";
    stream<< "    pid: " << py_eios->pid << "\n";
    stream<< "}";

    std::string result = stream.str();
    return python->PyUnicode_FromStringAndSize(result.c_str(), result.size());
}

PyMemberDef PyEIOS_Members[] = {
        //{(char*)"pid", T_INT, offsetof(PyEIOS, pid), READONLY, PyDoc_STR("Process ID")},
        //{(char*)"width", T_INT, offsetof(PyEIOS, width), READONLY, PyDoc_STR("Client Width")},
        //{(char*)"height", T_INT, offsetof(PyEIOS, height), READONLY, PyDoc_STR("Client Height")},
        {nullptr}  /* Sentinel */
};

PyGetSetDef PyEIOS_PropertyMembers[] = {
        {(char*)"pid", [](PyObject* object, void *closure) -> PyObject* {
            return object ? python->PyLong_FromLong(reinterpret_cast<PyEIOS*>(object)->pid) : python->PyLong_FromLong(-1);
        }, nullptr, PyDoc_STR("Process ID"), nullptr},

        {(char*)"dimensions", [](PyObject* object, void *closure) -> PyObject* {
            if (object)
            {
                return Python_EIOS_GetTargetDimensions(reinterpret_cast<PyEIOS*>(object), nullptr, 0);
            }
            return python->PyTuple_Pack(2, python->PyLong_FromLong(0), python->PyLong_FromLong(0));
        }, nullptr, PyDoc_STR("Client Dimensions"), nullptr},

        {nullptr}  /* Sentinel */
};

PyMethodDef PyEIOS_Methods[] = {
        {(char*)"inject",                 PYTHON_FASTCALL(Python_EIOS_Inject),               METH_FASTCALL | METH_STATIC, PyDoc_STR("inject('process_name', /)\n--\n\nInjects into a process by name")},
        {(char*)"inject_pid",             PYTHON_FASTCALL(Python_EIOS_Inject_PID), METH_FASTCALL | METH_STATIC,           PyDoc_STR("inject_pid(process_pid)\n--\n\nInjects into a process by PID")},
        {(char*)"from_pid",               PYTHON_FASTCALL(Python_EIOS_From_PID), METH_FASTCALL | METH_STATIC,             PyDoc_STR("Gets an existing EIOS by PID")},
        {(char*)"request_target",         PYTHON_FASTCALL(Python_EIOS_RequestTarget), METH_FASTCALL | METH_STATIC,        PyDoc_STR("Requests an EIOS by PID, and Pairs to it if not already paired")},
        {(char*)"get_target_dimensions",  PYTHON_FASTCALL(Python_EIOS_GetTargetDimensions),  METH_FASTCALL, PyDoc_STR("Returns an existing EIOS for the specified pid")},
        {(char*)"get_image_format",       PYTHON_FASTCALL(Python_EIOS_GetImageFormat), METH_FASTCALL, (char*)""},
        {(char*)"set_image_format",       PYTHON_FASTCALL(Python_EIOS_SetImageFormat), METH_FASTCALL, (char*)""},
        {(char*)"get_image_buffer",       PYTHON_FASTCALL(Python_EIOS_GetImageBuffer),       METH_FASTCALL,          (char*)""},
        {(char*)"get_debug_image_buffer", PYTHON_FASTCALL(Python_EIOS_GetDebugImageBuffer),  METH_FASTCALL,     (char*)""},
        {(char*)"set_graphics_debugging", PYTHON_FASTCALL(Python_EIOS_SetGraphicsDebugging), METH_FASTCALL,    (char*)""},
        {(char*)"has_focus",              PYTHON_FASTCALL(Python_EIOS_HasFocus),             METH_FASTCALL,                (char*)""},
        {(char*)"gain_focus",             PYTHON_FASTCALL(Python_EIOS_GainFocus),            METH_FASTCALL,               (char*)""},
        {(char*)"lose_focus",             PYTHON_FASTCALL(Python_EIOS_LoseFocus), METH_FASTCALL,               (char*)""},
        {(char*)"is_keyboard_input_enabled", PYTHON_FASTCALL(Python_EIOS_IsKeyboardInputEnabled), METH_FASTCALL,  (char*)""},
        {(char*)"set_keyboard_input_enabled", PYTHON_FASTCALL(Python_EIOS_SetKeyboardInputEnabled), METH_FASTCALL, (char*)""},
        {(char*)"is_mouse_input_enabled",     PYTHON_FASTCALL(Python_EIOS_IsMouseInputEnabled), METH_FASTCALL,     (char*)""},
        {(char*)"set_mouse_input_enabled",    PYTHON_FASTCALL(Python_EIOS_SetMouseInputEnabled), METH_FASTCALL,    (char*)""},
        {(char*)"get_mouse_position",         PYTHON_FASTCALL(Python_EIOS_GetMousePosition), METH_FASTCALL,        (char*)""},
        {(char*)"get_real_mouse_position",    PYTHON_FASTCALL(Python_EIOS_GetRealMousePosition), METH_FASTCALL,    (char*)""},
        {(char*)"move_mouse",                 PYTHON_FASTCALL(Python_EIOS_MoveMouse), METH_FASTCALL,               (char*)""},
        {(char*)"hold_mouse",                 PYTHON_FASTCALL(Python_EIOS_HoldMouse), METH_FASTCALL,               (char*)""},
        {(char*)"release_mouse",              PYTHON_FASTCALL(Python_EIOS_ReleaseMouse), METH_FASTCALL,            (char*)""},
        {(char*)"scroll_mouse",               PYTHON_FASTCALL(Python_EIOS_ScrollMouse), METH_FASTCALL,             (char*)""},
        {(char*)"is_mouse_button_held",       PYTHON_FASTCALL(Python_EIOS_IsMouseButtonHeld), METH_FASTCALL,       (char*)""},
        {(char*)"send_string",                PYTHON_FASTCALL(Python_EIOS_SendString), METH_FASTCALL,              (char*)""},
        {(char*)"hold_key",                   PYTHON_FASTCALL(Python_EIOS_HoldKey),              METH_FASTCALL, (char*)""},
        {(char*)"release_key",                PYTHON_FASTCALL(Python_EIOS_ReleaseKey),           METH_FASTCALL, (char*)""},
        {(char*)"is_key_held",                PYTHON_FASTCALL(Python_EIOS_IsKeyHeld),            METH_FASTCALL, (char*)""},
        {(char*)"get_keyboard_speed",         PYTHON_FASTCALL(Python_EIOS_GetKeyboardSpeed),     METH_FASTCALL, (char*)""},
        {(char*)"set_keyboard_speed",         PYTHON_FASTCALL(Python_EIOS_SetKeyboardSpeed),  METH_FASTCALL, (char*)""},
        {(char*)"get_keyboard_repeat_delay",  PYTHON_FASTCALL(Python_EIOS_GetKeyboardRepeatDelay), METH_FASTCALL, (char*)""},
        {(char*)"set_keyboard_repeat_delay",  PYTHON_FASTCALL(Python_EIOS_SetKeyboardRepeatDelay), METH_FASTCALL, (char*)""},
        {(char*)"get_clients_pids",         PYTHON_FASTCALL(Python_EIOS_GetClientsPIDs),   METH_FASTCALL | METH_STATIC,      (char*)""},
        {(char*)"pair_client_pid",           PYTHON_FASTCALL(Python_EIOS_PairClient_PID),        METH_FASTCALL | METH_STATIC, (char*)""},
        {(char*)"kill_client_pid",           PYTHON_FASTCALL(Python_EIOS_KillClientPID),     METH_FASTCALL | METH_STATIC, (char*)""},
        {(char*)"kill_client",               PYTHON_FASTCALL(Python_EIOS_KillClient),        METH_FASTCALL, (char*)""},

        {(char*)"reflect_object",            PYTHON_FASTCALL(Python_Reflect_Object), METH_FASTCALL,      (char*)""},
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
        {(char*)"release_objects",            PYTHON_FASTCALL(Python_Reflect_Release_Objects), METH_FASTCALL,      (char*)""},
        {nullptr}  /* Sentinel */
};

PyType_Slot PyEIOS_Slots[] = {
        {Py_tp_init, nullptr},
        {Py_tp_members, PyEIOS_Members},
        {Py_tp_methods, PyEIOS_Methods},
        {Py_tp_getset, PyEIOS_PropertyMembers},
        {Py_tp_dealloc, reinterpret_cast<void*>(&PyEIOS_Dealloc)},
        {Py_tp_clear, reinterpret_cast<void*>(&PyEIOS_Clear)},
        {Py_tp_str, reinterpret_cast<void*>(&PyEIOS_Str)},
        {Py_tp_doc, static_cast<void*>(const_cast<char*>(PyDoc_STR("EIOS Structure")))},
        {0, NULL},
};

PyType_Spec PyEIOS_Spec {
        "remote_input.EIOS",
        sizeof(PyEIOS),
        0,
        #if HAS_PYTHON_VERSION(0x030A0000)
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_IMMUTABLETYPE | Py_TPFLAGS_DISALLOW_INSTANTIATION,
        #else
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        #endif
        PyEIOS_Slots
};

// MARK: EXPORTS

PyTypeObject* PyEIOS_Type() noexcept
{
    static PyTypeObject* type = reinterpret_cast<PyTypeObject*>(python->PyType_FromSpec(&PyEIOS_Spec));
    return type;
}

EIOS* python_get_eios(PyEIOS* eios)
{
    return PythonGetEIOS(reinterpret_cast<PyObject*>(eios))->native_eios;
}

PyObject* Python_EIOS_Inject(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<std::string>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'process_name: string'", error.c_str());
        return nullptr;
    }
    #endif

    std::string process_name = from_python_object<std::string>(args[0]);
    EIOS_Inject(process_name.c_str());
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_Inject_PID(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<std::int32_t>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'process_id: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    std::int32_t pid = from_python_object<std::int32_t>(args[0]);
    EIOS_Inject_PID(pid);
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_From_PID(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<std::int32_t>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'process_id: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    std::int32_t pid = from_python_object<std::int32_t>(args[0]);
    return PythonWrapEIOS(EIOS_From_PID(pid));
}

PyObject* Python_EIOS_RequestTarget(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<std::int32_t>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'process_id: String'", error.c_str());
        return nullptr;
    }
    #endif

    std::string pid = from_python_object<std::string>(args[0]);
    return PythonWrapEIOS(EIOS_RequestTarget(pid.c_str()));
}

PyObject* Python_EIOS_GetTargetDimensions(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 0)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 0, args_length);
        return nullptr;
    }
    #endif

    std::int32_t width = 0;
    std::int32_t height = 0;
    EIOS_GetTargetDimensions(python_get_eios(self), &width, &height);
    return python->PyTuple_Pack(2, python->PyLong_FromLong(width), python->PyLong_FromLong(height));
}

PyObject* Python_EIOS_GetImageFormat(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    ImageFormat format = EIOS_GetImageFormat(python_get_eios(self));
    return to_python_object(format);
}

PyObject* Python_EIOS_SetImageFormat(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    EIOS_SetImageFormat(python_get_eios(self), from_python_object<ImageFormat>(args[0]));
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_GetImageBuffer(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 0)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 0, args_length);
        return nullptr;
    }
    #endif

    std::int32_t width = 0;
    std::int32_t height = 0;
    EIOS_GetTargetDimensions(python_get_eios(self), &width, &height);
    std::uint8_t* buffer = EIOS_GetImageBuffer(python_get_eios(self));
    return python->PyMemoryView_FromMemory(reinterpret_cast<char*>(buffer), width * height * 4, 0x200);
}

PyObject* Python_EIOS_GetDebugImageBuffer(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 0)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 0, args_length);
        return nullptr;
    }
    #endif

    std::int32_t width = 0;
    std::int32_t height = 0;
    EIOS_GetTargetDimensions(python_get_eios(self), &width, &height);
    std::uint8_t* buffer = EIOS_GetDebugImageBuffer(python_get_eios(self));
    return python->PyMemoryView_FromMemory(reinterpret_cast<char*>(buffer), width * height * 4, 0x200);
}

PyObject* Python_EIOS_SetGraphicsDebugging(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'enabled: Bool'", error.c_str());
        return nullptr;
    }
    #endif

    bool enabled = from_python_object<bool>(args[0]);
    EIOS_SetGraphicsDebugging(python_get_eios(self), enabled);
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_HasFocus(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 0)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 0, args_length);
        return nullptr;
    }
    #endif

    return to_python_object(EIOS_HasFocus(python_get_eios(self)));
}

PyObject* Python_EIOS_GainFocus(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 0)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 0, args_length);
        return nullptr;
    }
    #endif

    EIOS_GainFocus(python_get_eios(self));
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_LoseFocus(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 0)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 0, args_length);
        return nullptr;
    }
    #endif

    EIOS_LoseFocus(python_get_eios(self));
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_IsKeyboardInputEnabled(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 0)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 0, args_length);
        return nullptr;
    }
    #endif

    return to_python_object(EIOS_IsKeyboardInputEnabled(python_get_eios(self)));
}

PyObject* Python_EIOS_SetKeyboardInputEnabled(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'enabled: Bool'", error.c_str());
        return nullptr;
    }
    #endif

    bool enabled = from_python_object<bool>(args[0]);
    EIOS_SetKeyboardInputEnabled(python_get_eios(self), enabled);
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_IsMouseInputEnabled(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 0)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 0, args_length);
        return nullptr;
    }
    #endif

    return to_python_object(EIOS_IsMouseInputEnabled(python_get_eios(self)));
}

PyObject* Python_EIOS_SetMouseInputEnabled(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'enabled: Bool'", error.c_str());
        return nullptr;
    }
    #endif

    bool enabled = from_python_object<bool>(args[0]);
    EIOS_SetMouseInputEnabled(python_get_eios(self), enabled);
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_GetMousePosition(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 0)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 0, args_length);
        return nullptr;
    }
    #endif

    std::int32_t width = 0;
    std::int32_t height = 0;
    EIOS_GetMousePosition(python_get_eios(self), &width, &height);
    return python->PyTuple_Pack(2, python->PyLong_FromLong(width), python->PyLong_FromLong(height));
}

PyObject* Python_EIOS_GetRealMousePosition(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 0)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 0, args_length);
        return nullptr;
    }
    #endif

    std::int32_t width = 0;
    std::int32_t height = 0;
    EIOS_GetRealMousePosition(python_get_eios(self), &width, &height);
    return python->PyTuple_Pack(2, python->PyLong_FromLong(width), python->PyLong_FromLong(height));
}

PyObject* Python_EIOS_MoveMouse(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 2)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 2, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'x: Int32'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<bool>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'y: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    EIOS_MoveMouse(python_get_eios(self), from_python_object<std::int32_t>(args[0]), from_python_object<std::int32_t>(args[1]));
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_HoldMouse(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;

    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'button: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    EIOS_HoldMouse(python_get_eios(self), 0, 0, from_python_object<std::int32_t>(args[0]));
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_ReleaseMouse(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;

    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'button: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    EIOS_ReleaseMouse(python_get_eios(self), 0, 0, from_python_object<std::int32_t>(args[0]));
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_ScrollMouse(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'lines: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    EIOS_ScrollMouse(python_get_eios(self), 0, 0, from_python_object<std::int32_t>(args[0]));
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_IsMouseButtonHeld(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'button: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    return to_python_object(EIOS_IsMouseButtonHeld(python_get_eios(self), from_python_object<std::int32_t>(args[0])));
}

PyObject* Python_EIOS_SendString(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 3)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 3, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'string: String'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<bool>(args[1], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'keymod: Int32'", error.c_str());
        return nullptr;
    }

    if (!type_python_is_valid<bool>(args[2], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'keymodwait: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    std::string text = from_python_object<std::string>(args[0]);
    EIOS_SendString(python_get_eios(self), text.c_str(), from_python_object<std::int32_t>(args[1]), from_python_object<std::int32_t>(args[2]));
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_HoldKey(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'key: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    EIOS_HoldKey(python_get_eios(self), from_python_object<std::int32_t>(args[0]));
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_ReleaseKey(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'key: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    EIOS_ReleaseKey(python_get_eios(self), from_python_object<std::int32_t>(args[0]));
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_IsKeyHeld(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'key: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    return to_python_object(EIOS_IsKeyHeld(python_get_eios(self), from_python_object<std::int32_t>(args[0])));
}

PyObject* Python_EIOS_GetKeyboardSpeed(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 0)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 0, args_length);
        return nullptr;
    }
    #endif

    return to_python_object(EIOS_GetKeyboardSpeed(python_get_eios(self)));
}

PyObject* Python_EIOS_SetKeyboardSpeed(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'speed: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    EIOS_SetKeyboardSpeed(python_get_eios(self), from_python_object<std::int32_t>(args[0]));
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_GetKeyboardRepeatDelay(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 0)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 0, args_length);
        return nullptr;
    }
    #endif

    return to_python_object(EIOS_GetKeyboardRepeatDelay(python_get_eios(self)));
}

PyObject* Python_EIOS_SetKeyboardRepeatDelay(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'delay: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    EIOS_SetKeyboardRepeatDelay(python_get_eios(self), from_python_object<std::int32_t>(args[0]));
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_GetClientsPIDs(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'unpaired_only: Bool'", error.c_str());
        return nullptr;
    }
    #endif

    std::size_t client_count = EIOS_GetClients(from_python_object<bool>(args[0]));
    std::vector<std::int32_t> client_pids(client_count);

    for (std::size_t i = 0; i < client_count; ++i)
    {
        client_pids[i] = EIOS_GetClientPID(i);
    }
    return to_python_array(client_pids);
}

PyObject* Python_EIOS_PairClient_PID(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'process_id: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    return PythonWrapEIOS(EIOS_PairClient(from_python_object<std::int32_t>(args[0])));
}

PyObject* Python_EIOS_KillClientPID(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 1)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu argument (%zus given)", 1, args_length);
        return nullptr;
    }

    std::string error;
    if (!type_python_is_valid<bool>(args[0], error))
    {
        PyErr_Format(PyExc_TypeError, "Invalid Argument: %s. Expected 'process_id: Int32'", error.c_str());
        return nullptr;
    }
    #endif

    EIOS_KillClientPID(from_python_object<std::int32_t>(args[0]));
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_EIOS_KillClient(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    #ifdef PYTHON_API_VALIDATION
    if (args_length != 0)
    {
        PyErr_Format(PyExc_TypeError, "function takes exactly %zu arguments (%zus given)", 0, args_length);
        return nullptr;
    }
    #endif

    EIOS_KillClient(python_get_eios(self));
    
    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}

PyObject* Python_Reflect_Release_Objects(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept
{
    extern int PyJavaObject_Clear(PyObject* object);
    extern int PyJavaArray_Clear(PyObject* object);

    // Flatten the List
    std::stack<PyObject*> stack;
    std::vector<PyObject*> objects;
    stack.push(args[0]);

    while (!stack.empty()) {
        PyObject* object = stack.top();
        stack.pop();

        if ((python->PyList_Check)(object))
        {
            for (std::size_t i = 0; i < python->PyList_Size(object); ++i)
            {
                stack.push(python->PyList_GetItem(object, i));
            }
        }
        else
        {
            objects.push_back(object);
        }
    }

    // Early exit if no objects to free
    if (objects.empty())
    {
        (python->Py_INCREF)(python->Py_GetNone_Object());
        return python->Py_GetNone_Object();
    }

    // Unwrap each object and clear as we go along to prevent double-free
    std::vector<jobject> result;
    result.reserve(objects.size());

    for (PyObject* object : objects)
    {
        PyRemoteInputType type = GetObjectType(object);
        if (type == PyRemoteInputType::JAVA_OBJECT)
        {
            result.push_back(from_python_object<jobject>(object));
            PyJavaObject_Clear(object);
        }
        else if (type == PyRemoteInputType::JAVA_ARRAY)
        {
            result.push_back(from_python_object<jarray>(object));
            PyJavaArray_Clear(object);
        }
    }

    // Release all objects at once
    Reflect_Release_Objects(python_get_eios(self), &result[0], result.size());

    // Clear the List
    python->PySequence_DelSlice(args[0], 0, python->PySequence_Length(args[0]));

    (python->Py_INCREF)(python->Py_GetNone_Object());
    return python->Py_GetNone_Object();
}