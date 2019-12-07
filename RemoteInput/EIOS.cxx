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

std::unordered_map<std::int32_t, EIOS*> clients;

EIOS* EIOS_RequestTarget(const char* initargs)
{
    printf("%s\n", __FUNCTION__);

    std::size_t pos = 0;
    std::int32_t pid = std::stoi(initargs, &pos);
    if (pos == std::string(initargs).length())
    {
        if (clients.count(pid))
        {
            return clients[pid];
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
                eios->control_center->set_parent(getpid());

                clients[pid] = eios;
                return eios;
            }
        }
        catch(std::exception& e)
        {
            printf("%s\n", e.what());
        }
    }
	return nullptr;
}

void EIOS_ReleaseTarget(EIOS* eios)
{
    printf("%s\n", __FUNCTION__);
    if (eios)
    {
        clients.erase(eios->pid);
		eios->control_center->set_parent(-1);
		eios->control_center->terminate();
		eios->control_center.reset();
		delete eios;
    }
}

void EIOS_GetTargetDimensions(EIOS* eios, std::int32_t* width, std::int32_t* height)
{
    printf("%s\n", __FUNCTION__);
    if (eios)
    {
		eios->control_center->get_target_dimensions(width, height);
    }
}

std::uint8_t* EIOS_GetImageBuffer(EIOS* eios)
{
    printf("%s\n", __FUNCTION__);
    if (eios)
    {
		return eios->control_center->get_image();
    }
    return nullptr;
}

void EIOS_UpdateImageBuffer(EIOS* eios)
{
    printf("%s\n", __FUNCTION__);
}

void EIOS_GetMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y)
{
    printf("%s\n", __FUNCTION__);

	if (eios)
	{
		eios->control_center->get_mouse_position(x, y);
	}
}

void EIOS_MoveMouse(EIOS* eios, std::int32_t x, std::int32_t y)
{
    printf("%s\n", __FUNCTION__);

	if (eios)
	{
		eios->control_center->move_mouse(x, y);
	}
}

void EIOS_HoldMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button)
{
    printf("%s\n", __FUNCTION__);

	if (eios)
	{
		eios->control_center->hold_mouse(x, y, button);
	}
}

void EIOS_ReleaseMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button)
{
    printf("%s\n", __FUNCTION__);

	if (eios)
	{
		eios->control_center->release_mouse(x, y, button);
	}
}

bool EIOS_IsMouseHeld(EIOS* eios, std::int32_t button)
{
    printf("%s\n", __FUNCTION__);
	return false;
}

void EIOS_SendString(EIOS* eios, const char* string, std::int32_t keywait, std::int32_t keymodwait)
{
    printf("%s\n", __FUNCTION__);
}

void EIOS_HoldKey(EIOS* eios, std::int32_t key)
{
    printf("%s\n", __FUNCTION__);
}

void EIOS_ReleaseKey(EIOS* eios, std::int32_t key)
{
    printf("%s\n", __FUNCTION__);
}

bool EIOS_IsKeyHeld(EIOS* eios, std::int32_t key)
{
    printf("%s\n", __FUNCTION__);
	return false;
}
