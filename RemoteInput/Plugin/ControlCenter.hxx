#ifndef ControlCenter_HXX_INCLUDED
#define ControlCenter_HXX_INCLUDED

#include <atomic>
#include "SharedEvent.hxx"
#include "Reflection.hxx"
#include "MemoryMap.hxx"


struct ImageData;

class ControlCenter final {
private:
	pid_t pid;
	bool is_controller;
	std::atomic_bool stopped;
	std::unique_ptr<Mutex> map_lock;
	std::unique_ptr<Semaphore> command_signal;
	std::unique_ptr<Semaphore> response_signal;
	std::unique_ptr<Reflection> reflector;
	std::unique_ptr<MemoryMap<char>> memory_map;

	bool init_maps();
	bool init_locks();
	bool init_signals();
	void process_command();

	ImageData* get_image_data() const;

public:
	ControlCenter(pid_t pid, bool is_controller, std::unique_ptr<Reflection> &&reflector);
	~ControlCenter();

	void terminate();
	bool hasReflector();
	void set_parent(pid_t pid);

	std::int32_t parent_id() const;
	std::int32_t get_width() const;
	std::int32_t get_height() const;
	std::uint8_t* get_image() const;

	void get_target_dimensions(std::int32_t* width, std::int32_t* height);
	void get_mouse_position(std::int32_t* x, std::int32_t* y);
	void move_mouse(std::int32_t x, std::int32_t y);
	void hold_mouse(std::int32_t x, std::int32_t y, std::int32_t button);
	void release_mouse(std::int32_t x, std::int32_t y, std::int32_t button);

	int mouse_x = 100;
	int mouse_y = 100;
	int w = 0;
	int h = 0;

	void update(int x, int y)
	{
	    w = x;
	    h = y;
	}
};

#endif /* ControlCenter_HXX_INCLUDED */
