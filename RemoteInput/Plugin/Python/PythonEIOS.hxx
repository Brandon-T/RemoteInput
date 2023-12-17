//
// Created by Brandon on 2022-09-21.
//

#ifndef REMOTEINPUT_PYTHONEIOS_HXX
#define REMOTEINPUT_PYTHONEIOS_HXX

#include "PythonCommon.hxx"
#include "EIOS.hxx"

#ifdef __cplusplus
extern "C"
{
#endif

PyObject* Python_EIOS_Inject(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_Inject_PID(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_From_PID(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_RequestTarget(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
//PyObject* Python_EIOS_ReleaseTarget(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_GetTargetDimensions(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_GetImageFormat(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_SetImageFormat(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_GetImageBuffer(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_GetDebugImageBuffer(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_SetGraphicsDebugging(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_HasFocus(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_GainFocus(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_LoseFocus(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_IsKeyboardInputEnabled(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_SetKeyboardInputEnabled(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_IsMouseInputEnabled(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_SetMouseInputEnabled(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_GetMousePosition(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_GetRealMousePosition(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_MoveMouse(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_HoldMouse(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_ReleaseMouse(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_ScrollMouse(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_IsMouseButtonHeld(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_SendString(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_HoldKey(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_ReleaseKey(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_IsKeyHeld(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_GetKeyboardSpeed(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_SetKeyboardSpeed(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_GetKeyboardRepeatDelay(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_SetKeyboardRepeatDelay(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_GetClientsPIDs(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_PairClient_PID(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_KillClientPID(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;
PyObject* Python_EIOS_KillClient(PyEIOS* self, PyObject* args[], Py_ssize_t args_length) noexcept;

#ifdef __cplusplus
}
#endif

#endif //REMOTEINPUT_PYTHONEIOS_HXX
