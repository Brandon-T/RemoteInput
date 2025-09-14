//
//  JavaInputOutput.hxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-28.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef INPUTOUTPUT_HXX_INCLUDED
#define INPUTOUTPUT_HXX_INCLUDED

#include <cstdint>
#include <list>
#include <atomic>
#include <mutex>
#include <array>
#include "DetachedThreadPool.hxx"
#include "ThreadPool.hxx"
#include "Reflection.hxx"
#include "Component.hxx"
#include "RIEventQueue.hxx"
#include "JVM.hxx"
#include "InputOutput.hxx"

class JavaInputOutput final : public InputOutput
{
private:
    JavaVM* vm;
    jobject applet;
    std::mutex mutex;
    ThreadPool input_thread;
    std::unique_ptr<java::RIEventQueue> event_queue;

    // MARK: - Input Variables
    std::atomic<std::int32_t> currently_held_key;
    std::list<std::int32_t> held_keys;
    std::int32_t x;
    std::int32_t y;
    std::size_t w;
    std::size_t h;
    std::int32_t click_count;
    std::size_t keyboard_speed;
    std::size_t keyboard_repeat_delay;
    bool mouse_buttons[3];

    // MARK: - Functions
    std::int32_t CharToJavaKeyCode(char c) const noexcept;
    jchar NativeKeyCodeToChar(std::int32_t keycode, std::int32_t modifiers) const noexcept;
    std::int32_t GetJavaKeyCode(std::int32_t native_key_code) const noexcept;
    std::int32_t GetKeyLocation(std::int32_t keycode) const noexcept;
    std::int32_t GetActiveKeyModifiers() const noexcept;
    std::int32_t ModifiersForChar(char c) const noexcept;
    std::int32_t SimbaMouseButtonToJava(std::int32_t button) const noexcept;

    bool has_focus(java::Component* component) const noexcept;
    void gain_focus(java::Component* component) const noexcept;
    void lose_focus(java::Component* component) const noexcept;

    bool any_key_held(std::array<std::int32_t, 4>&& keys) const noexcept;
    void handle_resize(java::Component* component) noexcept;

public:
    explicit JavaInputOutput(Reflection* reflection) noexcept;
    ~JavaInputOutput() noexcept;

    bool has_focus() const noexcept override;
    void gain_focus() const noexcept override;
    void lose_focus() const noexcept override;
    bool is_keyboard_input_enabled() const noexcept override;
    void set_keyboard_input_enabled(bool enabled) const noexcept override;
    bool is_mouse_input_enabled() const noexcept override;
    void set_mouse_input_enabled(bool enabled) const noexcept override;

    std::int32_t get_keyboard_speed() const noexcept override;
    void set_keyboard_speed(std::int32_t speed) noexcept override;
    std::int32_t get_keyboard_repeat_delay() const noexcept override;
    void set_keyboard_repeat_delay(std::int32_t delay) noexcept override;

    void hold_key(std::int32_t code) noexcept override;
    void release_key(std::int32_t code) noexcept override;
    bool is_key_held(std::int32_t code) const noexcept override;
    void send_string(std::string string, std::int32_t keywait, std::int32_t keymodwait) const noexcept override;
    void send_key(char key, std::int32_t key_down_time, std::int32_t key_up_time, std::int32_t modifier_down_time, std::int32_t modifier_up_time) const noexcept override;
    void key_send(std::string string, std::vector<std::int32_t> sleeptimes) const noexcept override;
    
    void get_mouse_position(std::int32_t* x, std::int32_t* y) noexcept override;
    void get_real_mouse_position(std::int32_t* x, std::int32_t* y) const noexcept override;
    void move_mouse(std::int32_t x, std::int32_t y) noexcept override;
    void hold_mouse(std::int32_t button) noexcept override;
    void release_mouse(std::int32_t button) noexcept override;
    void scroll_mouse(std::int32_t lines) noexcept override;
    bool is_mouse_held(std::int32_t button) const noexcept override;
    void stop_all_processing() noexcept override;

private:
    friend class ControlCenter;
    void get_applet_dimensions(std::int32_t &x, std::int32_t &y, std::size_t &width, std::size_t &height) const noexcept;
    void get_applet_mouse_position(std::int32_t &x, std::int32_t &y) const noexcept;
};

#endif /* INPUTOUTPUT_HXX_INCLUDED */
