#ifndef ControlCenter_HXX_INCLUDED
#define ControlCenter_HXX_INCLUDED

#include <memory>
#include <atomic>
#include "Signal.hxx"
#include "Reflection.hxx"
#include "Applet.hxx"
#include "MemoryMapStream.hxx"
#include "InputOutput.hxx"
#include "TypeTraits.hxx"
#include "EIOSTypes.hxx"


class RemoteVM;

class ControlCenter final {
private:
    using Signal = ::Signal<Semaphore>;

    std::int32_t pid;
    bool is_controller;
    std::atomic_bool stopped;
    std::unique_ptr<Mutex> map_lock;
    std::unique_ptr<Signal> command_signal;
    std::unique_ptr<Signal> response_signal;
    std::unique_ptr<AtomicSignal> sync_signal;
    std::unique_ptr<Reflection> reflector;
    std::unique_ptr<MemoryMapStream> memory_map;
    std::unique_ptr<InputOutput> io_controller;
    std::unique_ptr<RemoteVM> remote_vm;

    bool init_maps() noexcept;
    bool init_locks() noexcept;
    bool init_wait() noexcept;
    bool init_signals() noexcept;
    void process_command() noexcept;

    ImageData* get_image_data() const noexcept;

    template<typename Func>
    bool send_command(Func &&sender) const noexcept;

    void send_array_response_index_length(Stream &stream, jarray array, ReflectionType type, std::size_t index, std::size_t length) const noexcept;
    void send_array_response_indices(Stream &stream, jarray array, ReflectionType type, std::int32_t* indices, std::size_t length) const noexcept;
    void process_reflect_array_indices(Stream &stream, jarray array) const noexcept;
    void process_reflect_array_index_length(Stream &stream, jarray array, ReflectionType type, std::size_t length, std::size_t dimensions) const noexcept;
    void process_reflect_array_all(Stream &stream, jarray array, ReflectionType type, std::size_t dimensions) const noexcept;

public:
    ControlCenter(std::int32_t pid, bool is_controller, std::unique_ptr<Reflection> &&reflector);
    ~ControlCenter();

    void terminate() noexcept;
    bool hasReflector() const noexcept;
    void set_parent_process_id(std::int32_t pid) const noexcept;
    void set_parent_thread_id(std::int32_t tid) const noexcept;
    void update_dimensions(std::int32_t width, std::int32_t height) const noexcept;

    static void signal_sync(std::int32_t pid) noexcept;
    static void wait_for_sync(std::int32_t pid) noexcept;
    static bool controller_exists(std::int32_t pid) noexcept;
    static bool controller_is_paired(std::int32_t pid, bool* exists) noexcept;
    void kill_process(std::int32_t pid) const noexcept;
    static void kill_zombie_process(std::int32_t pid) noexcept;

    std::int32_t parent_process_id() const noexcept;
    std::int32_t parent_thread_id() const noexcept;
    std::int32_t get_width() const noexcept;
    std::int32_t get_height() const noexcept;
    std::uint8_t* get_image() const noexcept;
    std::uint8_t* get_debug_image() const noexcept;
    bool get_debug_graphics() const noexcept;
    void set_debug_graphics(bool enabled) const noexcept;
    bool get_graphics_scaling() const noexcept;
    void set_graphics_scaling(bool enabled) const noexcept;

    bool has_focus() const noexcept;
    void gain_focus() const noexcept;
    void lose_focus() const noexcept;
    bool is_keyboard_input_enabled() const noexcept;
    void set_keyboard_input_enabled(bool enabled) const noexcept;
    bool is_mouse_input_enabled() const noexcept;
    void set_mouse_input_enabled(bool enabled) const noexcept;
    void get_target_dimensions(std::int32_t* width, std::int32_t* height) const noexcept;
    void get_mouse_position(std::int32_t* x, std::int32_t* y) const noexcept;
    void get_real_mouse_position(std::int32_t* x, std::int32_t* y) const noexcept;
    void move_mouse(std::int32_t x, std::int32_t y) const noexcept;
    void hold_mouse(std::int32_t x, std::int32_t y, std::int32_t button) const noexcept;
    void release_mouse(std::int32_t x, std::int32_t y, std::int32_t button) const noexcept;
    void scroll_mouse(std::int32_t x, std::int32_t y, std::int32_t lines) const noexcept;
    bool is_mouse_held(std::int32_t button) const noexcept;
    void send_string(const char* string, std::int32_t keywait, std::int32_t keymodwait) const noexcept;
    void hold_key(std::int32_t key) const noexcept;
    void release_key(std::int32_t key) const noexcept;
    bool is_key_held(std::int32_t key) const noexcept;
    std::int32_t get_keyboard_speed() const noexcept;
    void set_keyboard_speed(std::int32_t speed) const noexcept;
    std::int32_t get_keyboard_repeat_delay() const noexcept;
    void set_keyboard_repeat_delay(std::int32_t delay) const noexcept;
    void stop_all_processing() const noexcept;

    bool reflect_is_objects_equal(const jobject first, const jobject second) const noexcept;
    bool reflect_instance_of(const jobject object, std::string cls) const noexcept;
    jobject reflect_object(const ReflectionHook &hook) const noexcept;
    void reflect_release_object(const jobject object) const noexcept;
    void reflect_release_objects(const jobject* array, std::size_t length) const noexcept;
    char reflect_char(const ReflectionHook &hook) const noexcept;
    std::uint8_t reflect_byte(const ReflectionHook &hook) const noexcept;
    bool reflect_boolean(const ReflectionHook &hook) const noexcept;
    std::int16_t reflect_short(const ReflectionHook &hook) const noexcept;
    std::int32_t reflect_int(const ReflectionHook &hook) const noexcept;
    std::int64_t reflect_long(const ReflectionHook &hook) const noexcept;
    float reflect_float(const ReflectionHook &hook) const noexcept;
    double reflect_double(const ReflectionHook &hook) const noexcept;
    std::string reflect_string(const ReflectionHook &hook) const noexcept;

    jarray reflect_array(const ReflectionHook &hook) const noexcept;
    jarray reflect_array(const ReflectionHook &hook, std::size_t* output_size) const noexcept;
    std::size_t reflect_array_size(const jarray array) const noexcept;

    template<typename... Index>
    typename std::enable_if<is_same_of<std::size_t, Index...>::value, ImageData*>::type reflect_array(const jarray array, const ReflectionType type, const std::size_t length, const Index... index) const noexcept;

    template<typename... Index>
    typename std::enable_if<is_same_of<std::size_t, Index...>::value, ImageData*>::type reflect_array(const ReflectionHook &hook, const std::size_t length, const ReflectionType type, const Index... index) const noexcept;


    ImageData* reflect_array_indices(const jarray array, ReflectionType type, std::int32_t* indices, std::size_t length) const noexcept;

    static std::size_t reflect_size_for_type(ReflectionType type) noexcept;

    java::Applet reflect_applet() const noexcept;
    java::Component reflect_canvas() const noexcept;
    std::unique_ptr<RemoteVM> create_remote_vm() noexcept;

    void get_applet_dimensions(std::int32_t* x, std::int32_t* y, std::size_t* width, std::size_t* height) const noexcept;
    void get_applet_mouse_position(std::int32_t* x, std::int32_t* y) const noexcept;
};

template<typename Func>
bool ControlCenter::send_command(Func &&sender) const noexcept
{
    ImageData* image_data = get_image_data();
    if (!image_data)
    {
        return false;
    }

    image_data->prepare_for_read();
    image_data->prepare_for_write();

    std::forward<Func>(sender)(image_data->data_stream(), image_data);
    command_signal->signal();

    //On Windows, it is much faster to sleep a thread in a loop, than to yield it.
    //Yield is equivalent to Sleep(0) and gives up a time-slice.
    //It's really SLOW in fast iteration loops.
    //
    //On other platforms, a small yield is faster than a sleep for fast iteration loops.
    //
    //However, on Windows (and other platforms too),
    //it's much faster to poll the signal instead of calling timed_wait
    //which uses WaitSingleObject
    //
    //The difference is milli-second resolution vs. nano-second and it's very noticeable.
    //IE: 16ms response time with timed_wait vs. nano-second response time with polling.
    //This is because nano-sleep is implemented with CreateWaitableTimer in WinAPI
    //and uses QUAD/LARGE_INTEGER for sleeping small amounts.
    //On other platforms, pthread_mutex_wait, sem_timed_op, etc..
    //has a minimum resolution of 5 nano-seconds

    static const std::int64_t timeout = 5000;
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - now).count() < timeout)  // elapsed_time<std::chrono::milliseconds>(now) < timeout)
    {
        if (response_signal->try_wait())
        {
            return true;
        }

        //6 is a 1ns higher than default resolution on MacOS and Linux.
        //Spikes around 10% CPU usage for thousands of calls per second.
        std::this_thread::sleep_for(std::chrono::nanoseconds(6));
        //yield_thread(std::chrono::nanoseconds(1));
    }

    return false;
}

template<typename... Args>
typename std::enable_if<is_same_of<std::size_t, Args...>::value, ImageData*>::type ControlCenter::reflect_array(const jarray array, const ReflectionType type, const std::size_t length, const Args... index) const noexcept
{
    send_command([&](Stream &stream, ImageData* image_data) {
        image_data->set_command(EIOSCommand::REFLECT_ARRAY_WITH_INDEX_SIZE);
        stream.write(array);
        stream.write(type);
        stream.write(length);
        stream.write(sizeof...(index));  // dimensions
        (stream.write(index), ...); // x, y, z, w, ...
    });

    return get_image_data();
}

template<typename... Args>
typename std::enable_if<is_same_of<std::size_t, Args...>::value, ImageData*>::type ControlCenter::reflect_array(const ReflectionHook &hook, const std::size_t length, const ReflectionType type, const Args... index) const noexcept
{
    send_command([&](Stream &stream, ImageData* image_data) {
        image_data->set_command(EIOSCommand::REFLECT_ARRAY_WITH_INDEX_SIZE);
        image_data->data_stream() << hook;
        stream.write(type);
        stream.write(length);
        stream.write(sizeof...(index));  // dimensions
        (stream.write(index), ...); // x, y, z, w, ...
    });

    return get_image_data();
}

#endif /* ControlCenter_HXX_INCLUDED */
