#include "SimbaPlugin.hxx"
#include <memory>
#include <cstring>
#include <unordered_map>
#include <type_traits>

#include "ControlCenter.hxx"

#if defined(_WIN32) || defined(_WIN64)
extern HMODULE module;
#endif

TMemoryManager PLUGIN_MEMORY_MANAGER = {0};
TSimbaMethods PLUGIN_SYNC_METHODS = {0};
TSimbaMemoryAllocators PLUGIN_MEMORY_ALLOCATORS = {0};
extern std::unique_ptr<ControlCenter> control_center;


// MARK: - DECLARATIONS

template<typename T>
int PascalHigh(T* Arr);

template<typename T>
int PascalLength(T* Arr);

template<typename T>
T* AllocateArray(std::size_t size, std::size_t element_size = sizeof(T));

template<typename T>
T* GetArray(void* ptr, std::size_t* size);

template<typename T>
T* AllocateString(std::size_t size, std::size_t element_size = sizeof(T));

template<typename T>
T* GetString(void* ptr);

template<typename T>
T PascalRead(void* ptr);

std::string PascalRead(void* &ptr);

template<typename T>
void PascalWrite(void* ptr, T result);

void PascalWrite(EIOS* eios, void* ptr, void* result, ReflectionArrayType type);


// MARK: - EXPORTS

int GetPluginABIVersion()
{
    return 2;
}

int GetFunctionCount()
{
    return PascalExportCount;
}

int GetTypeCount()
{
	return PascalTypeCount;
}

int GetFunctionInfo(int Index, void** Address, char** Definition)
{
    if (Index < PascalExportCount)
    {
        #if defined(_WIN32) || defined(_WIN64)
        *Address = (void*)GetProcAddress(module, PascalExports[Index * 2]);
        #else
        *Address = (void*)dlsym(RTLD_DEFAULT, PascalExports[Index * 2]);
        #endif
        strcpy(*Definition, PascalExports[Index * 2 + 1]);
        return Index;
    }
    return -1;
}

int GetTypeInfo(int Index, char** Type, char** Definition)
{
	if (Index < PascalTypeCount)
	{
		strcpy(*Type, PascalTypes[Index * 2 + 0]);
		strcpy(*Definition, PascalTypes[Index * 2 + 1]);
		return Index;
	}
	return -1;
}

void SetPluginMemManager(TMemoryManager MemMgr)
{
    PLUGIN_MEMORY_MANAGER = MemMgr;
}

void SetPluginSimbaMethods(TSimbaMethods Methods)
{
	PLUGIN_SYNC_METHODS = Methods;
}

void SetPluginSimbaMemoryAllocators(TSimbaMemoryAllocators Allocators)
{
	PLUGIN_MEMORY_ALLOCATORS = Allocators;
}

void OnAttach(void* info)
{
    control_center.reset();
	EIOS_KillZombieClients();
}

void OnDetach()
{
	control_center.reset();
	EIOS_KillZombieClients();
}


// MARK: - Reflection

typedef struct
{
    std::make_signed_t<std::size_t> refCount;
    std::make_signed_t<std::size_t> length;
    char data[];
} __attribute__((__packed__)) PascalArray;

#if defined(FPC2)
typedef struct
{
    std::make_signed_t<std::size_t> refCount;
    std::make_signed_t<std::size_t> length;
    char data[];
} __attribute__((__packed__)) PascalString;
#else
typedef struct
{
    std::uint16_t codePage;
    std::uint16_t elementSize;
    #if defined(__x86_64__)
    std::uint32_t dummy;
    #endif
    std::make_signed_t<std::size_t> refCount;
    std::make_signed_t<std::size_t> length;
    char data[];
} __attribute__((__packed__)) PascalString;
#endif // defined

typedef struct
{
    char* cls;
    char* field;
    char* desc;
} __attribute__((__packed__)) PascalField;

template<typename T, std::size_t size>
struct __attribute__((__packed__)) StaticPascalArray
{
    const std::make_signed_t<std::size_t> refCount = -1;
    const std::make_signed_t<std::size_t> length = size;
	T data[size] = {0};
};


// MARK: Single Functions

void Pascal_Reflect_Equal(void** Params, void** Result)
{
    EIOS* eios = PascalRead<EIOS*>(Params[0]);
    if (eios)
    {
        jobject first = PascalRead<jobject>(Params[1]);
        jobject second = PascalRead<jobject>(Params[2]);

        PascalWrite(Result, eios->control_center->reflect_is_objects_equal(first, second));
    }
}

void Pascal_Reflect_InstanceOf(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
    if (eios)
    {
        jobject object = PascalRead<jobject>(Params[1]);
        char* cls = PascalRead<char*>(Params[2]);

        PascalWrite(Result, eios->control_center->reflect_instance_of(object, cls));
    }
}

void Pascal_Reflect_Object(void** Params, void** Result)
{
    EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
	    jobject object = PascalRead<jobject>(Params[1]);
	    PascalField* field = PascalRead<PascalField*>(Params[2]);

		ReflectionHook hook{object, field->cls, field->field, field->desc};
		PascalWrite(Result, eios->control_center->reflect_object(hook));
	}
}

void Pascal_Reflect_Release_Object(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
	    jobject object = PascalRead<jobject>(Params[1]);
	    eios->control_center->reflect_release_object(object);
	}
}

void Pascal_Reflect_Release_Objects(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
	    std::size_t length = 0;
	    jobject* objects = GetArray<jobject>(PascalRead<void*>(Params[1]), &length);
		eios->control_center->reflect_release_objects(objects, length);
	}
}

void Pascal_Reflect_Boolean(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
	    jobject object = PascalRead<jobject>(Params[1]);
        PascalField* field = PascalRead<PascalField*>(Params[2]);

		ReflectionHook hook{object, field->cls, field->field, field->desc};
		PascalWrite(Result, eios->control_center->reflect_boolean(hook));
	}
}

void Pascal_Reflect_Char(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
	    jobject object = PascalRead<jobject>(Params[1]);
        PascalField* field = PascalRead<PascalField*>(Params[2]);

		ReflectionHook hook{object, field->cls, field->field, field->desc};
		PascalWrite(Result, eios->control_center->reflect_char(hook));
	}
}

void Pascal_Reflect_Byte(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
	    jobject object = PascalRead<jobject>(Params[1]);
        PascalField* field = PascalRead<PascalField*>(Params[2]);

		ReflectionHook hook{object, field->cls, field->field, field->desc};
		PascalWrite(Result, eios->control_center->reflect_byte(hook));
	}
}

void Pascal_Reflect_Short(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
	    jobject object = PascalRead<jobject>(Params[1]);
        PascalField* field = PascalRead<PascalField*>(Params[2]);

		ReflectionHook hook{object, field->cls, field->field, field->desc};
		PascalWrite(Result, eios->control_center->reflect_short(hook));
	}
}

void Pascal_Reflect_Int(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
	    jobject object = PascalRead<jobject>(Params[1]);
        PascalField* field = PascalRead<PascalField*>(Params[2]);

		ReflectionHook hook{object, field->cls, field->field, field->desc};
		PascalWrite(Result, eios->control_center->reflect_int(hook));
	}
}

void Pascal_Reflect_Long(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
	    jobject object = PascalRead<jobject>(Params[1]);
        PascalField* field = PascalRead<PascalField*>(Params[2]);

		ReflectionHook hook{object, field->cls, field->field, field->desc};
		PascalWrite(Result, eios->control_center->reflect_long(hook));
	}
}

void Pascal_Reflect_Float(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
	    jobject object = PascalRead<jobject>(Params[1]);
        PascalField* field = PascalRead<PascalField*>(Params[2]);

		ReflectionHook hook{object, field->cls, field->field, field->desc};
		PascalWrite(Result, eios->control_center->reflect_float(hook));
	}
}

void Pascal_Reflect_Double(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
	    jobject object = PascalRead<jobject>(Params[1]);
        PascalField* field = PascalRead<PascalField*>(Params[2]);

		ReflectionHook hook{object, field->cls, field->field, field->desc};
		PascalWrite(Result, eios->control_center->reflect_double(hook));
	}
}

void Pascal_Reflect_String(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);

	if (eios)
	{
	    jobject object = PascalRead<jobject>(Params[1]);
        PascalField* field = PascalRead<PascalField*>(Params[2]);

		ReflectionHook hook{object, field->cls, field->field, field->desc};
		std::string result = eios->control_center->reflect_string(hook);

		if (!result.empty())
		{
			char* output = AllocateString<char>(result.length());
			std::memcpy(output, &result[0], result.length());
			output[result.length()] = '\0';

			PascalWrite(Result, output);
		}
	}
}


// MARK: - Array Functions

void Pascal_Reflect_Array(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);

	if (eios)
	{
	    jobject object = PascalRead<jobject>(Params[1]);
        PascalField* field = PascalRead<PascalField*>(Params[2]);

		ReflectionHook hook{object, field->cls, field->field, field->desc};
		PascalWrite(Result, eios->control_center->reflect_array(hook));
	}
}

void Pascal_Reflect_Array_With_Size(void** Params, void** Result)
{
    EIOS* eios = PascalRead<EIOS*>(Params[0]);

	if (eios)
	{
	    jobject object = PascalRead<jobject>(Params[1]);
	    std::size_t* output_size = PascalRead<std::size_t*>(Params[2]);
        PascalField* field = PascalRead<PascalField*>(Params[3]);

		ReflectionHook hook{object, field->cls, field->field, field->desc};
		PascalWrite(Result, eios->control_center->reflect_array_with_size(hook, output_size));
	}
}

void Pascal_Reflect_Array_Size(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);

	if (eios)
	{
	    jarray array = PascalRead<jarray>(Params[1]);
		PascalWrite(Result, eios->control_center->reflect_array_size(array));
	}
}

void Pascal_Reflect_Array_Index_Size(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);

	if (eios)
	{
	    jarray array = PascalRead<jarray>(Params[1]);
		std::int32_t index = PascalRead<std::int32_t>(Params[2]);
		PascalWrite(Result, eios->control_center->reflect_array_size(array, index));
	}
}

void Pascal_Reflect_Array_SingleIndex(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);

	if (eios)
	{
	    jarray array = PascalRead<jarray>(Params[1]);
	    ReflectionArrayType type = PascalRead<ReflectionArrayType>(Params[2]);
	    std::size_t index = PascalRead<std::size_t>(Params[3]);

		void* result = eios->control_center->reflect_array_index(array, type, index, 1);
		PascalWrite(eios, Result, result, type);
	}
}

void Pascal_Reflect_Array_SingleIndex2D(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);

	if (eios)
	{
	    jarray array = PascalRead<jarray>(Params[1]);
	    ReflectionArrayType type = PascalRead<ReflectionArrayType>(Params[2]);
	    std::int32_t x = PascalRead<std::int32_t>(Params[3]);
		std::int32_t y = PascalRead<std::int32_t>(Params[4]);

		void* result = eios->control_center->reflect_array_index2d(array, type, 1, x, y);
		PascalWrite(eios, Result, result, type);
	}
}

void Pascal_Reflect_Array_SingleIndex3D(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);

	if (eios)
	{
	    jarray array = PascalRead<jarray>(Params[1]);
	    ReflectionArrayType type = PascalRead<ReflectionArrayType>(Params[2]);
	    std::int32_t x = PascalRead<std::int32_t>(Params[3]);
		std::int32_t y = PascalRead<std::int32_t>(Params[4]);
		std::int32_t z = PascalRead<std::int32_t>(Params[5]);

		void* result = eios->control_center->reflect_array_index3d(array, type, 1, x, y, z);
		PascalWrite(eios, Result, result, type);
	}
}

void Pascal_Reflect_Array_SingleIndex4D(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);

	if (eios)
	{
	    jarray array = PascalRead<jarray>(Params[1]);
	    ReflectionArrayType type = PascalRead<ReflectionArrayType>(Params[2]);
	    std::int32_t x = PascalRead<std::int32_t>(Params[3]);
		std::int32_t y = PascalRead<std::int32_t>(Params[4]);
		std::int32_t z = PascalRead<std::int32_t>(Params[5]);
		std::int32_t w = PascalRead<std::int32_t>(Params[6]);

		void* result = eios->control_center->reflect_array_index4d(array, type, 1, x, y, z, w);
		PascalWrite(eios, Result, result, type);
	}
}

void Pascal_Reflect_Array_Index(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);

	if (eios)
	{
	    jarray array = PascalRead<jarray>(Params[1]);
	    ReflectionArrayType type = PascalRead<ReflectionArrayType>(Params[2]);
	    std::size_t index = PascalRead<std::size_t>(Params[3]);
	    std::size_t length = PascalRead<std::size_t>(Params[4]);
		
		if (length == 0)
		{
			return;
		}

		void* result = eios->control_center->reflect_array_index(array, type, index, length);

		if (type == ReflectionArrayType::STRING)
		{
			std::size_t element_size = ControlCenter::reflect_size_for_type(type);
			void** buffer = AllocateArray<void*>(length, element_size);

			for (std::size_t i = 0; i < length; ++i)
			{
				buffer[i] = nullptr;
				std::string string = PascalRead(result);
				if (!string.empty())
				{
					char* output = AllocateString<char>(string.length());
					std::memcpy(output, &string[0], string.length());
					output[string.length()] = '\0';

					buffer[i] = output;
				}
			}

			PascalWrite(Result, buffer);
			return;
		}

		std::size_t element_size = ControlCenter::reflect_size_for_type(type);
		void* buffer = AllocateArray<void>(length, element_size);
		std::memcpy(buffer, result, length * element_size);
		PascalWrite(Result, buffer);
	}
}

void Pascal_Reflect_Array_Index2D(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);

	if (eios)
	{
	    jarray array = PascalRead<jarray>(Params[1]);
	    ReflectionArrayType type = PascalRead<ReflectionArrayType>(Params[2]);
	    std::size_t length = PascalRead<std::size_t>(Params[3]);
	    std::int32_t x = PascalRead<std::int32_t>(Params[4]);
	    std::int32_t y = PascalRead<std::int32_t>(Params[5]);
		
		if (length == 0)
		{
			return;
		}

		void* result = eios->control_center->reflect_array_index2d(array, type, length, x, y);

		if (type == ReflectionArrayType::STRING)
		{
			std::size_t element_size = ControlCenter::reflect_size_for_type(type);
			void** buffer = AllocateArray<void*>(length, element_size);

			for (std::size_t i = 0; i < length; ++i)
			{
				buffer[i] = nullptr;
				std::string string = PascalRead(result);
				if (!string.empty())
				{
					char* output = AllocateString<char>(string.length());
					std::memcpy(output, &string[0], string.length());
					output[string.length()] = '\0';

					buffer[i] = output;
				}
			}

			PascalWrite(Result, buffer);
			return;
		}

		std::size_t element_size = ControlCenter::reflect_size_for_type(type);
		void* buffer = AllocateArray<void>(length, element_size);
		std::memcpy(buffer, result, length * element_size);
		PascalWrite(Result, buffer);
	}
}

void Pascal_Reflect_Array_Index3D(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);

	if (eios)
	{
	    jarray array = PascalRead<jarray>(Params[1]);
	    ReflectionArrayType type = PascalRead<ReflectionArrayType>(Params[2]);
	    std::size_t length = PascalRead<std::size_t>(Params[3]);
	    std::int32_t x = PascalRead<std::int32_t>(Params[4]);
	    std::int32_t y = PascalRead<std::int32_t>(Params[5]);
	    std::int32_t z = PascalRead<std::int32_t>(Params[6]);
		
		if (length == 0)
		{
			return;
		}

		void* result = eios->control_center->reflect_array_index3d(array, type, length, x, y, z);

		if (type == ReflectionArrayType::STRING)
		{
			std::size_t element_size = ControlCenter::reflect_size_for_type(type);
			void** buffer = AllocateArray<void*>(length, element_size);

			for (std::size_t i = 0; i < length; ++i)
			{
				buffer[i] = nullptr;
				std::string string = PascalRead(result);
				if (!string.empty())
				{
					char* output = AllocateString<char>(string.length());
					std::memcpy(output, &string[0], string.length());
					output[string.length()] = '\0';

					buffer[i] = output;
				}
			}

			PascalWrite(Result, buffer);
			return;
		}

		std::size_t element_size = ControlCenter::reflect_size_for_type(type);
		void* buffer = AllocateArray<void>(length, element_size);
		std::memcpy(buffer, result, length * element_size);
		PascalWrite(Result, buffer);
	}
}

void Pascal_Reflect_Array_Index4D(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);

	if (eios)
	{
	    jarray array = PascalRead<jarray>(Params[1]);
	    ReflectionArrayType type = PascalRead<ReflectionArrayType>(Params[2]);
	    std::size_t length = PascalRead<std::size_t>(Params[3]);
	    std::int32_t x = PascalRead<std::int32_t>(Params[4]);
	    std::int32_t y = PascalRead<std::int32_t>(Params[5]);
	    std::int32_t z = PascalRead<std::int32_t>(Params[6]);
	    std::int32_t w = PascalRead<std::int32_t>(Params[7]);
		
		if (length == 0)
		{
			return;
		}

		void* result = eios->control_center->reflect_array_index4d(array, type, length, x, y, z, w);

		if (type == ReflectionArrayType::STRING)
		{
			std::size_t element_size = ControlCenter::reflect_size_for_type(type);
			void** buffer = AllocateArray<void*>(length, element_size);

			for (std::size_t i = 0; i < length; ++i)
			{
				buffer[i] = nullptr;
				std::string string = PascalRead(result);
				if (!string.empty())
				{
					char* output = AllocateString<char>(string.length());
					std::memcpy(output, &string[0], string.length());
					output[string.length()] = '\0';

					buffer[i] = output;
				}
			}

			PascalWrite(Result, buffer);
			return;
		}

		std::size_t element_size = ControlCenter::reflect_size_for_type(type);
		void* buffer = AllocateArray<void>(length, element_size);
		std::memcpy(buffer, result, length * element_size);
		PascalWrite(Result, buffer);
	}
}

void Pascal_GetDebugImageBuffer(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
		PascalWrite(Result, eios->control_center->get_debug_image());
	}
}

void Pascal_SetGraphicsDebugging(void** Params, void** Result)
{
    EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
	    bool enabled = PascalRead<bool>(Params[1]);
	    EIOS_SetGraphicsDebugging(eios, enabled);
	}
}

void Pascal_PairClient(void** Params, void** Result)
{
    pid_t pid = PascalRead<pid_t>(Params[0]);
    PascalWrite(Result, EIOS_PairClient(pid));
}

void Pascal_KillClientPID(void** Params, void** Result)
{
    EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
        EIOS_KillClient(eios);
	}
}

void Pascal_KillClient(void** Params, void** Result)
{
    pid_t pid = PascalRead<pid_t>(Params[0]);
    EIOS_KillClientPID(pid);
}

void Pascal_GetClients(void** Params, void** Result)
{
    bool unpaired_only = PascalRead<bool>(Params[0]);
    PascalWrite(Result, EIOS_GetClients(unpaired_only));
}

void Pascal_GetClientPID(void** Params, void** Result)
{
    std::size_t index = PascalRead<std::size_t>(Params[0]);
    PascalWrite(Result, EIOS_GetClientPID(index));
}

void Pascal_Inject(void** Params, void** Result)
{
	#if defined(_WIN32) || defined(_WIN64) || defined(__APPLE__)
    const char* process_name = PascalRead<const char*>(Params[0]);
    if (process_name)
    {
        extern void InjectProcesses(const char* process_name);
        InjectProcesses(process_name);
    }
	#endif
}

void Pascal_Inject_PID(void** Params, void** Result)
{
	#if defined(_WIN32) || defined(_WIN64) || defined(__APPLE__)
    pid_t pid = PascalRead<pid_t>(Params[0]);
    if (pid)
    {
        extern void InjectProcess(pid_t);
        InjectProcess(pid);
    }
	#endif
}

void Pascal_HasFocus(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
		PascalWrite(Result, EIOS_HasFocus(eios));
	}
}

void Pascal_GainFocus(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
		EIOS_GainFocus(eios);
	}
}

void Pascal_LoseFocus(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
		EIOS_LoseFocus(eios);
	}
}

void Pascal_IsInputEnabled(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
		PascalWrite(Result, EIOS_IsInputEnabled(eios));
	}
}

void Pascal_SetInputEnabled(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	if (eios)
	{
		bool enabled = PascalRead<bool>(Params[1]);
		EIOS_SetInputEnabled(eios, enabled);
	}
}

void Pascal_GetRealMousePosition(void** Params, void** Result)
{
	EIOS* eios = PascalRead<EIOS*>(Params[0]);
	std::int32_t* x = static_cast<std::int32_t*>(Params[1]);
	std::int32_t* y = static_cast<std::int32_t*>(Params[2]);
	
	if (eios)
	{
		if (x && y)
		{
			EIOS_GetRealMousePosition(eios, x, y);
		}
	}
	else
	{
		if (x && y)
		{
			*x = std::numeric_limits<std::int32_t>::min();
			*y = std::numeric_limits<std::int32_t>::min();
		}
	}
}

template<typename T>
int PascalHigh(T* Arr)
{
    return reinterpret_cast<int*>(Arr)[-1];
}

template<typename T>
int PascalLength(T* Arr)
{
    return PascalHigh<T>(Arr) + 1;
}

template<typename T>
T* AllocateArray(std::size_t size, std::size_t element_size)
{
    std::size_t new_size = (size * element_size) + sizeof(PascalArray);
    PascalArray* ptr = static_cast<PascalArray*>(PLUGIN_MEMORY_MANAGER.AllocMem(new_size));
    ptr->refCount = 1;
    ptr->length = size - 1;
    return reinterpret_cast<T*>(++ptr);
}

template<typename T>
T* AllocateString(std::size_t size, std::size_t element_size)
{
    std::size_t new_size = (size * element_size) + sizeof(PascalString);
    PascalString* ptr = static_cast<PascalString*>(PLUGIN_MEMORY_MANAGER.AllocMem(new_size + 1));
    ptr->refCount = 1;
    ptr->length = size;
    return reinterpret_cast<T*>(++ptr);
}

template<typename T>
T* GetArray(void* ptr, std::size_t* size)
{
    PascalArray* mem = static_cast<PascalArray*>(ptr);
    return reinterpret_cast<T*>((--mem)->data);
}

template<typename T>
T* GetString(void* ptr)
{
    PascalString* mem = static_cast<PascalString*>(ptr);
    return reinterpret_cast<T*>((--mem)->data);
}

template<typename T>
T PascalRead(void* ptr)
{
    return *static_cast<T*>(ptr);
}

std::string PascalRead(void* &ptr)
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
	return std::string();
}

template<typename T>
void PascalWrite(void* ptr, T result)
{
    *static_cast<T*>(ptr) = result;
}

template<typename T>
void PascalProcess(EIOS* eios, void* ptr, void* result)
{
	T* buffer = reinterpret_cast<T*>(&eios->local_storage);
	*buffer = PascalRead<T>(result);
	PascalWrite(ptr, buffer);
}

void PascalWrite(EIOS* eios, void* ptr, void* result, ReflectionArrayType type)
{
	switch (type)
	{
		case ReflectionArrayType::CHAR:
		{
			PascalProcess<jchar>(eios, ptr, result);
		}
			break;

		case ReflectionArrayType::BYTE:
		{
			PascalProcess<jbyte>(eios, ptr, result);
		}
			break;

		case ReflectionArrayType::BOOL:
		{
			PascalProcess<jboolean>(eios, ptr, result);
		}
			break;

		case ReflectionArrayType::SHORT:
		{
			PascalProcess<jshort>(eios, ptr, result);
		}
			break;

		case ReflectionArrayType::INT:
		{
			PascalProcess<jint>(eios, ptr, result);
		}
			break;

		case ReflectionArrayType::LONG:
		{
			PascalProcess<jlong>(eios, ptr, result);
		}
			break;

		case ReflectionArrayType::FLOAT:
		{
			PascalProcess<jfloat>(eios, ptr, result);
		}
			break;

		case ReflectionArrayType::DOUBLE:
		{
			PascalProcess<jdouble>(eios, ptr, result);
		}
			break;

		case ReflectionArrayType::STRING:
		{
			std::string string = PascalRead(result);
			if (!string.empty())
			{
				char* output = AllocateString<char>(string.length());
				std::memcpy(output, &string[0], string.length());
				output[string.length()] = '\0';

				PascalWrite(ptr, output);
			}
		}
			break;

		case ReflectionArrayType::OBJECT:
		{
			PascalWrite(ptr, PascalRead<jobject>(result));
		}
			break;

		default:
		{
			std::terminate();
		}
			break;
	}
}
