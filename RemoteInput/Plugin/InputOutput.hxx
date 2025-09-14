//
// Created by Brandon on 2025-05-17.
//

#ifndef REMOTEINPUT_INPUTOUTPUT_HXX
#define REMOTEINPUT_INPUTOUTPUT_HXX

class InputOutput
{
public:
    virtual bool has_focus() const noexcept = 0;
    virtual void gain_focus() const noexcept = 0;
    virtual void lose_focus() const noexcept = 0;
    virtual bool is_keyboard_input_enabled() const noexcept = 0;
    virtual void set_keyboard_input_enabled(bool enabled) const noexcept = 0;
    virtual bool is_mouse_input_enabled() const noexcept = 0;
    virtual void set_mouse_input_enabled(bool enabled) const noexcept = 0;

    virtual std::int32_t get_keyboard_speed() const noexcept = 0;
    virtual void set_keyboard_speed(std::int32_t speed) noexcept = 0;
    virtual std::int32_t get_keyboard_repeat_delay() const noexcept = 0;
    virtual void set_keyboard_repeat_delay(std::int32_t delay) noexcept = 0;

    virtual void hold_key(std::int32_t code) noexcept = 0;
    virtual void release_key(std::int32_t code) noexcept = 0;
    virtual bool is_key_held(std::int32_t code) const noexcept = 0;
    virtual void send_string(std::string string, std::int32_t keywait, std::int32_t keymodwait) const noexcept = 0;
    virtual void send_key(char key, std::int32_t key_down_time, std::int32_t key_up_time, std::int32_t modifier_down_time, std::int32_t modifier_up_time) const noexcept = 0;
    virtual void key_send(std::string string, std::vector<std::int32_t> sleeptimes) const noexcept = 0;

    virtual void get_mouse_position(std::int32_t* x, std::int32_t* y) noexcept = 0;
    virtual void get_real_mouse_position(std::int32_t* x, std::int32_t* y) const noexcept = 0;
    virtual void move_mouse(std::int32_t x, std::int32_t y) noexcept = 0;
    virtual void hold_mouse(std::int32_t button) noexcept = 0;
    virtual void release_mouse(std::int32_t button) noexcept = 0;
    virtual void scroll_mouse(std::int32_t lines) noexcept = 0;
    virtual bool is_mouse_held(std::int32_t button) const noexcept = 0;
    virtual void stop_all_processing() noexcept = 0;
};

#endif //REMOTEINPUT_INPUTOUTPUT_HXX
