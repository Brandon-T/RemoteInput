//
// Created by Brandon on 2022-09-21.
//

#include "NativePlugin.hxx"

#include <cstring>
#include <unordered_map>
#include <unordered_set>

#include "Platform.hxx"
#include "ControlCenter.hxx"

// MARK: - EXPORTS

void EIOS_Inject(const char* process_name) noexcept
{
    if (process_name)
    {
        std::vector<std::int32_t> result = InjectProcesses(process_name);
        for (std::int32_t pid : result)
        {
            ControlCenter::wait_for_sync(pid);
        }
    }
}

void EIOS_Inject_PID(std::int32_t pid) noexcept
{
    if (pid != -1)
    {
        if (InjectProcess(pid) != -1)
        {
            ControlCenter::wait_for_sync(pid);
        }
    }
}

EIOS* EIOS_From_PID(std::int32_t pid) noexcept
{
    extern std::unordered_map<std::int32_t, EIOS*> clients;
    if (clients.count(pid))
    {
        return clients[pid];
    }
    return nullptr;
}

jobject Reflect_Object(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
    if (eios)
    {
        ReflectionHook hook{object, cls, field, desc};
        return eios->control_center->reflect_object(hook);
    }
    return nullptr;
}

jboolean Reflect_IsSame_Object(EIOS* eios, jobject first, jobject second) noexcept
{
    if (eios)
    {
        return eios->control_center->reflect_is_objects_equal(first, second);
    }
    return false;
}

jboolean Reflect_InstanceOf(EIOS* eios, jobject object, const char* cls) noexcept
{
    if (eios)
    {
        return eios->control_center->reflect_instance_of(object, cls);
    }
    return false;
}

void Reflect_Release_Object(EIOS* eios, jobject object) noexcept
{
    if (eios)
    {
        //Make sure we're not freeing null objects..
        if (object)
        {
            eios->control_center->reflect_release_object(object);
        }
    }
}

void Reflect_Release_Objects(EIOS* eios, jobject* objects, std::size_t amount) noexcept
{
    if (eios)
    {
        //Make sure we're not freeing null objects..
        if (objects && amount > 0)
        {
            //This is the fastest possible way to remove duplicates..
            //Do NOT use `unordered_set` constructor instead of the for-loop.
            //It is slower than emplace/insert.
            std::unordered_set<jobject> set;
            for (std::size_t i = 0; i < amount; ++i)
            {
                set.insert(objects[i]);
            }

            //Remove null objects
            set.erase(nullptr);

            //Create a contiguous array of objects for the client.
            std::vector<jobject> objects;
            objects.assign(set.begin(), set.end());

            //Make sure we're not freeing null objects..
            if (objects.size())
            {
                eios->control_center->reflect_release_objects(objects.data(), objects.size());
            }
        }
    }
}

bool Reflect_Bool(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
    if (eios)
    {
        ReflectionHook hook{object, cls, field, desc};
        return eios->control_center->reflect_boolean(hook);
    }
    return false;
}

char Reflect_Char(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
    if (eios)
    {
        ReflectionHook hook{object, cls, field, desc};
        return eios->control_center->reflect_char(hook);
    }
    return '\0';
}

std::uint8_t Reflect_Byte(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
    if (eios)
    {
        ReflectionHook hook{object, cls, field, desc};
        return eios->control_center->reflect_byte(hook);
    }
    return 0;
}

std::int16_t Reflect_Short(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
    if (eios)
    {
        ReflectionHook hook{object, cls, field, desc};
        return eios->control_center->reflect_short(hook);
    }
    return -1;
}

std::int32_t Reflect_Int(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
    if (eios)
    {
        ReflectionHook hook{object, cls, field, desc};
        return eios->control_center->reflect_int(hook);
    }
    return -1;
}

std::int64_t Reflect_Long(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
    if (eios)
    {
        ReflectionHook hook{object, cls, field, desc};
        return eios->control_center->reflect_long(hook);
    }
    return -1;
}

float Reflect_Float(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
    if (eios)
    {
        ReflectionHook hook{object, cls, field, desc};
        return eios->control_center->reflect_float(hook);
    }
    return -1.0;
}

double Reflect_Double(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
    if (eios)
    {
        ReflectionHook hook{object, cls, field, desc};
        return eios->control_center->reflect_double(hook);
    }
    return -1.0;
}

void Reflect_String(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc, char* output, std::size_t output_size) noexcept
{
    if (eios)
    {
        ReflectionHook hook{object, cls, field, desc};
        std::string result = eios->control_center->reflect_string(hook);
        std::size_t length = std::min(result.length(), output_size);
        std::memcpy(output, &result[0], length);
    }
}

jarray Reflect_Array(EIOS* eios, jobject object, const char* cls, const char* field, const char* desc) noexcept
{
    if (eios)
    {
        ReflectionHook hook{object, cls, field, desc};
        return eios->control_center->reflect_array(hook);
    }
    return nullptr;
}

jarray Reflect_Array_With_Size(EIOS* eios, jobject object, std::size_t* output_size, const char* cls, const char* field, const char* desc) noexcept
{
    if (eios)
    {
        ReflectionHook hook{object, cls, field, desc};
        return eios->control_center->reflect_array(hook, output_size);
    }
    return nullptr;
}

std::size_t Reflect_Array_Size(EIOS* eios, jarray array) noexcept
{
    if (eios)
    {
        return eios->control_center->reflect_array_size(array);
    }
    return 0;
}

void* Reflect_Array_Index(EIOS* eios, jarray array, ReflectionType type, std::size_t index, std::size_t length) noexcept
{
    if (eios)
    {
        ImageData* image_data = eios->control_center->reflect_array(array, type, length, index);
        image_data->data_stream().seekg(sizeof(std::size_t));
        return image_data->io_buffer(std::ios::in);
    }
    return nullptr;
}

void* Reflect_Array_Index2D(EIOS* eios, jarray array, ReflectionType type, std::size_t x, std::size_t y, std::size_t length) noexcept
{
    if (eios)
    {
        ImageData* image_data = eios->control_center->reflect_array(array, type, length, x, y);
        image_data->data_stream().seekg(sizeof(std::size_t));
        return image_data->io_buffer(std::ios::in);
    }
    return nullptr;
}

void* Reflect_Array_Index3D(EIOS* eios, jarray array, ReflectionType type, std::size_t x, std::size_t y, std::size_t z, std::size_t length) noexcept
{
    if (eios)
    {
        ImageData* image_data = eios->control_center->reflect_array(array, type, length, x, y, z);
        image_data->data_stream().seekg(sizeof(std::size_t));
        return image_data->io_buffer(std::ios::in);
    }
    return nullptr;
}

void* Reflect_Array_Index4D(EIOS* eios, jarray array, ReflectionType type, std::size_t x, std::size_t y, std::size_t z, std::size_t w, std::size_t length) noexcept
{
    if (eios)
    {
        ImageData* image_data = eios->control_center->reflect_array(array, type, length, x, y, z, w);
        image_data->data_stream().seekg(sizeof(std::size_t));
        return image_data->io_buffer(std::ios::in);
    }
    return nullptr;
}

void* Reflect_Array_Indices(EIOS* eios, jarray array, ReflectionType type, std::int32_t* indices, std::size_t length) noexcept
{
    if (eios)
    {
        return eios->control_center->reflect_array_indices(array, type, indices, length);
    }
    return nullptr;
}