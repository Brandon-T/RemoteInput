#include "ControlCenter.hxx"
#include <thread>
#include <unordered_map>
#include <cstring>
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

	if (length > 0)
	{
		std::string result = std::string(reinterpret_cast<const char*>(ptr), length);
		ptr = static_cast<char*>(ptr) + (result.length() * sizeof(char));
		ptr = static_cast<char*>(ptr) + 1;
		return result;
	}
	return "";
}

template<typename T>
void EIOS_Write(void* &ptr, T result)
{
	*static_cast<T*>(ptr) = result;
	ptr = static_cast<T*>(ptr) + 1;
}

void EIOS_Write(void* &ptr, const std::string &result)
{
	if (result.empty())
	{
		*static_cast<std::size_t*>(ptr) = 0;
		ptr = static_cast<std::size_t*>(ptr) + 1;
		return;
	}
	
	*static_cast<std::size_t*>(ptr) = result.length();
	ptr = static_cast<std::size_t*>(ptr) + 1;

	memcpy(ptr, &result[0], result.length() * sizeof(char));
	ptr = static_cast<char*>(ptr) + (result.length() * sizeof(char));

	*static_cast<char*>(ptr) = '\0';
	ptr = static_cast<char*>(ptr) + 1;
}

ControlCenter::ControlCenter(pid_t pid, bool is_controller, std::unique_ptr<Reflection> &&reflector) : pid(pid),  is_controller(is_controller), stopped(is_controller), map_lock(), command_signal(), response_signal(), reflector(std::move(reflector)), io_controller()
{
	if (pid <= 0)
	{
		throw std::runtime_error("Invalid Process ID");
	}

	if (!init_maps())
	{
		throw std::runtime_error("Cannot Initialize Maps");
	}

//	if (!init_locks())
//	{
//		throw std::runtime_error("Cannot Initialize Locks");
//	}

	if (!init_signals())
	{
		throw std::runtime_error("Cannot Initialize Signals");
	}

	if (!is_controller)
	{
		this->set_parent(-1);
		
		std::thread thread = std::thread([&]{

            #if defined(_WIN32) || defined(_WIN64)
            HANDLE this_process = GetCurrentProcess();
            SetPriorityClass(this_process, NORMAL_PRIORITY_CLASS);

            HANDLE this_thread = GetCurrentThread();
            SetThreadPriority(this_thread, THREAD_PRIORITY_HIGHEST);
            #else
            pthread_t this_thread = pthread_self();
            struct sched_param params = {0};
            params.sched_priority = sched_get_priority_max(SCHED_FIFO);
            pthread_setschedparam(this_thread, SCHED_FIFO, &params);
            #endif // defined

			if (this->reflector->Attach())
			{
				this->io_controller = std::make_unique<InputOutput>(this->reflector.get());

				while(!stopped)
                {
					if (!command_signal || /*!map_lock ||*/ !response_signal || !memory_map)
					{
						break;
					}

					command_signal->wait();

//					while(!command_signal->try_wait())
//					{
//						usleep(1);
//						continue;
//					}

					if (stopped)
					{
						break;
					}

					process_command();
					response_signal->signal();
				}

				if (this->io_controller)
                {
                    this->io_controller.reset();
                }


                if (this->reflector)
                {
                    this->reflector->Detach();
                    this->reflector.reset();
                }
			}
		});

		thread.detach();
	}
}

ControlCenter::~ControlCenter()
{
	terminate();
	//reflector.reset();

	if (map_lock)
	{
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
			
		case EIOSCommand::KILL_APPLICATION:
		{
			stopped = true;
			response_signal->signal();
			
			if (this->io_controller)
			{
				this->io_controller.reset();
			}


			if (this->reflector)
			{
				this->reflector->Detach();
				this->reflector.reset();
			}
			
			std::exit(0);
		}
			break;

		case EIOSCommand::GET_TARGET_DIMENSIONS:
		{
			EIOS_Write<std::int32_t>(response, get_width());
			EIOS_Write<std::int32_t>(response, get_height());
		}
			break;

		case EIOSCommand::GET_MOUSE:
		{
			std::int32_t x = -1;
			std::int32_t y = -1;

			io_controller->get_mouse_position(&x, &y);

			EIOS_Write<std::int32_t>(response, x);
			EIOS_Write<std::int32_t>(response, y);
		}
			break;

		case EIOSCommand::MOVE_MOUSE:
		{
			std::int32_t x = EIOS_Read<std::int32_t>(arguments);
			std::int32_t y = EIOS_Read<std::int32_t>(arguments);
			io_controller->move_mouse(x, y);
		}
			break;

		case EIOSCommand::HOLD_MOUSE:
		{
			std::int32_t x = EIOS_Read<std::int32_t>(arguments);
			std::int32_t y = EIOS_Read<std::int32_t>(arguments);
			std::int32_t button = EIOS_Read<std::int32_t>(arguments);
			io_controller->hold_mouse(x, y, button);
		}
			break;

		case EIOSCommand::RELEASE_MOUSE:
		{
			std::int32_t x = EIOS_Read<std::int32_t>(arguments);
			std::int32_t y = EIOS_Read<std::int32_t>(arguments);
			std::int32_t button = EIOS_Read<std::int32_t>(arguments);
			io_controller->release_mouse(x, y, button);
		}
			break;

		case EIOSCommand::SCROLL_MOUSE:
		{
			std::int32_t x = EIOS_Read<std::int32_t>(arguments);
			std::int32_t y = EIOS_Read<std::int32_t>(arguments);
			std::int32_t lines = EIOS_Read<std::int32_t>(arguments);
			io_controller->scroll_mouse(x, y, lines);
		}
			break;

		case EIOSCommand::IS_MOUSE_HELD:
		{
			std::int32_t button = EIOS_Read<std::int32_t>(arguments);
			auto result = io_controller->is_mouse_held(button);
			EIOS_Write(response, result);
		}
			break;

		case EIOSCommand::SEND_STRING:
		{
			std::string string = EIOS_Read(arguments);
			std::int32_t keywait = EIOS_Read<std::int32_t>(arguments);
			std::int32_t keymodwait = EIOS_Read<std::int32_t>(arguments);
			io_controller->send_string(string, keywait, keymodwait);
		}
			break;

		case EIOSCommand::HOLD_KEY:
		{
			std::int32_t keycode = EIOS_Read<std::int32_t>(arguments);
			io_controller->hold_key(keycode);
		}
			break;

		case EIOSCommand::RELEASE_KEY:
		{
			std::int32_t keycode = EIOS_Read<std::int32_t>(arguments);
			io_controller->release_key(keycode);
		}
			break;

		case EIOSCommand::IS_KEY_HELD:
		{
			std::int32_t keycode = EIOS_Read<std::int32_t>(arguments);
			auto result = io_controller->is_key_held(keycode);
			EIOS_Write(response, result);
		}
			break;

		case EIOSCommand::REFLECT_OBJECT:
		{
			ReflectionHook hook;
			hook.read(arguments);

			auto result = reflector->getField<jobject>(hook.object, hook);
			EIOS_Write(response, result);
		}
			break;

		case EIOSCommand::REFLECT_RELEASE_OBJECT:
		{
			jobject object = EIOS_Read<jobject>(arguments);
			if (object)
			{
				reflector->getEnv()->DeleteGlobalRef(object);
			}
		}
			break;

		case EIOSCommand::REFLECT_RELEASE_OBJECTS:
		{
			jsize size = EIOS_Read<jsize>(arguments);
			for (jsize i = 0; i < size; ++i)
			{
				jobject object = EIOS_Read<jobject>(arguments);
				if (object)
				{
					reflector->getEnv()->DeleteGlobalRef(object);
				}
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

			auto result = reflector->getField<std::string>(hook.object, hook);
			EIOS_Write(response, result);
		}
			break;

		case EIOSCommand::REFLECT_ARRAY:
		{
			ReflectionHook hook;
			hook.read(arguments);

			auto result = reflector->getField<jarray>(hook.object, hook);
			EIOS_Write(arguments, result);
		}
			break;

        case EIOSCommand::REFLECT_ARRAY_WITH_SIZE:
        {
            ReflectionHook hook;
			hook.read(arguments);

			auto result = reflector->getField<jarray>(hook.object, hook);
			jsize length = reflector->getEnv()->GetArrayLength(result);
			EIOS_Write(arguments, length);
			EIOS_Write(arguments, result);
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
			jarray array = EIOS_Read<jarray>(arguments);
			process_reflect_array_index(array, arguments, response, 1);
		}
			break;

		case EIOSCommand::REFLECT_ARRAY_INDEX2D:
		{
			jarray array = EIOS_Read<jarray>(arguments);
			process_reflect_array_index(array, arguments, response, 2);
		}
			break;

		case EIOSCommand::REFLECT_ARRAY_INDEX3D:
		{
			jarray array = EIOS_Read<jarray>(arguments);
			process_reflect_array_index(array, arguments, response, 3);
		}
			break;

		case EIOSCommand::REFLECT_ARRAY_INDEX4D:
		{
			jarray array = EIOS_Read<jarray>(arguments);
			process_reflect_array_index(array, arguments, response, 4);
		}
			break;
	}
}

void ControlCenter::process_reflect_array_index(jarray array, void* &arguments, void* &response, int dimensions)
{
	auto write_result = [this](jarray array, ReflectionArrayType type, jsize index, jsize length, void* &response) -> void {
		if (!array)
		{
			return EIOS_Write(response, nullptr);
		}

		if (length == 0)
        {
            length = reflector->getEnv()->GetArrayLength(array);
        }

		//Maybe better to use GetPrimitiveArrayCritical + memcpy
		switch (type) {
			case ReflectionArrayType::CHAR:
			{
				reflector->getEnv()->GetCharArrayRegion(static_cast<jcharArray>(array), index, length, static_cast<jchar*>(response));
			}
				break;

			case ReflectionArrayType::BYTE:
			{
				reflector->getEnv()->GetByteArrayRegion(static_cast<jbyteArray>(array), index, length, static_cast<jbyte*>(response));
			}
				break;

			case ReflectionArrayType::BOOL:
			{
				reflector->getEnv()->GetBooleanArrayRegion(static_cast<jbooleanArray>(array), index, length, static_cast<jboolean*>(response));
			}
				break;

			case ReflectionArrayType::SHORT:
			{
				reflector->getEnv()->GetShortArrayRegion(static_cast<jshortArray>(array), index, length, static_cast<jshort*>(response));
			}
				break;

			case ReflectionArrayType::INT:
			{
				reflector->getEnv()->GetIntArrayRegion(static_cast<jintArray>(array), index, length, static_cast<jint*>(response));
			}
				break;

			case ReflectionArrayType::LONG:
			{
				reflector->getEnv()->GetLongArrayRegion(static_cast<jlongArray>(array), index, length, static_cast<jlong*>(response));
			}
				break;

			case ReflectionArrayType::FLOAT:
			{
				reflector->getEnv()->GetFloatArrayRegion(static_cast<jfloatArray>(array), index, length, static_cast<jfloat*>(response));
			}
				break;

			case ReflectionArrayType::DOUBLE:
			{
				reflector->getEnv()->GetDoubleArrayRegion(static_cast<jdoubleArray>(array), index, length, static_cast<jdouble*>(response));
			}
				break;

			case ReflectionArrayType::STRING:
			{
				if (length > 1)
				{
					for (jsize i = 0; i < length; ++i)
					{
						auto element = reflector->getEnv()->GetObjectArrayElement(static_cast<jobjectArray>(array), index + i);
						std::string result = reflector->getField<std::string>(static_cast<jstring>(element));
						EIOS_Write(response, result);
						reflector->getEnv()->DeleteLocalRef(element);
					}
				}
				else
				{
					auto element = reflector->getEnv()->GetObjectArrayElement(static_cast<jobjectArray>(array), index);
					std::string result = reflector->getField<std::string>(static_cast<jstring>(element));
					EIOS_Write(response, result);
					reflector->getEnv()->DeleteLocalRef(element);
				}
			}
				break;

			case ReflectionArrayType::OBJECT:
			{
				if (length > 1)
				{
					for (jsize i = 0; i < length; ++i)
					{
						auto result = reflector->getEnv()->GetObjectArrayElement(static_cast<jobjectArray>(array), index + i);
						EIOS_Write(response, result ? reflector->getEnv()->NewGlobalRef(result) : nullptr);
						reflector->getEnv()->DeleteLocalRef(result);
					}
				}
				else
				{
					auto result = reflector->getEnv()->GetObjectArrayElement(static_cast<jobjectArray>(array), index);
					EIOS_Write(response, result ? reflector->getEnv()->NewGlobalRef(result) : nullptr);
					reflector->getEnv()->DeleteLocalRef(result);
				}
			}
				break;
		}
	};

	ReflectionArrayType type = EIOS_Read<ReflectionArrayType>(arguments);

	if (dimensions == 1)
	{
		jsize index = EIOS_Read<jsize>(arguments);
		jsize length = EIOS_Read<jsize>(arguments);
		write_result(array, type, index, length, response);
		return;
	}

	array = static_cast<jarray>(reflector->getEnv()->NewLocalRef(array));

	for (int i = 0; i < dimensions - 1; ++i)
	{
		jsize index = EIOS_Read<jsize>(arguments);

		if (array)
		{
			jarray local = std::exchange(array, static_cast<jarray>(reflector->getEnv()->GetObjectArrayElement(static_cast<jobjectArray>(array), index)));
			reflector->getEnv()->DeleteLocalRef(local);
		}
	}

	jsize index = EIOS_Read<jsize>(arguments);
	jsize length = EIOS_Read<jsize>(arguments);
	write_result(array, type, index, length, response);
	reflector->getEnv()->DeleteLocalRef(array);
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
	int debug_size = width * height * 4 * sizeof(std::uint8_t);
    int extra_size = (1024 * sizeof(std::int32_t));
	std::int32_t map_size = sizeof(ImageData) + image_size + debug_size + extra_size;
	memory_map = std::make_unique<MemoryMap<char>>(mapName, map_size, std::ios::in | std::ios::out);
	bool result = memory_map && memory_map->open() && memory_map->map();
	if (result)
	{
		update_dimensions(width, height);
	}
	return result;
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

	command_signal = std::make_unique<Signal>(readName);
    response_signal = std::make_unique<Signal>(writeName);
    return command_signal && response_signal;
}

void ControlCenter::kill_zombie_process(pid_t pid)
{
	#if defined(_WIN32) || defined(_WIN64)
	
	#elif defined(__linux__)
	char buffer[256] = {0};
	
	//Kill Memory Maps
	sprintf(buffer, "RemoteInput_%d", pid);
	shm_unlink(buffer);
	
	//Kill Locks
	sprintf(buffer, "/RemoteInput_Lock_%d", pid);
	shm_unlink(buffer);
	
	//Kill Signals
	sprintf(buffer, "/RemoteInput_ControlCenter_EventRead_%d", pid);
	sem_unlink(buffer);
	
	sprintf(buffer, "/RemoteInput_ControlCenter_EventWrite_%d", pid);
	sem_unlink(buffer);
	#else
	char buffer[256] = {0};
	
	//Kill Memory Maps
	sprintf(buffer, "RemoteInput_%d", pid);
	shm_unlink(buffer);
	
	//Kill Locks
	sprintf(buffer, "/RemoteInput_Lock_%d", pid);
	shm_unlink(buffer);
	
	//Kill Signals
	sprintf(buffer, "/RI_CC_EventRead_%d", pid);
	sem_unlink(buffer);
	
	sprintf(buffer, "/RI_CC_EventWrite_%d", pid);
	sem_unlink(buffer);
	#endif
}

bool ControlCenter::hasReflector()
{
	return reflector != nullptr;
}

ImageData* ControlCenter::get_image_data() const
{
	return memory_map && memory_map->is_mapped() ? static_cast<ImageData*>(memory_map->data()) : nullptr;
}

bool ControlCenter::send_command(std::function<void(ImageData*)> &&writer)
{
	writer(get_image_data());
	command_signal->signal();
	
	static const std::int64_t timeout = 5000;
	std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	while(elapsed_time<std::chrono::milliseconds>(now) < timeout)
	{
		if (response_signal->try_wait())
		{
			return true;
		}
		yield_thread(std::chrono::nanoseconds(1));
	}
	
	return false;
}

std::int32_t ControlCenter::parent_id() const
{
	return memory_map && memory_map->is_mapped() ? get_image_data()->parentId : -1;
}

std::int32_t ControlCenter::get_width() const
{
	return memory_map && memory_map->is_mapped() ? get_image_data()->width : -1;
}

std::int32_t ControlCenter::get_height() const
{
	return memory_map && memory_map->is_mapped() ? get_image_data()->height : -1;
}

std::uint8_t* ControlCenter::get_image() const
{
	ImageData* image_data = get_image_data();
	return image_data ? reinterpret_cast<std::uint8_t*>(image_data) + sizeof(ImageData) : nullptr;
}

std::uint8_t* ControlCenter::get_debug_image() const
{
	std::uint8_t* image = get_image();
	return image ? image + (get_width() * get_height() * 4) : nullptr;
}

bool ControlCenter::get_debug_graphics() const
{
	if (memory_map && memory_map->is_mapped())
	{
		return get_image_data()->debug_graphics;
	}
	return false;
}

void ControlCenter::set_debug_graphics(bool enabled)
{
	if (memory_map && memory_map->is_mapped())
	{
		get_image_data()->debug_graphics = enabled;
	}
}

pid_t ControlCenter::get_parent()
{
	if (memory_map && memory_map->is_mapped())
	{
		return get_image_data()->parentId;
	}
	return -1;
}

void ControlCenter::set_parent(pid_t pid)
{
	if (memory_map && memory_map->is_mapped())
	{
		get_image_data()->parentId = pid;
	}
}

bool ControlCenter::controller_exists(pid_t pid)
{
	char mapName[256] = {0};
	#if defined(_WIN32) || defined(_WIN64)
    sprintf(mapName, "Local\\RemoteInput_%d", pid);
	#else
	sprintf(mapName, "RemoteInput_%d", pid);
	#endif

	return MemoryMap<char>(mapName, std::ios::in).open();
}

bool ControlCenter::controller_is_paired(pid_t pid, bool* exists)
{
	char mapName[256] = {0};
	#if defined(_WIN32) || defined(_WIN64)
    sprintf(mapName, "Local\\RemoteInput_%d", pid);
	#else
	sprintf(mapName, "RemoteInput_%d", pid);
	#endif

	*exists = false;
	auto memory = MemoryMap<char>(mapName, std::ios::in);
	if (memory.open())
	{
		*exists = true;
		if (memory.map(sizeof(ImageData)))
		{
			bool is_paired = static_cast<ImageData*>(memory.data())->parentId != -1;
			memory.unmap(sizeof(ImageData));
			return is_paired;
		}
	}
	
	return false;
}

void ControlCenter::kill_process(pid_t pid)
{
	send_command([](ImageData* image_data) {
		image_data->command = EIOSCommand::KILL_APPLICATION;
	});
}

void ControlCenter::update_dimensions(std::int32_t width, std::int32_t height)
{
	if (memory_map && memory_map->is_mapped())
	{
		get_image_data()->width = width;
		get_image_data()->height = height;
	}
}

void ControlCenter::get_target_dimensions(std::int32_t* width, std::int32_t* height)
{
	if (memory_map && memory_map->is_mapped())
	{
		*width = get_width();
		*height = get_height();
	}
	else
	{
		*width = -1;
		*height = -1;
	}
}

void ControlCenter::get_mouse_position(std::int32_t* x, std::int32_t* y)
{
	auto result = send_command([](ImageData* image_data) {
		image_data->command = EIOSCommand::GET_MOUSE;
	});

	if (result)
	{
		void* arguments = get_image_data()->args;
		*x = EIOS_Read<std::int32_t>(arguments);
		*y = EIOS_Read<std::int32_t>(arguments);
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

void ControlCenter::scroll_mouse(std::int32_t x, std::int32_t y, std::int32_t lines)
{
	send_command([x, y, lines](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::SCROLL_MOUSE;
		EIOS_Write<std::int32_t>(arguments, x);
		EIOS_Write<std::int32_t>(arguments, y);
		EIOS_Write<std::int32_t>(arguments, lines);
	});
}

bool ControlCenter::is_mouse_held(std::int32_t button)
{
	auto result = send_command([button](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::IS_MOUSE_HELD;
		EIOS_Write<std::int32_t>(arguments, button);
	});

	if (result)
	{
		void* arguments = get_image_data()->args;
		return EIOS_Read<bool>(arguments);
	}
	return false;
}

void ControlCenter::send_string(const char* string, std::int32_t keywait, std::int32_t keymodwait)
{
	send_command([string, keywait, keymodwait](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::SEND_STRING;
		EIOS_Write(arguments, std::string(string));
		EIOS_Write<std::int32_t>(arguments, keywait);
		EIOS_Write<std::int32_t>(arguments, keymodwait);
	});
}

void ControlCenter::hold_key(std::int32_t key)
{
	send_command([key](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::HOLD_KEY;
		EIOS_Write<std::int32_t>(arguments, key);
	});
}

void ControlCenter::release_key(std::int32_t key)
{
	send_command([key](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::RELEASE_KEY;
		EIOS_Write<std::int32_t>(arguments, key);
	});
}

bool ControlCenter::is_key_held(std::int32_t key)
{
	auto result = send_command([key](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::IS_KEY_HELD;
		EIOS_Write<std::int32_t>(arguments, key);
	});

	if (result)
	{
		void* arguments = get_image_data()->args;
		return EIOS_Read<bool>(arguments);
	}
	return false;
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
		jobject object = EIOS_Read<jobject>(response);
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

void ControlCenter::reflect_release_objects(const jobject* array, std::size_t length)
{
	send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_RELEASE_OBJECTS;
		EIOS_Write(arguments, static_cast<jsize>(length));
		std::memcpy(arguments, array, ControlCenter::reflect_size_for_type(ReflectionArrayType::OBJECT) * length);
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
		jchar object = EIOS_Read<jchar>(response);
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
		jbyte object = EIOS_Read<jbyte>(response);
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
		jboolean object = EIOS_Read<jboolean>(response);
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
		jshort object = EIOS_Read<jshort>(response);
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
		jint object = EIOS_Read<jint>(response);
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
		jlong object = EIOS_Read<jlong>(response);
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
		jfloat object = EIOS_Read<jfloat>(response);
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
		jdouble object = EIOS_Read<jdouble>(response);
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
		std::string object = EIOS_Read(response);
		return object;
	}
	return "";
}

jarray ControlCenter::reflect_array(const ReflectionHook &hook)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_ARRAY;
		hook.write(arguments);
	});

	if (result)
	{
		void* response = get_image_data()->args;
		jarray object = EIOS_Read<jarray>(response);
		return object;
	}
	return nullptr;
}

jarray ControlCenter::reflect_array_with_size(const ReflectionHook &hook, std::size_t* output_size)
{
    auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_ARRAY_WITH_SIZE;
		hook.write(arguments);
	});

	if (result)
	{
		void* response = get_image_data()->args;
		*output_size = EIOS_Read<jsize>(response);
		jarray object = EIOS_Read<jarray>(response);
		return object;
	}
	return nullptr;
}

std::size_t ControlCenter::reflect_array_size(const jarray array)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_ARRAY_SIZE;
		EIOS_Write(arguments, array);
	});

	if (result)
	{
		void* response = get_image_data()->args;
		jsize object = EIOS_Read<jsize>(response);
		return object;
	}
	return 0;
}

void* ControlCenter::reflect_array_index(const jarray array, ReflectionArrayType type, std::size_t index, std::size_t length)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_ARRAY_INDEX;
		EIOS_Write(arguments, array);
		EIOS_Write(arguments, type);
		EIOS_Write(arguments, static_cast<jsize>(index));
		EIOS_Write(arguments, static_cast<jsize>(length));
	});

	if (result)
	{
		return get_image_data()->args;
	}
	return nullptr;
}

void* ControlCenter::reflect_array_index2d(const jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_ARRAY_INDEX2D;
		EIOS_Write(arguments, array);
		EIOS_Write(arguments, type);
		EIOS_Write(arguments, static_cast<jsize>(x));
		EIOS_Write(arguments, static_cast<jsize>(y));
		EIOS_Write(arguments, static_cast<jsize>(length));
	});

	if (result)
	{
		return get_image_data()->args;
	}
	return nullptr;
}

void* ControlCenter::reflect_array_index3d(const jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_ARRAY_INDEX3D;
		EIOS_Write(arguments, array);
		EIOS_Write(arguments, type);
		EIOS_Write(arguments, static_cast<jsize>(x));
		EIOS_Write(arguments, static_cast<jsize>(y));
		EIOS_Write(arguments, static_cast<jsize>(z));
		EIOS_Write(arguments, static_cast<jsize>(length));
	});

	if (result)
	{
		return get_image_data()->args;
	}
	return nullptr;
}

void* ControlCenter::reflect_array_index4d(const jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t w)
{
	auto result = send_command([&](ImageData* image_data) {
		void* arguments = image_data->args;
		image_data->command = EIOSCommand::REFLECT_ARRAY_INDEX4D;
		EIOS_Write(arguments, array);
		EIOS_Write(arguments, type);
		EIOS_Write(arguments, static_cast<jsize>(x));
		EIOS_Write(arguments, static_cast<jsize>(y));
		EIOS_Write(arguments, static_cast<jsize>(z));
		EIOS_Write(arguments, static_cast<jsize>(w));
		EIOS_Write(arguments, static_cast<jsize>(length));
	});

	if (result)
	{
		return get_image_data()->args;
	}
	return nullptr;
}

std::size_t ControlCenter::reflect_size_for_type(ReflectionArrayType type)
{
	static std::size_t mapping[] = {
		sizeof(jchar),
		sizeof(jbyte),
		sizeof(jboolean),
		sizeof(jshort),
		sizeof(jint),
		sizeof(jlong),
		sizeof(jfloat),
		sizeof(jdouble),
		sizeof(jstring),
		sizeof(jobject)
	};

	return mapping[static_cast<std::uint8_t>(type)];
}

Component ControlCenter::reflect_canvas()
{
	Applet applet{reflector->getEnv(), reflector->getApplet(), false};
	return applet.getComponent(0);
}

void ControlCenter::get_applet_dimensions(std::int32_t* x, std::int32_t* y, std::int32_t* width, std::int32_t* height)
{
    if (io_controller)
    {
        io_controller->get_applet_dimensions(*x, *y, *width, *height);
    }
}

void ControlCenter::get_applet_mouse_position(std::int32_t* x, std::int32_t* y)
{
    if (io_controller)
    {
        io_controller->get_applet_mouse_position(*x, *y);
    }
}
