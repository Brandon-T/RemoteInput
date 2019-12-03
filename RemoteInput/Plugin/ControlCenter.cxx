#include "ControlCenter.hxx"
#include <thread>
#include "EIOS.hxx"
#include "Platform.hxx"
#include "NativeHooks.hxx"
#include "Graphics.hxx"
#include "ReflectionHook.hxx"

template<typename T>
T EIOS_Read(void* &ptr)
{
	T result = *static_cast<T*>(ptr);
	ptr = static_cast<T*>(ptr) + 1;
	return result;
}

std::string EIOS_Read(void* &ptr)
{
	std::size_t length = *static_cast<std::size_t*>(ptr);
	ptr = static_cast<std::size_t*>(ptr) + 1;
	
	std::string result = std::string(reinterpret_cast<const char*>(ptr), length);
	ptr = static_cast<char*>(ptr) + (result.length() * sizeof(char));
	ptr = static_cast<char*>(ptr) + 1;
	return result;
}

template<typename T>
void EIOS_Write(void* &ptr, T result)
{
	*static_cast<T*>(ptr) = result;
	ptr = static_cast<T*>(ptr) + 1;
}

void EIOS_Write(void* &ptr, const std::string &result)
{
	*static_cast<std::size_t*>(ptr) = result.length();
	ptr = static_cast<std::size_t*>(ptr) + 1;
	
	memcpy(ptr, &result[0], result.length() * sizeof(char));
	ptr = static_cast<char*>(ptr) + (result.length() * sizeof(char));
	
	*static_cast<char*>(ptr) = '\0';
	ptr = static_cast<char*>(ptr) + 1;
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
				memory_map->flush();
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
		map_lock.reset();
	}

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
	void* response = info->args;

	switch(command)
	{
		case EIOSCommand::COMMAND_NONE:
			break;

		case EIOSCommand::GET_TARGET_DIMENSIONS:
		{
//			EIOS_Write<std::int32_t>(arguments, w);
//			EIOS_Write<std::int32_t>(arguments, h);
		}
			break;

		case EIOSCommand::GET_MOUSE:
		{
//			EIOS_Write<std::int32_t>(arguments, mouse_x);
//			EIOS_Write<std::int32_t>(arguments, mouse_y);
		}
			break;

		case EIOSCommand::MOVE_MOUSE:
		{
//			std::int32_t x = EIOS_Read<std::int32_t>(arguments);
//			std::int32_t y = EIOS_Read<std::int32_t>(arguments);
//
//			mouse_x = x;
//			mouse_y = y;
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

		case EIOSCommand::REFLECT_OBJECT:
		{
			ReflectionHook hook;
			hook.read(arguments);
			
			reflector->Attach();
			auto result = reflector->getField<jobject>(hook.object, hook);
			EIOS_Write(response, result);
			reflector->Detach();
		}
			break;
			
		case EIOSCommand::REFLECT_RELEASE_OBJECT:
		{
			jobject object = EIOS_Read<jobject>(arguments);
			if (object)
			{
				reflector->Attach();
				reflector->getEnv()->DeleteGlobalRef(object);
				reflector->Detach();
			}
		}
			break;
			
		case EIOSCommand::REFLECT_CHAR:
		{
			ReflectionHook hook;
			hook.read(arguments);
			
			auto result = reflector->getPrimitive<jchar>(hook.object, hook);
			EIOS_Write(response, result);
		}
			break;
			
		case EIOSCommand::REFLECT_BYTE:
		{
			ReflectionHook hook;
			hook.read(arguments);
			
			auto result = reflector->getPrimitive<jbyte>(hook.object, hook);
			EIOS_Write(response, result);
		}
			break;
			
		case EIOSCommand::REFLECT_BOOLEAN:
		{
			ReflectionHook hook;
			hook.read(arguments);
			
			auto result = reflector->getPrimitive<jboolean>(hook.object, hook);
			EIOS_Write(response, result);
		}
			break;
			
		case EIOSCommand::REFLECT_SHORT:
		{
			ReflectionHook hook;
			hook.read(arguments);
			
			auto result = reflector->getPrimitive<jshort>(hook.object, hook);
			EIOS_Write(response, result);
		}
			break;
			
		case EIOSCommand::REFLECT_INT:
		{
			ReflectionHook hook;
			hook.read(arguments);
			
			auto result = reflector->getPrimitive<jint>(hook.object, hook);
			EIOS_Write(response, result);
		}
			break;
			
		case EIOSCommand::REFLECT_LONG:
		{
			ReflectionHook hook;
			hook.read(arguments);
			
			auto result = reflector->getPrimitive<jlong>(hook.object, hook);
			EIOS_Write(response, result);
		}
			break;
			
		case EIOSCommand::REFLECT_FLOAT:
		{
			ReflectionHook hook;
			hook.read(arguments);
			
			auto result = reflector->getPrimitive<jfloat>(hook.object, hook);
			EIOS_Write(response, result);
		}
			break;
			
		case EIOSCommand::REFLECT_DOUBLE:
		{
			ReflectionHook hook;
			hook.read(arguments);
			
			auto result = reflector->getPrimitive<jdouble>(hook.object, hook);
			EIOS_Write(response, result);
		}
			break;
			
		case EIOSCommand::REFLECT_STRING:
		{
			ReflectionHook hook;
			hook.read(arguments);
			
			reflector->Attach();
			auto result = reflector->getField<std::string>(hook.object, hook);
			reflector->Detach();
			EIOS_Write(response, result);
		}
			break;
			
		case EIOSCommand::REFLECT_ARRAY:
		{
			ReflectionHook hook;
			hook.read(arguments);
			
			auto result = reflector->getField<jobjectArray>(hook.object, hook);
			EIOS_Write(response, result);
		}
			break;
			
		case EIOSCommand::REFLECT_ARRAY_SIZE:
		{
			jobjectArray array = EIOS_Read<jobjectArray>(arguments);
			jsize length = reflector->getEnv()->GetArrayLength(array);
			EIOS_Write(response, length);
		}
			break;
			
		case EIOSCommand::REFLECT_ARRAY_INDEX:
		{
			jobjectArray array = EIOS_Read<jobjectArray>(arguments);
			jsize index = EIOS_Read<jsize>(arguments);
			jobject object = reflector->getEnv()->NewGlobalRef(reflector->getEnv()->GetObjectArrayElement(array, index));
			EIOS_Write(response, object);
		}
			break;
	}
}

bool ControlCenter::init_maps()
{
    char mapName[256] = {0};
	#if defined(_WIN32) || defined(_WIN64)
    sprintf(mapName, "Local\\RemoteInput_%d", pid);
	#else
	sprintf(mapName, "RemoteInput_%d", pid);
	#endif

    if (is_controller)
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
    int extra_size = (1024 * sizeof(std::int32_t));
	std::int32_t map_size = sizeof(ImageData) + image_size + extra_size;
	memory_map = std::make_unique<MemoryMap<char>>(mapName, map_size, std::ios::in | std::ios::out);
	return memory_map && memory_map->open() && memory_map->map();
}


bool ControlCenter::init_locks()
{
	#if defined(_WIN32) || defined(_WIN64)
	char lockName[256] = {0};
	sprintf(lockName, "Local\\RemoteInput_Lock_%d", pid);
	#elif defined(__linux__)
	char lockName[256] = {0};
	sprintf(lockName, "/RemoteInput_Lock_%d", pid);
	#else
	char lockName[31] = {0};  //PSHMNAMELEN from <sys/posix_shm.h>
	sprintf(lockName, "/RemoteInput_Lock_%d", pid);
	#endif
	
	map_lock = std::make_unique<Mutex>(lockName);
    return map_lock != nullptr;
}

bool ControlCenter::init_signals()
{
	#if defined(_WIN32) || defined(_WIN64)
	char readName[256] = {0};
	char writeName[256] = {0};
	sprintf(readName, "Local\\RemoteInput_ControlCenter_EventRead_%d", pid);
	sprintf(writeName, "Local\\RemoteInput_ControlCenter_EventWrite_%d", pid);
	#elif defined(__linux__)
	char readName[256] = {0};
	char writeName[256] = {0};
	sprintf(readName, "/RemoteInput_ControlCenter_EventRead_%d", pid);
	sprintf(writeName, "/RemoteInput_ControlCenter_EventWrite_%d", pid);
	#else
	char readName[31] = {0};  //PSHMNAMELEN from <sys/posix_shm.h>
	char writeName[31] = {0};
	sprintf(readName, "/RI_CC_EventRead_%d", pid);
	sprintf(writeName, "/RI_CC_EventWrite_%d", pid);
	#endif

	command_signal = std::make_unique<Semaphore>(readName);
    response_signal = std::make_unique<Semaphore>(writeName);
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

bool ControlCenter::send_command(std::function<void(ImageData*)> &&writer)
{
	map_lock->lock();
	writer(get_image_data());
	memory_map->flush();
	map_lock->unlock();
	command_signal->signal();
	return response_signal->wait();
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
	auto result = send_command([](ImageData* image_data) {
		image_data->command = EIOSCommand::GET_TARGET_DIMENSIONS;
	});

	if (result)
	{
		map_lock->lock();
		void* arguments = get_image_data()->args;
		*width = EIOS_Read<std::int32_t>(arguments);
		*height = EIOS_Read<std::int32_t>(arguments);
		map_lock->unlock();
	}
}

void ControlCenter::get_mouse_position(std::int32_t* x, std::int32_t* y)
{
	auto result = send_command([](ImageData* image_data) {
		image_data->command = EIOSCommand::GET_MOUSE;
	});
	
	if (result)
	{
		map_lock->lock();
		void* arguments = get_image_data()->args;
		*x = EIOS_Read<std::int32_t>(arguments);
		*y = EIOS_Read<std::int32_t>(arguments);
		map_lock->unlock();
	}
}

void ControlCenter::move_mouse(std::int32_t x, std::int32_t y)
{
	send_command([x, y](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::MOVE_MOUSE;
		EIOS_Write<std::int32_t>(arguments, x);
		EIOS_Write<std::int32_t>(arguments, y);
	});
}

void ControlCenter::hold_mouse(std::int32_t x, std::int32_t y, std::int32_t button)
{
	send_command([x, y, button](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::HOLD_MOUSE;
		EIOS_Write<std::int32_t>(arguments, x);
		EIOS_Write<std::int32_t>(arguments, y);
		EIOS_Write<std::int32_t>(arguments, button);
	});
}

void ControlCenter::release_mouse(std::int32_t x, std::int32_t y, std::int32_t button)
{
	send_command([x, y, button](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::RELEASE_MOUSE;
		EIOS_Write<std::int32_t>(arguments, x);
		EIOS_Write<std::int32_t>(arguments, y);
		EIOS_Write<std::int32_t>(arguments, button);
	});
}

jobject ControlCenter::reflect_object(const ReflectionHook &hook)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_OBJECT;
		hook.write(arguments);
	});
	
	if (result)
	{
		void* response = get_image_data()->args;
		
		map_lock->lock();
		jobject object = EIOS_Read<jobject>(response);
		map_lock->unlock();
		return object;
	}
	return nullptr;
}

void ControlCenter::reflect_release_object(const jobject object)
{
	send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_RELEASE_OBJECT;
		EIOS_Write(arguments, object);
	});
}

char ControlCenter::reflect_char(const ReflectionHook &hook)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_CHAR;
		hook.write(arguments);
	});
	
	if (result)
	{
		void* response = get_image_data()->args;
		
		map_lock->lock();
		jchar object = EIOS_Read<jchar>(response);
		map_lock->unlock();
		return object;
	}
	return '\0';
}

std::uint8_t ControlCenter::reflect_byte(const ReflectionHook &hook)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_BYTE;
		hook.write(arguments);
	});
	
	if (result)
	{
		void* response = get_image_data()->args;
		
		map_lock->lock();
		jbyte object = EIOS_Read<jbyte>(response);
		map_lock->unlock();
		return object;
	}
	return 0;
}

bool ControlCenter::reflect_boolean(const ReflectionHook &hook)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_BOOLEAN;
		hook.write(arguments);
	});
	
	if (result)
	{
		void* response = get_image_data()->args;
		
		map_lock->lock();
		jboolean object = EIOS_Read<jboolean>(response);
		map_lock->unlock();
		return object;
	}
	return false;
}

std::int16_t ControlCenter::reflect_short(const ReflectionHook &hook)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_SHORT;
		hook.write(arguments);
	});
	
	if (result)
	{
		void* response = get_image_data()->args;
		
		map_lock->lock();
		jshort object = EIOS_Read<jshort>(response);
		map_lock->unlock();
		return object;
	}
	return -1;
}

std::int32_t ControlCenter::reflect_int(const ReflectionHook &hook)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_INT;
		hook.write(arguments);
	});
	
	if (result)
	{
		void* response = get_image_data()->args;
		
		map_lock->lock();
		jint object = EIOS_Read<jint>(response);
		map_lock->unlock();
		return object;
	}
	return -1;
}

std::int64_t ControlCenter::reflect_long(const ReflectionHook &hook)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_LONG;
		hook.write(arguments);
	});
	
	if (result)
	{
		void* response = get_image_data()->args;
		
		map_lock->lock();
		jlong object = EIOS_Read<jlong>(response);
		map_lock->unlock();
		return object;
	}
	return -1;
}

float ControlCenter::reflect_float(const ReflectionHook &hook)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_FLOAT;
		hook.write(arguments);
	});
	
	if (result)
	{
		void* response = get_image_data()->args;
		
		map_lock->lock();
		jfloat object = EIOS_Read<jfloat>(response);
		map_lock->unlock();
		return object;
	}
	return -1;
}

double ControlCenter::reflect_double(const ReflectionHook &hook)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_DOUBLE;
		hook.write(arguments);
	});
	
	if (result)
	{
		void* response = get_image_data()->args;
		
		map_lock->lock();
		jdouble object = EIOS_Read<jdouble>(response);
		map_lock->unlock();
		return object;
	}
	return -1;
}

std::string ControlCenter::reflect_string(const ReflectionHook &hook)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_STRING;
		hook.write(arguments);
	});
	
	if (result)
	{
		void* response = get_image_data()->args;
		
		map_lock->lock();
		std::string object = EIOS_Read(response);
		map_lock->unlock();
		return object;
	}
	return "";
}

jobjectArray ControlCenter::reflect_array(const ReflectionHook &hook)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_ARRAY;
		hook.write(arguments);
	});
	
	if (result)
	{
		void* response = get_image_data()->args;
		
		map_lock->lock();
		jobjectArray object = EIOS_Read<jobjectArray>(response);
		map_lock->unlock();
		return object;
	}
	return nullptr;
}

std::size_t ControlCenter::reflect_array_size(const jobjectArray array)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_ARRAY_SIZE;
		EIOS_Write(arguments, array);
	});
	
	if (result)
	{
		void* response = get_image_data()->args;
		
		map_lock->lock();
		jsize object = EIOS_Read<jsize>(response);
		map_lock->unlock();
		return object;
	}
	return 0;
}

jobject ControlCenter::reflect_array_index(const jobjectArray array, std::size_t index)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_ARRAY_INDEX;
		EIOS_Write(arguments, array);
		EIOS_Write(arguments, static_cast<jsize>(index));
	});
	
	if (result)
	{
		void* response = get_image_data()->args;
		
		map_lock->lock();
		jobject object = EIOS_Read<jobject>(response);
		map_lock->unlock();
		return object;
	}
	return nullptr;
}
