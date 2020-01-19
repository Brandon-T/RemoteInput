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

EIOS* EIOS_RequestTarget(const char* initargs)
{
    std::size_t pos = 0;
    std::int32_t pid = std::stoi(initargs, &pos);
    if (pos == std::string(initargs).length())
    {
		return EIOS_PairClient(pid);
    }
	return nullptr;
}

void EIOS_ReleaseTarget(EIOS* eios)
{
    if (eios)
    {
        clients.erase(eios->pid);
		eios->control_center->set_parent_process_id(-1);
		eios->control_center->set_parent_thread_id(-1);
		eios->control_center->terminate();
		eios->control_center.reset();
		delete eios;
    }
}

void EIOS_GetTargetDimensions(EIOS* eios, std::int32_t* width, std::int32_t* height)
{
    if (eios)
    {
		eios->control_center->get_target_dimensions(width, height);
    }
}

std::uint8_t* EIOS_GetImageBuffer(EIOS* eios)
{
    if (eios)
    {
		return eios->control_center->get_image();
    }
    return nullptr;
}

std::uint8_t* EIOS_GetDebugImageBuffer(EIOS* eios)
{
    if (eios)
    {
		return eios->control_center->get_debug_image();
    }
    return nullptr;
}

void EIOS_SetGraphicsDebugging(EIOS* eios, bool enabled)
{
	if (eios)
	{
		eios->control_center->set_debug_graphics(enabled);
	}
}

void EIOS_UpdateImageBuffer(EIOS* eios)
{
    //printf("%s\n", __FUNCTION__);
}

void EIOS_GetMousePosition(EIOS* eios, std::int32_t* x, std::int32_t* y)
{

	if (eios)
	{
		eios->control_center->get_mouse_position(x, y);
	}
}

void EIOS_MoveMouse(EIOS* eios, std::int32_t x, std::int32_t y)
{
	if (eios)
	{
		eios->control_center->move_mouse(x, y);
	}
}

void EIOS_HoldMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button)
{
	if (eios)
	{
		eios->control_center->hold_mouse(x, y, button);
	}
}

void EIOS_ReleaseMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button)
{
	if (eios)
	{
		eios->control_center->release_mouse(x, y, button);
	}
}

void EIOS_ScrollMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t lines)
{
	if (eios)
	{
		eios->control_center->scroll_mouse(x, y, lines);
	}
}

bool EIOS_IsMouseHeld(EIOS* eios, std::int32_t button)
{
	if (eios)
	{
		return eios->control_center->is_mouse_held(button);
	}
	return false;
}

void EIOS_SendString(EIOS* eios, const char* string, std::int32_t keywait, std::int32_t keymodwait)
{
	if (eios)
	{
		eios->control_center->send_string(string, keywait, keymodwait);
	}
}

void EIOS_HoldKey(EIOS* eios, std::int32_t key)
{
	if (eios)
	{
		eios->control_center->hold_key(key);
	}
}

void EIOS_ReleaseKey(EIOS* eios, std::int32_t key)
{
	if (eios)
	{
		eios->control_center->release_key(key);
	}
}

bool EIOS_IsKeyHeld(EIOS* eios, std::int32_t key)
{
	if (eios)
	{
		return eios->control_center->is_key_held(key);
	}
	return false;
}

EIOS* EIOS_PairClient(pid_t pid)
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

void EIOS_KillClientPID(pid_t pid)
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

void EIOS_KillClient(EIOS* eios)
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

void EIOS_KillZombieClients()
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

std::size_t EIOS_GetClients(bool unpaired_only)
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

pid_t EIOS_GetClientPID(std::size_t index)
{
	return index < all_clients.size() && all_clients.size() > 0 ? all_clients[index] : -1;
}
