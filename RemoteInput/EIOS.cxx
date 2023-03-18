//
//  EIOS.cpp
//  RemoteInput
//
//  Created by Brandon on 2019-09-14.
//  Copyright © 2019 XIO. All rights reserved.
//

#include "EIOS.hxx"
#include <cstdio>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <cstring>
#include "MemoryMap.hxx"
#include "SharedEvent.hxx"
#include "Reflection.hxx"
#include "Platform.hxx"

std::unordered_map<std::int32_t, EIOS*> clients;
std::vector<std::int32_t> all_clients;

EIOS* STD_CALL EIOS_RequestTarget(const char* initargs) noexcept
{
    std::size_t pos = 0;
    std::int32_t pid = std::stoi(initargs, &pos);
    if (pos == std::string(initargs).length())
    {
        return EIOS_PairClient(pid);
    }
    return nullptr;
}

void STD_CALL EIOS_ReleaseTarget(EIOS* eios) noexcept
{
    if (eios)
    {
        clients.erase(eios->pid);
        eios->control_center->set_parent_process_id(-1);
        eios->control_center->set_parent_thread_id(-1);
        eios->control_center->terminate();
        eios->control_center.reset();
        eios->pid = -1;
        delete eios;
    }
}

void STD_CALL EIOS_GetTargetDimensions(EIOS* eios, std::int32_t* width, std::int32_t* height) noexcept
{
    if (eios)
    {
        eios->control_center->get_target_dimensions(width, height);
    }
}

std::uint8_t* STD_CALL EIOS_GetImageBuffer(EIOS* eios) noexcept
{
    if (eios)
    {
        return eios->control_center->get_image();
    }
    return nullptr;
}

std::uint8_t* STD_CALL EIOS_GetDebugImageBuffer(EIOS* eios) noexcept
{
    if (eios)
    {
        return eios->control_center->get_debug_image();
    }
    return nullptr;
}

void STD_CALL EIOS_SetGraphicsDebugging(EIOS* eios, bool enabled) noexcept
{
    if (eios)
    {
        eios->control_center->set_debug_graphics(enabled);
    }
}

void STD_CALL EIOS_UpdateImageBuffer(EIOS* eios) noexcept
{
    //printf("%s\n", __FUNCTION__);
}

bool STD_CALL EIOS_HasFocus(EIOS* eios) noexcept
{
    if (eios)
    {
        return eios->control_center->has_focus();
    }
    return false;
}

void STD_CALL EIOS_GainFocus(EIOS* eios) noexcept
{
    if (eios)
    {
        eios->control_center->gain_focus();
    }
}

void STD_CALL EIOS_LoseFocus(EIOS* eios) noexcept
{
    if (eios)
    {
        eios->control_center->lose_focus();
    }
}

bool STD_CALL EIOS_IsKeyboardInputEnabled(EIOS* eios) noexcept
{
    if (eios)
    {
        return eios->control_center->is_keyboard_input_enabled();
    }
    return false;
}

void STD_CALL EIOS_SetKeyboardInputEnabled(EIOS* eios, bool enabled) noexcept
{
    if (eios)
    {
        eios->control_center->set_keyboard_input_enabled(enabled);
    }
}

bool STD_CALL EIOS_IsMouseInputEnabled(EIOS* eios) noexcept
{
    if (eios)
    {
        return eios->control_center->is_mouse_input_enabled();
    }
    return false;
}

void STD_CALL EIOS_SetMouseInputEnabled(EIOS* eios, bool enabled) noexcept
{
    if (eios)
    {
        eios->control_center->set_mouse_input_enabled(enabled);
    }
}

void STD_CALL EIOS_GetMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y) noexcept
{

    if (eios)
    {
        eios->control_center->get_mouse_position(x, y);
    }
}

void STD_CALL EIOS_GetRealMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y) noexcept
{

    if (eios)
    {
        eios->control_center->get_real_mouse_position(x, y);
    }
}

void STD_CALL EIOS_MoveMouse(EIOS* eios, std::int32_t x, std::int32_t y) noexcept
{
    if (eios)
    {
        eios->control_center->move_mouse(x, y);
    }
}

void STD_CALL EIOS_HoldMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button) noexcept
{
    if (eios)
    {
        eios->control_center->hold_mouse(x, y, button);
    }
}

void STD_CALL EIOS_ReleaseMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button) noexcept
{
    if (eios)
    {
        eios->control_center->release_mouse(x, y, button);
    }
}

void STD_CALL EIOS_ScrollMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t lines) noexcept
{
    if (eios)
    {
        eios->control_center->scroll_mouse(x, y, lines);
    }
}

bool STD_CALL EIOS_IsMouseButtonHeld(EIOS* eios, std::int32_t button) noexcept
{
    if (eios)
    {
        return eios->control_center->is_mouse_held(button);
    }
    return false;
}

void STD_CALL EIOS_SendString(EIOS* eios, const char* string, std::int32_t keywait, std::int32_t keymodwait) noexcept
{
    if (eios)
    {
        eios->control_center->send_string(string, keywait, keymodwait);
    }
}

void STD_CALL EIOS_HoldKey(EIOS* eios, std::int32_t key) noexcept
{
    if (eios)
    {
        eios->control_center->hold_key(key);
    }
}

void STD_CALL EIOS_ReleaseKey(EIOS* eios, std::int32_t key) noexcept
{
    if (eios)
    {
        eios->control_center->release_key(key);
    }
}

bool STD_CALL EIOS_IsKeyHeld(EIOS* eios, std::int32_t key) noexcept
{
    if (eios)
    {
        return eios->control_center->is_key_held(key);
    }
    return false;
}

std::int32_t STD_CALL EIOS_GetKeyboardSpeed(EIOS* eios) noexcept
{
    if (eios)
    {
        return eios->control_center->get_keyboard_speed();
    }
    return -1;
}

void STD_CALL EIOS_SetKeyboardSpeed(EIOS* eios, std::int32_t speed) noexcept
{
    if (eios)
    {
        eios->control_center->set_keyboard_speed(speed);
    }
}

std::int32_t STD_CALL EIOS_GetKeyboardRepeatDelay(EIOS* eios) noexcept
{
    if (eios)
    {
        return eios->control_center->get_keyboard_repeat_delay();
    }
    return -1;
}

void STD_CALL EIOS_SetKeyboardRepeatDelay(EIOS* eios, std::int32_t delay) noexcept
{
    if (eios)
    {
        eios->control_center->set_keyboard_repeat_delay(delay);
    }
}

bool STD_CALL EIOS_GetGraphicsScaling(EIOS* eios) noexcept
{
    if (eios)
    {
        return eios->control_center->get_graphics_scaling();
    }
    return false;
}

void STD_CALL EIOS_SetGraphicsScaling(EIOS* eios, bool enabled) noexcept
{
    if (eios)
    {
        eios->control_center->set_graphics_scaling(enabled);
    }
}

EIOS* STD_CALL EIOS_PairClient(std::int32_t pid) noexcept
{
    std::int32_t tid = GetCurrentThreadID();

    if (clients.count(pid))
    {
        EIOS* eios = clients[pid];
        return eios->control_center->parent_thread_id() == tid ? eios : nullptr;
    }

    if (!IsProcessAlive(pid))
    {
        clients.erase(pid);
        ControlCenter::kill_zombie_process(pid);
        return nullptr;
    }

    try {
        std::unique_ptr<ControlCenter> control_center = std::make_unique<ControlCenter>(pid, true, nullptr);
        if (control_center)
        {
            EIOS* eios = new EIOS();
            eios->pid = pid;
            eios->width = control_center->get_width();
            eios->height = control_center->get_height();
            eios->control_center = std::move(control_center);
            eios->control_center->set_parent_process_id(getpid());
            eios->control_center->set_parent_thread_id(tid);

            clients[pid] = eios;
            return eios;
        }
    }
    catch(std::exception& e)
    {
        printf("%s\n", e.what());
    }
    return nullptr;
}

void STD_CALL EIOS_KillClientPID(std::int32_t pid) noexcept
{
    if (pid != -1 && clients.count(pid) && IsProcessAlive(pid))
    {
        if (EIOS* eios = clients[pid]; eios->control_center->parent_thread_id() == GetCurrentThreadID())
        {
            clients.erase(pid);
            eios->control_center->kill_process(pid);
        }
    }
}

void STD_CALL EIOS_KillClient(EIOS* eios) noexcept
{
    if (eios && eios->pid != -1 && clients.count(eios->pid) && IsProcessAlive(eios->pid))
    {
        if (eios->control_center->parent_thread_id() == GetCurrentThreadID())
        {
            clients.erase(eios->pid);
            eios->control_center->kill_process(eios->pid);
        }
    }
}

void STD_CALL EIOS_KillZombieClients() noexcept
{
    std::vector<std::int32_t> dead_clients;
    for(auto it = clients.begin(); it != clients.end(); ++it)
    {
        if (!IsProcessAlive(it->first))
        {
            ControlCenter::kill_zombie_process(it->first);
            dead_clients.push_back(it->first);
        }
    }

    for (auto pid : dead_clients)
    {
        clients.erase(pid);
    }
}

std::size_t STD_CALL EIOS_GetClients(bool unpaired_only) noexcept
{
    all_clients.clear();

    auto pids = get_pids();
    for (auto pid : pids)
    {
        if (unpaired_only)
        {
            bool exists = false;
            if (!ControlCenter::controller_is_paired(pid, &exists))
            {
                if (exists)
                {
                    all_clients.push_back(pid);
                }
            }
        }
        else
        {
            if (ControlCenter::controller_exists(pid))
            {
                all_clients.push_back(pid);
            }
        }
    }

    return all_clients.size();
}

std::int32_t STD_CALL EIOS_GetClientPID(std::size_t index) noexcept
{
    return index < all_clients.size() && all_clients.size() > 0 ? all_clients[index] : -1;
}
