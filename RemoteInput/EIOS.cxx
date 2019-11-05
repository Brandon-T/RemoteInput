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
#include "Echo/MemoryMap.hxx"
#include "Echo/SharedEvent.hxx"
#include "Reflection.hxx"

std::unordered_map<std::int32_t, EIOS*> clients;
std::unique_ptr<MemoryMap<char>> globalMap;
std::unique_ptr<Mutex> globalLock;
std::unique_ptr<Semaphore> commandSignal;
std::unique_ptr<Semaphore> responseSignal;
std::unique_ptr<Reflection> globalReflector;

#include "Plugin/CommandThread.hxx"
extern ResponsiveThread* thread;

template<typename T>
T& EIOS_Cast(void* ptr, std::intptr_t offset)
{
    return *reinterpret_cast<T*>(static_cast<std::uint8_t*>(ptr) + offset);
}

template<typename T>
void EIOS_Write(void* &ptr, T value)
{
    *static_cast<T*>(ptr) = value;
    ptr = static_cast<T*>(ptr) + 1;
}

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

        globalMap.reset();
        globalLock.reset();
        commandSignal.reset();
        responseSignal.reset();

        char mapName[256] = {0};
        sprintf(mapName, "Local\\RemoteInput_%d", pid);
        MemoryMap<char>* memory = new MemoryMap<char>{mapName, std::ios::in | std::ios::out};
        if (memory->open() && memory->map())
        {
            char buffer[256] = {0};
            sprintf(buffer, "Local\\RemoteInput_Lock_%d", pid);
            globalLock.reset(new Mutex(buffer));

            sprintf(buffer, "Local\\RemoteInput_EventRead_%d", pid);
            commandSignal.reset(new Semaphore(buffer));

            sprintf(buffer, "Local\\RemoteInput_EventWrite_%d", pid);
            responseSignal.reset(new Semaphore(buffer));

            EIOS* eios = new EIOS();
            eios->pid = pid;
            eios->memoryMap = memory;
            eios->imageData = reinterpret_cast<ImageData*>(memory->data());
            eios->imageData->imgoff = sizeof(ImageData);

            globalLock->lock();
            eios->imageData->parentId = getpid();
            globalLock->unlock();
            clients[pid] = eios;
            return eios;
        }
        delete memory;
    }
	return nullptr;
}

void EIOS_ReleaseTarget(EIOS* eios)
{
    printf("%s\n", __FUNCTION__);
    if (eios)
    {
        clients.erase(eios->pid);

        /*globalLock->lock();
        eios->imageData->parentId = -1;
        eios->memoryMap->unmap();
        eios->memoryMap->close();
        delete eios->memoryMap;
        delete eios;

        globalLock->unlock();
        globalLock.reset();*/
    }
}

void EIOS_GetTargetDimensions(EIOS* eios, std::int32_t* width, std::int32_t* height)
{
    printf("%s\n", __FUNCTION__);
    if (eios)
    {
        *width = eios->imageData->width;
        *height = eios->imageData->height;
    }
}

std::uint8_t* EIOS_GetImageBuffer(EIOS* eios)
{
    printf("%s\n", __FUNCTION__);
    if (eios)
    {
        return reinterpret_cast<std::uint8_t*>(eios->imageData) + eios->imageData->imgoff;
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
    *x = 0;
    *y = 0;
}

void EIOS_MoveMouse(EIOS* eios, std::int32_t x, std::int32_t y)
{
    printf("%s\n", __FUNCTION__);

    globalLock->lock();
    void* args = eios->imageData->args;
    eios->imageData->command = EIOS_MOVE_MOUSE;
    EIOS_Write(args, x);
    EIOS_Write(args, y);
    globalLock->unlock();

    commandSignal->signal();
    responseSignal->wait();
}

void EIOS_HoldMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button)
{
    printf("%s\n", __FUNCTION__);
}

void EIOS_ReleaseMouse(EIOS* eios, std::int32_t x, std::int32_t y, std::int32_t button)
{
    printf("%s\n", __FUNCTION__);
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
