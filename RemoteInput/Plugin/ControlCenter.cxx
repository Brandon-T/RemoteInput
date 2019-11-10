#include "ControlCenter.hxx"
#include <thread>
#include "EIOS.hxx"
#include "Platform.hxx"
#include "NativeHooks.hxx"
#include "Graphics.hxx"

template<typename T>
T EIOS_Read(void* &ptr)
{
	T result = *static_cast<T*>(ptr);
	ptr = static_cast<T*>(ptr) + 1;
	return result;
}

template<typename T>
void EIOS_Write(void* &ptr, T result)
{
	*static_cast<T*>(ptr) = result;
	ptr = static_cast<T*>(ptr) + 1;
}

ControlCenter::ControlCenter(pid_t pid, bool is_controller, std::unique_ptr<Reflection> &&reflector) : pid(pid),  is_controller(is_controller), stopped(is_controller), map_lock(), command_signal(), response_signal(), reflector(std::move(reflector))
{
	if (pid <= 0)
	{
		throw std::runtime_error("Invalid Process ID");
	}
	
	if (!init_maps())
	{
		throw std::runtime_error("Cannot Initialize Maps");
	}
	
	if (!init_locks())
	{
		throw std::runtime_error("Cannot Initialize Locks");
	}
	
	if (!init_signals())
	{
		throw std::runtime_error("Cannot Initialize Signals");
	}
	
	if (!is_controller)
	{
		std::thread thread = std::thread([&]{
			while(!stopped) {
				if (!command_signal || !map_lock || !response_signal || !memory_map)
				{
					return;
				}

				command_signal->wait();
				if (stopped)
				{
					return;
				}

				map_lock->lock();
				process_command();
				map_lock->unlock();
				response_signal->signal();
			}
		});
		
		thread.detach();
	}
}

ControlCenter::~ControlCenter()
{
	terminate();
	
	reflector.reset();

	if (map_lock)
	{
		map_lock->unlock();
	}
	map_lock.reset();

	if (memory_map)
	{
		memory_map->unmap();
		memory_map->close();
	}
}

void ControlCenter::terminate()
{
	if (!stopped)
	{
		stopped = true;
		
		if (command_signal)
		{
			command_signal->signal();
		}
	}
}

void ControlCenter::process_command()
{
	ImageData* info = reinterpret_cast<ImageData*>(memory_map->data());
	EIOSCommand command = info->command;
	void* arguments = info->args;

	switch(command)
	{
		case EIOSCommand::COMMAND_NONE:
			break;
			
		case EIOSCommand::GET_TARGET_DIMENSIONS:
		{
			EIOS_Write<std::int32_t>(arguments, 0);
			EIOS_Write<std::int32_t>(arguments, 0);
		}
			break;
			
		case EIOSCommand::GET_MOUSE:
		{
			EIOS_Write<std::int32_t>(arguments, 0);
			EIOS_Write<std::int32_t>(arguments, 0);
		}
			break;
			
		case EIOSCommand::MOVE_MOUSE:
		{
			std::int32_t x = EIOS_Read<std::int32_t>(arguments);
			std::int32_t y = EIOS_Read<std::int32_t>(arguments);
		}
			break;
			
		case EIOSCommand::HOLD_MOUSE:
		{
			std::int32_t x = EIOS_Read<std::int32_t>(arguments);
			std::int32_t y = EIOS_Read<std::int32_t>(arguments);
			std::int32_t button = EIOS_Read<std::int32_t>(arguments);
		}
			break;
			
		case EIOSCommand::RELEASE_MOUSE:
		{
			std::int32_t x = EIOS_Read<std::int32_t>(arguments);
			std::int32_t y = EIOS_Read<std::int32_t>(arguments);
			std::int32_t button = EIOS_Read<std::int32_t>(arguments);
		}
			break;
			
		case EIOSCommand::IS_MOUSE_HELD:
			break;
			
		case EIOSCommand::SEND_STRING:
			break;
			
		case EIOSCommand::HOLD_KEY:
			break;
			
		case EIOSCommand::RELEASE_KEY:
			break;
			
		case EIOSCommand::IS_KEY_HELD:
			break;
			
		case EIOSCommand::REFLECT:
			break;
	}
}

bool ControlCenter::init_maps()
{
    char mapName[256] = {0};
    sprintf(mapName, "Local\\RemoteInput_%d", pid);

    if (!is_controller)
	{
		//Open only..
		memory_map = std::make_unique<MemoryMap<char>>(mapName, std::ios::in | std::ios::out);
		return memory_map && memory_map->open() && memory_map->map();
	}

	//Create only..
    int width = 0;
    int height = 0;
    GetDesktopResolution(width, height); //TODO: Change to Target Window size..

    int image_size = width * height * 4 * sizeof(std::uint8_t);
	std::int32_t map_size = (1024 * sizeof(std::int32_t)) + image_size;
	memory_map = std::make_unique<MemoryMap<char>>(mapName, map_size, std::ios::in | std::ios::out);
	return memory_map && memory_map->open() && memory_map->map();
}


bool ControlCenter::init_locks()
{
    char lockName[256] = {0};
    sprintf(lockName, "Local\\RemoteInput_Lock_%d", pid);
	map_lock = std::make_unique<Mutex>(lockName);
    return map_lock != nullptr;
}

bool ControlCenter::init_signals()
{
    char signalName[256] = {0};
    sprintf(signalName, "Local\\RemoteInput_ControlCenter_EventRead_%d", pid);
	command_signal = std::make_unique<Semaphore>(signalName);

    sprintf(signalName, "Local\\RemoteInput_ControlCenter_EventWrite_%d", pid);
    response_signal = std::make_unique<Semaphore>(signalName);
    return command_signal && response_signal;
}

bool ControlCenter::hasReflector()
{
	return reflector != nullptr;
}

ImageData* ControlCenter::get_image_data() const
{
	return static_cast<ImageData*>(memory_map->data());
}

std::int32_t ControlCenter::parent_id() const
{
	return get_image_data()->parentId;
}

std::int32_t ControlCenter::get_width() const
{
	return get_image_data()->width;
}

std::int32_t ControlCenter::get_height() const
{
	return get_image_data()->height;
}

std::uint8_t* ControlCenter::get_image() const
{
	ImageData* image_data = get_image_data();
	return reinterpret_cast<std::uint8_t*>(image_data) + sizeof(ImageData);
}

void ControlCenter::set_parent(pid_t pid)
{
	get_image_data()->parentId = pid;
}

void ControlCenter::get_target_dimensions(std::int32_t* width, std::int32_t* height)
{
	ImageData* image_data = get_image_data();
	
	map_lock->lock();
	image_data->command = EIOSCommand::GET_TARGET_DIMENSIONS;
	map_lock->unlock();
	command_signal->signal();
	if (response_signal->timed_wait(10))
	{
		map_lock->lock();
		void* arguments = image_data->args;
		*width = EIOS_Read<std::int32_t>(arguments);
		*height = EIOS_Read<std::int32_t>(arguments);
		map_lock->unlock();
	}
}

void ControlCenter::get_mouse_position(std::int32_t* x, std::int32_t* y)
{
	ImageData* image_data = get_image_data();
	
	map_lock->lock();
	image_data->command = EIOSCommand::GET_MOUSE;
	map_lock->unlock();
	command_signal->signal();
	if (response_signal->timed_wait(10))
	{
		map_lock->lock();
		void* arguments = image_data->args;
		*x = EIOS_Read<std::int32_t>(arguments);
		*y = EIOS_Read<std::int32_t>(arguments);
		map_lock->unlock();
	}
}

void ControlCenter::move_mouse(std::int32_t x, std::int32_t y)
{
	ImageData* image_data = get_image_data();
	void* arguments = image_data->args;
	
	map_lock->lock();
	image_data->command = EIOSCommand::MOVE_MOUSE;
	EIOS_Write<std::int32_t>(arguments, x);
	EIOS_Write<std::int32_t>(arguments, y);
	map_lock->unlock();
	command_signal->signal();
	response_signal->timed_wait(10);
}

void ControlCenter::hold_mouse(std::int32_t x, std::int32_t y, std::int32_t button)
{
	ImageData* image_data = get_image_data();
	void* arguments = image_data->args;
	
	map_lock->lock();
	image_data->command = EIOSCommand::HOLD_MOUSE;
	EIOS_Write<std::int32_t>(arguments, x);
	EIOS_Write<std::int32_t>(arguments, y);
	EIOS_Write<std::int32_t>(arguments, button);
	map_lock->unlock();
	command_signal->signal();
	response_signal->timed_wait(10);
}

void ControlCenter::release_mouse(std::int32_t x, std::int32_t y, std::int32_t button)
{
	ImageData* image_data = get_image_data();
	void* arguments = image_data->args;
	
	map_lock->lock();
	image_data->command = EIOSCommand::RELEASE_MOUSE;
	EIOS_Write<std::int32_t>(arguments, x);
	EIOS_Write<std::int32_t>(arguments, y);
	EIOS_Write<std::int32_t>(arguments, button);
	map_lock->unlock();
	command_signal->signal();
	response_signal->timed_wait(10);
}
