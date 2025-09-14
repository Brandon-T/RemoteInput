//
// Created by Brandon on 2025-05-17.
//

#ifndef REMOTEINPUT_NATIVEINPUTOUTPUT_HXX
#define REMOTEINPUT_NATIVEINPUTOUTPUT_HXX

#include <cstdint>
#include <list>
#include <atomic>
#include <mutex>
#include <array>
#include "DetachedThreadPool.hxx"
#include "ThreadPool.hxx"
#include "NativeClient.hxx"
#include "InputOutput.hxx"

class NativeInputOutput final : public InputOutput
{
private:
    NativeClient* client;
    std::mutex mutex;
    ThreadPool input_thread;

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
    char KeyCodeToChar(std::int32_t keycode, bool shifted) const noexcept;
    std::int32_t CharToKeyCode(char chr, bool &shifted) const noexcept;
    std::int32_t GetActiveKeyModifiers() const noexcept;
    std::int32_t ModifiersForChar(char c) const noexcept;
    std::int32_t GetNativeKeyCode(std::int32_t keycode) const noexcept;
    std::int32_t SimbaMouseButtonToNative(std::int32_t button) const noexcept;

    bool has_focus() const noexcept;
    void gain_focus() const noexcept;
    void lose_focus() const noexcept;

    bool any_key_held(std::array<std::int32_t, 4>&& keys) const noexcept;

public:
    NativeInputOutput(NativeClient* client) noexcept;
    ~NativeInputOutput() noexcept;

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
};

#endif //REMOTEINPUT_NATIVEINPUTOUTPUT_HXX
