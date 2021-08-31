#ifndef ControlCenter_HXX_INCLUDED
#define ControlCenter_HXX_INCLUDED

#include <memory>
#include <atomic>
#include "Signal.hxx"
#include "Reflection.hxx"
#include "Applet.hxx"
#include "MemoryMap.hxx"
#include "InputOutput.hxx"


class RemoteVM;
struct ImageData;

enum class ReflectionArrayType: std::uint8_t
{
	CHAR,
	BYTE,
	BOOL,
	SHORT,
	INT,
	LONG,
	FLOAT,
	DOUBLE,
	STRING,
	OBJECT
};

class ControlCenter final {
private:
	using Signal = ::Signal<Semaphore>;

	pid_t pid;
	bool is_controller;
	std::atomic_bool stopped;
	std::unique_ptr<Mutex> map_lock;
	std::unique_ptr<Signal> command_signal;
	std::unique_ptr<Signal> response_signal;
	std::unique_ptr<AtomicSignal> sync_signal;
	std::unique_ptr<Reflection> reflector;
	std::unique_ptr<MemoryMap<char>> memory_map;
	std::unique_ptr<InputOutput> io_controller;
    std::unique_ptr<RemoteVM> remote_vm;

	bool init_maps() noexcept;
	bool init_locks() noexcept;
	bool init_wait() noexcept;
	bool init_signals() noexcept;
	void process_command() noexcept;

	ImageData* get_image_data() const noexcept;
	bool send_command(std::function<void(ImageData*)> &&writer) const noexcept;
    void process_reflect_array_indices(jarray array, void* &arguments, void* &response) const noexcept;
	void process_reflect_array_index(jarray array, void* &arguments, void* &response, int dimensions) const noexcept;

public:
	ControlCenter(pid_t pid, bool is_controller, std::unique_ptr<Reflection> &&reflector);
	~ControlCenter();

	void terminate() noexcept;
	bool hasReflector() const noexcept;
	void set_parent_process_id(std::int32_t pid) const noexcept;
	void set_parent_thread_id(std::int32_t tid) const noexcept;
	void update_dimensions(std::int32_t width, std::int32_t height) const noexcept;

	static void signal_sync(pid_t pid) noexcept;
	static void wait_for_sync(pid_t pid) noexcept;
	static bool controller_exists(pid_t pid) noexcept;
	static bool controller_is_paired(pid_t pid, bool* exists) noexcept;
	void kill_process(pid_t pid) const noexcept;
	static void kill_zombie_process(pid_t pid) noexcept;

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
	bool is_input_enabled() const noexcept;
	void set_input_enabled(bool enabled) const noexcept;
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
	jarray reflect_array_with_size(const ReflectionHook &hook, std::size_t* output_size) const noexcept;
	std::size_t reflect_array_size(const jarray array) const noexcept;
	std::size_t reflect_array_size(const jarray array, std::size_t index) const noexcept;
	void* reflect_array_index(const jarray array, ReflectionArrayType type, std::size_t index, std::size_t length) const noexcept;
	void* reflect_array_index2d(const jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y) const noexcept;
	void* reflect_array_index3d(const jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z) const noexcept;
	void* reflect_array_index4d(const jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t w) const noexcept;
    void* reflect_array_indices(const jarray array, ReflectionArrayType type, std::int32_t* indices, std::size_t length) const noexcept;

	static std::size_t reflect_size_for_type(ReflectionArrayType type) noexcept;

	Applet reflect_applet() const noexcept;
	Component reflect_canvas() const noexcept;
	std::unique_ptr<RemoteVM> create_remote_vm() noexcept;

	void get_applet_dimensions(std::int32_t* x, std::int32_t* y, std::size_t* width, std::size_t* height) const noexcept;
	void get_applet_mouse_position(std::int32_t* x, std::int32_t* y) const noexcept;
};

#endif /* ControlCenter_HXX_INCLUDED */
