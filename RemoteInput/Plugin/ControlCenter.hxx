#ifndef ControlCenter_HXX_INCLUDED
#define ControlCenter_HXX_INCLUDED

#include <memory>
#include <atomic>
#include "Signal.hxx"
#include "Reflection.hxx"
#include "Applet.hxx"
#include "MemoryMap.hxx"
#include "InputOutput.hxx"


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
	using Signal = Signal<Semaphore>;

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

	bool init_maps();
	bool init_locks();
	bool init_wait();
	bool init_signals();
	void process_command();

	ImageData* get_image_data() const;
	bool send_command(std::function<void(ImageData*)> &&writer);
	void process_reflect_array_index(jarray array, void* &arguments, void* &response, int dimensions);

public:
	ControlCenter(pid_t pid, bool is_controller, std::unique_ptr<Reflection> &&reflector);
	~ControlCenter();

	void terminate();
	bool hasReflector();
	void set_parent_process_id(std::int32_t pid);
	void set_parent_thread_id(std::int32_t tid);
	void update_dimensions(std::int32_t width, std::int32_t height);

	static void wait_for_sync(pid_t pid);
	static bool controller_exists(pid_t pid);
	static bool controller_is_paired(pid_t pid, bool* exists);
	void kill_process(pid_t pid);
	static void kill_zombie_process(pid_t pid);

	std::int32_t parent_process_id() const;
	std::int32_t parent_thread_id() const;
	std::int32_t get_width() const;
	std::int32_t get_height() const;
	std::uint8_t* get_image() const;
	std::uint8_t* get_debug_image() const;
	bool get_debug_graphics() const;
	void set_debug_graphics(bool enabled);

	bool has_focus();
	void gain_focus();
	void lose_focus();
	bool is_input_enabled();
	void set_input_enabled(bool enabled);
	void get_target_dimensions(std::int32_t* width, std::int32_t* height);
	void get_mouse_position(std::int32_t* x, std::int32_t* y);
	void get_real_mouse_position(std::int32_t* x, std::int32_t* y);
	void move_mouse(std::int32_t x, std::int32_t y);
	void hold_mouse(std::int32_t x, std::int32_t y, std::int32_t button);
	void release_mouse(std::int32_t x, std::int32_t y, std::int32_t button);
	void scroll_mouse(std::int32_t x, std::int32_t y, std::int32_t lines);
	bool is_mouse_held(std::int32_t button);
	void send_string(const char* string, std::int32_t keywait, std::int32_t keymodwait);
	void hold_key(std::int32_t key);
	void release_key(std::int32_t key);
	bool is_key_held(std::int32_t key);

	bool reflect_is_objects_equal(const jobject first, const jobject second);
	bool reflect_instance_of(const jobject object, std::string cls);
	jobject reflect_object(const ReflectionHook &hook);
	void reflect_release_object(const jobject object);
	void reflect_release_objects(const jobject* array, std::size_t length);
	char reflect_char(const ReflectionHook &hook);
	std::uint8_t reflect_byte(const ReflectionHook &hook);
	bool reflect_boolean(const ReflectionHook &hook);
	std::int16_t reflect_short(const ReflectionHook &hook);
	std::int32_t reflect_int(const ReflectionHook &hook);
	std::int64_t reflect_long(const ReflectionHook &hook);
	float reflect_float(const ReflectionHook &hook);
	double reflect_double(const ReflectionHook &hook);
	std::string reflect_string(const ReflectionHook &hook);
	jarray reflect_array(const ReflectionHook &hook);
	jarray reflect_array_with_size(const ReflectionHook &hook, std::size_t* output_size);
	std::size_t reflect_array_size(const jarray array);
	std::size_t reflect_array_size(const jarray array, std::size_t index);
	void* reflect_array_index(const jarray array, ReflectionArrayType type, std::size_t index, std::size_t length);
	void* reflect_array_index2d(const jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y);
	void* reflect_array_index3d(const jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z);
	void* reflect_array_index4d(const jarray array, ReflectionArrayType type, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t w);

	static std::size_t reflect_size_for_type(ReflectionArrayType type);

	Applet reflect_applet();
	Component reflect_canvas();

	void get_applet_dimensions(std::int32_t* x, std::int32_t* y, std::int32_t* width, std::int32_t* height);
	void get_applet_mouse_position(std::int32_t* x, std::int32_t* y);
};

#endif /* ControlCenter_HXX_INCLUDED */
