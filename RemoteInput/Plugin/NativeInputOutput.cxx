//
// Created by Brandon on 2025-05-17.
//

#include "NativeInputOutput.hxx"
#include "ControlCenter.hxx"
#include "Platform.hxx"
#include "Random.hxx"

#if defined(_WIN32) || defined(_WIN64)
#define VK_UNKNOWN 0x00
#define VK_LBUTTON 0x01
#define VK_RBUTTON 0x02
#define VK_CANCEL 0x03
#define VK_MBUTTON 0x04
#define VK_XBUTTON1 0x05
#define VK_XBUTTON2 0x06
#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_CLEAR 0x0C
#define VK_RETURN 0x0D
#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_MENU 0x12
#define VK_PAUSE 0x13
#define VK_CAPITAL 0x14
#define VK_KANA 0x15
#define VK_HANGUL 0x15
#define VK_JUNJA 0x17
#define VK_FINAL 0x18
#define VK_HANJA 0x19
#define VK_KANJI 0x19
#define VK_ESCAPE 0x1B
#define VK_CONVERT 0x1C
#define VK_NONCONVERT 0x1D
#define VK_ACCEPT 0x1E
#define VK_MODECHANGE 0x1F
#define VK_SPACE 0x20
#define VK_PRIOR 0x21
#define VK_NEXT 0x22
#define VK_END 0x23
#define VK_HOME 0x24
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_SELECT 0x29
#define VK_PRINT 0x2A
#define VK_EXECUTE 0x2B
#define VK_SNAPSHOT 0x2C
#define VK_INSERT 0x2D
#define VK_DELETE 0x2E
#define VK_HELP 0x2F
#define VK_0 0x30
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39
#define VK_A 0x41
#define VK_B 0x42
#define VK_C 0x43
#define VK_D 0x44
#define VK_E 0x45
#define VK_F 0x46
#define VK_G 0x47
#define VK_H 0x48
#define VK_I 0x49
#define VK_J 0x4A
#define VK_K 0x4B
#define VK_L 0x4C
#define VK_M 0x4D
#define VK_N 0x4E
#define VK_O 0x4F
#define VK_P 0x50
#define VK_Q 0x51
#define VK_R 0x52
#define VK_S 0x53
#define VK_T 0x54
#define VK_U 0x55
#define VK_V 0x56
#define VK_W 0x57
#define VK_X 0x58
#define VK_Y 0x59
#define VK_Z 0x5A
#define VK_LWIN 0x5B
#define VK_RWIN 0x5C
#define VK_APPS 0x5D
#define VK_SLEEP 0x5F
#define VK_NUMPAD0 0x60
#define VK_NUMPAD1 0x61
#define VK_NUMPAD2 0x62
#define VK_NUMPAD3 0x63
#define VK_NUMPAD4 0x64
#define VK_NUMPAD5 0x65
#define VK_NUMPAD6 0x66
#define VK_NUMPAD7 0x67
#define VK_NUMPAD8 0x68
#define VK_NUMPAD9 0x69
#define VK_MULTIPLY 0x6A
#define VK_ADD 0x6B
#define VK_SEPARATOR 0x6C
#define VK_SUBTRACT 0x6D
#define VK_DECIMAL 0x6E
#define VK_DIVIDE 0x6F
#define VK_F1 0x70
#define VK_F2 0x71
#define VK_F3 0x72
#define VK_F4 0x73
#define VK_F5 0x74
#define VK_F6 0x75
#define VK_F7 0x76
#define VK_F8 0x77
#define VK_F9 0x78
#define VK_F10 0x79
#define VK_F11 0x7A
#define VK_F12 0x7B
#define VK_F13 0x7C
#define VK_F14 0x7D
#define VK_F15 0x7E
#define VK_F16 0x7F
#define VK_F17 0x80
#define VK_F18 0x81
#define VK_F19 0x82
#define VK_F20 0x83
#define VK_F21 0x84
#define VK_F22 0x85
#define VK_F23 0x86
#define VK_F24 0x87
#define VK_NUMLOCK 0x90
#define VK_SCROLL 0x91
#define VK_LSHIFT 0xA0
#define VK_RSHIFT 0xA1
#define VK_LCONTROL 0xA2
#define VK_RCONTROL 0xA3
#define VK_LMENU 0xA4
#define VK_RMENU 0xA5
#define VK_BROWSER_BACK 0xA6
#define VK_BROWSER_FORWARD 0xA7
#define VK_BROWSER_REFRESH 0xA8
#define VK_BROWSER_STOP 0xA9
#define VK_BROWSER_SEARCH 0xAA
#define VK_BROWSER_FAVORITES 0xAB
#define VK_BROWSER_HOME 0xAC
#define VK_VOLUME_MUTE 0xAD
#define VK_VOLUME_DOWN 0xAE
#define VK_VOLUME_UP 0xAF
#define VK_MEDIA_NEXT_TRACK 0xB0
#define VK_MEDIA_PREV_TRACK 0xB1
#define VK_MEDIA_STOP 0xB2
#define VK_MEDIA_PLAY_PAUSE 0xB3
#define VK_LAUNCH_MAIL 0xB4
#define VK_LAUNCH_MEDIA_SELECT 0xB5
#define VK_LAUNCH_APP1 0xB6
#define VK_LAUNCH_APP2 0xB7
#define VK_OEM_1 0xBA
#define VK_OEM_PLUS 0xBB
#define VK_OEM_COMMA 0xBC
#define VK_OEM_MINUS 0xBD
#define VK_OEM_PERIOD 0xBE
#define VK_OEM_2 0xBF
#define VK_OEM_3 0xC0
#define VK_OEM_4 0xDB
#define VK_OEM_5 0xDC
#define VK_OEM_6 0xDD
#define VK_OEM_7 0xDE
#define VK_OEM_8 0xDF
#define VK_OEM_102 0xE2
#define VK_PROCESSKEY 0xE7
#define VK_ATTN 0xF6
#define VK_CRSEL 0xF7
#define VK_EXSEL 0xF8
#define VK_EREOF 0xF9
#define VK_PLAY 0xFA
#define VK_ZOOM 0xFB
#define VK_NONAME 0xFC
#define VK_PA1 0xFD
#define VK_OEM_CLEAR 0xFE
#define VK_HIGHESTVALUE 0xFE
#define VK_UNDEFINED 0xFF
#elif defined(__APPLE__)
#define VK_UNKNOWN 0x00
#define VK_LBUTTON 0x00
#define VK_RBUTTON 0x00
#define VK_CANCEL 0x00
#define VK_MBUTTON 0x00
#define VK_XBUTTON1 0x00
#define VK_XBUTTON2 0x00
#define VK_BACK 0x33
#define VK_TAB 0x30
#define VK_CLEAR 0x47
#define VK_RETURN 0x24
#define VK_SHIFT 0x38
#define VK_CONTROL 0x3B
#define VK_MENU 0x3A
#define VK_PAUSE 0x00   //???
#define VK_CAPITAL 0x39
#define VK_KANA 0x00
#define VK_HANGUL 0x00
#define VK_JUNJA 0x00
#define VK_FINAL 0x00
#define VK_HANJA 0x00
#define VK_KANJI 0x00
#define VK_ESCAPE 0x35
#define VK_CONVERT 0x00
#define VK_NONCONVERT 0x00
#define VK_ACCEPT 0x00
#define VK_MODECHANGE 0x00
#define VK_SPACE 0x31
#define VK_PRIOR 0x00
#define VK_NEXT 0x00
#define VK_END 0x00
#define VK_HOME 0x00
#define VK_LEFT 0x7B
#define VK_UP 0x7E
#define VK_RIGHT 0x7C
#define VK_DOWN 0x7D
#define VK_SELECT 0x00
#define VK_PRINT 0x00
#define VK_EXECUTE 0x00
#define VK_SNAPSHOT 0x00
#define VK_INSERT 0x00
#define VK_DELETE 0x33
#define VK_HELP 0x00
#define VK_0 0x1D
#define VK_1 0x12
#define VK_2 0x13
#define VK_3 0x14
#define VK_4 0x15
#define VK_5 0x17
#define VK_6 0x16
#define VK_7 0x1A
#define VK_8 0x1C
#define VK_9 0x19
#define VK_A 0x00
#define VK_B 0x0B
#define VK_C 0x08
#define VK_D 0x02
#define VK_E 0x0E
#define VK_F 0x03
#define VK_G 0x05
#define VK_H 0x04
#define VK_I 0x22
#define VK_J 0x26
#define VK_K 0x28
#define VK_L 0x25
#define VK_M 0x2E
#define VK_N 0x2D
#define VK_O 0x1F
#define VK_P 0x23
#define VK_Q 0x0C
#define VK_R 0x0F
#define VK_S 0x01
#define VK_T 0x11
#define VK_U 0x20
#define VK_V 0x09
#define VK_W 0x0D
#define VK_X 0x07
#define VK_Y 0x10
#define VK_Z 0x06
#define VK_LWIN 0x37
#define VK_RWIN 0x36
#define VK_APPS 0x00
#define VK_SLEEP 0x00
#define VK_NUMPAD0 0x52
#define VK_NUMPAD1 0x53
#define VK_NUMPAD2 0x54
#define VK_NUMPAD3 0x55
#define VK_NUMPAD4 0x56
#define VK_NUMPAD5 0x57
#define VK_NUMPAD6 0x58
#define VK_NUMPAD7 0x59
#define VK_NUMPAD8 0x5B
#define VK_NUMPAD9 0x5C
#define VK_MULTIPLY 0x43
#define VK_ADD 0x45
#define VK_SEPARATOR 0x00
#define VK_SUBTRACT 0x4E
#define VK_DECIMAL 0x41
#define VK_DIVIDE 0x4B
#define VK_F1 0x7A
#define VK_F2 0x78
#define VK_F3 0x63
#define VK_F4 0x76
#define VK_F5 0x60
#define VK_F6 0x61
#define VK_F7 0x62
#define VK_F8 0x64
#define VK_F9 0x65
#define VK_F10 0x6D
#define VK_F11 0x67
#define VK_F12 0x6F
#define VK_F13 0x69
#define VK_F14 0x6B
#define VK_F15 0x71
#define VK_F16 0x6A
#define VK_F17 0x40
#define VK_F18 0x4F
#define VK_F19 0x50
#define VK_F20 0x5A
#define VK_F21 0x00
#define VK_F22 0x00
#define VK_F23 0x00
#define VK_F24 0x00
#define VK_NUMLOCK 0x47
#define VK_SCROLL 0x00
#define VK_LSHIFT 0x38
#define VK_RSHIFT 0x3C
#define VK_LCONTROL 0x3B
#define VK_RCONTROL 0x3E
#define VK_LMENU 0x3A
#define VK_RMENU 0x3D
#define VK_BROWSER_BACK 0x00
#define VK_BROWSER_FORWARD 0x00
#define VK_BROWSER_REFRESH 0x00
#define VK_BROWSER_STOP 0x00
#define VK_BROWSER_SEARCH 0x00
#define VK_BROWSER_FAVORITES 0x00
#define VK_BROWSER_HOME 0x00
#define VK_VOLUME_MUTE 0x4A
#define VK_VOLUME_DOWN 0x49
#define VK_VOLUME_UP 0x48
#define VK_MEDIA_NEXT_TRACK 0xB5
#define VK_MEDIA_PREV_TRACK 0xB6
#define VK_MEDIA_STOP 0x00
#define VK_MEDIA_PLAY_PAUSE 0xB0
#define VK_LAUNCH_MAIL 0x00
#define VK_LAUNCH_MEDIA_SELECT 0x00
#define VK_LAUNCH_APP1 0x00
#define VK_LAUNCH_APP2 0x00
#define VK_OEM_1 0x00
#define VK_OEM_PLUS 0x00
#define VK_OEM_COMMA 0x00
#define VK_OEM_MINUS 0x00
#define VK_OEM_PERIOD 0x00
#define VK_OEM_2 0x00
#define VK_OEM_3 0x00
#define VK_OEM_4 0x00
#define VK_OEM_5 0x00
#define VK_OEM_6 0x00
#define VK_OEM_7 0x00
#define VK_OEM_8 0x00
#define VK_OEM_102 0x00
#define VK_PROCESSKEY 0x00
#define VK_ATTN 0x00
#define VK_CRSEL 0x00
#define VK_EXSEL 0x00
#define VK_EREOF 0x00
#define VK_PLAY 0x00
#define VK_ZOOM 0x00
#define VK_NONAME 0x00
#define VK_PA1 0x00
#define VK_OEM_CLEAR 0x00
#define VK_HIGHESTVALUE 0xFF
#define VK_UNDEFINED 0xFF
#elif defined(__linux__)
#define VK_UNKNOWN 0x00
#define VK_LBUTTON 0x00
#define VK_RBUTTON 0x00
#define VK_CANCEL 0x00
#define VK_MBUTTON 0x00
#define VK_XBUTTON1 0x00
#define VK_XBUTTON2 0x00
#define VK_BACK 0x00
#define VK_TAB 0xFF09
#define VK_CLEAR 0x00
#define VK_RETURN 0xFF0D
#define VK_SHIFT 0xFFE1
#define VK_CONTROL 0xFFE3
#define VK_MENU 0x00
#define VK_PAUSE 0x00
#define VK_CAPITAL 0xFFE5
#define VK_KANA 0x00
#define VK_HANGUL 0x00
#define VK_JUNJA 0x00
#define VK_FINAL 0x00
#define VK_HANJA 0x00
#define VK_KANJI 0x00
#define VK_ESCAPE 0xFF1B
#define VK_CONVERT 0x00
#define VK_NONCONVERT 0x00
#define VK_ACCEPT 0x00
#define VK_MODECHANGE 0x00
#define VK_SPACE 0x0020
#define VK_PRIOR 0x00
#define VK_NEXT 0x00
#define VK_END 0x00
#define VK_HOME 0x00
#define VK_LEFT 0xFF51
#define VK_UP 0xFF52
#define VK_RIGHT 0xFF53
#define VK_DOWN 0xFF54
#define VK_SELECT 0x00
#define VK_PRINT 0x00
#define VK_EXECUTE 0x00
#define VK_SNAPSHOT 0x00
#define VK_INSERT 0x00
#define VK_DELETE 0xFFFF
#define VK_HELP 0x00
#define VK_0 0x0030
#define VK_1 0x0031
#define VK_2 0x0032
#define VK_3 0x0033
#define VK_4 0x0034
#define VK_5 0x0035
#define VK_6 0x0036
#define VK_7 0x0037
#define VK_8 0x0038
#define VK_9 0x0039
#define VK_A 0x0041
#define VK_B 0x0042
#define VK_C 0x0043
#define VK_D 0x0044
#define VK_E 0x0045
#define VK_F 0x0046
#define VK_G 0x0047
#define VK_H 0x0048
#define VK_I 0x0049
#define VK_J 0x004A
#define VK_K 0x004B
#define VK_L 0x004C
#define VK_M 0x004D
#define VK_N 0x004E
#define VK_O 0x004F
#define VK_P 0x0050
#define VK_Q 0x0051
#define VK_R 0x0052
#define VK_S 0x0053
#define VK_T 0x0054
#define VK_U 0x0055
#define VK_V 0x0056
#define VK_W 0x0057
#define VK_X 0x0058
#define VK_Y 0x0059
#define VK_Z 0x005A
#define VK_LWIN 0x00
#define VK_RWIN 0x00
#define VK_APPS 0x00
#define VK_SLEEP 0x00
#define VK_NUMPAD0 0x00
#define VK_NUMPAD1 0x00
#define VK_NUMPAD2 0x00
#define VK_NUMPAD3 0x00
#define VK_NUMPAD4 0x00
#define VK_NUMPAD5 0x00
#define VK_NUMPAD6 0x00
#define VK_NUMPAD7 0x00
#define VK_NUMPAD8 0x00
#define VK_NUMPAD9 0x00
#define VK_MULTIPLY 0x00
#define VK_ADD 0x00
#define VK_SEPARATOR 0x00
#define VK_SUBTRACT 0x00
#define VK_DECIMAL 0x00
#define VK_DIVIDE 0x00
#define VK_F1 0x00
#define VK_F2 0x00
#define VK_F3 0x00
#define VK_F4 0x00
#define VK_F5 0x00
#define VK_F6 0x00
#define VK_F7 0x00
#define VK_F8 0x00
#define VK_F9 0x00
#define VK_F10 0x00
#define VK_F11 0x00
#define VK_F12 0x00
#define VK_F13 0x00
#define VK_F14 0x00
#define VK_F15 0x00
#define VK_F16 0x00
#define VK_F17 0x00
#define VK_F18 0x00
#define VK_F19 0x00
#define VK_F20 0x00
#define VK_F21 0x00
#define VK_F22 0x00
#define VK_F23 0x00
#define VK_F24 0x00
#define VK_NUMLOCK 0x00
#define VK_SCROLL 0x00
#define VK_LSHIFT 0xFFE1
#define VK_RSHIFT 0xFFE2
#define VK_LCONTROL 0xFFE3
#define VK_RCONTROL 0xFFE4
#define VK_LMENU 0x00
#define VK_RMENU 0x00
#define VK_BROWSER_BACK 0x00
#define VK_BROWSER_FORWARD 0x00
#define VK_BROWSER_REFRESH 0x00
#define VK_BROWSER_STOP 0x00
#define VK_BROWSER_SEARCH 0x00
#define VK_BROWSER_FAVORITES 0x00
#define VK_BROWSER_HOME 0x00
#define VK_VOLUME_MUTE 0x00
#define VK_VOLUME_DOWN 0x00
#define VK_VOLUME_UP 0x00
#define VK_MEDIA_NEXT_TRACK 0x00
#define VK_MEDIA_PREV_TRACK 0x00
#define VK_MEDIA_STOP 0x00
#define VK_MEDIA_PLAY_PAUSE 0x00
#define VK_LAUNCH_MAIL 0x00
#define VK_LAUNCH_MEDIA_SELECT 0x00
#define VK_LAUNCH_APP1 0x00
#define VK_LAUNCH_APP2 0x00
#define VK_OEM_1 0x00
#define VK_OEM_PLUS 0x00
#define VK_OEM_COMMA 0x00
#define VK_OEM_MINUS 0x00
#define VK_OEM_PERIOD 0x00
#define VK_OEM_2 0x00
#define VK_OEM_3 0x00
#define VK_OEM_4 0x00
#define VK_OEM_5 0x00
#define VK_OEM_6 0x00
#define VK_OEM_7 0x00
#define VK_OEM_8 0x00
#define VK_OEM_102 0x00
#define VK_PROCESSKEY 0x00
#define VK_ATTN 0x00
#define VK_CRSEL 0x00
#define VK_EXSEL 0x00
#define VK_EREOF 0x00
#define VK_PLAY 0x00
#define VK_ZOOM 0x00
#define VK_NONAME 0x00
#define VK_PA1 0x00
#define VK_OEM_CLEAR 0x00
#define VK_HIGHESTVALUE 0x00
#define VK_UNDEFINED 0x00
#else
#error "Unknown Platform"
#endif

enum MouseButton : std::uint32_t
{
    LEFT = 0,
    RIGHT = 1,
    MIDDLE = 2
};

enum KeyButton : std::uint32_t
{
    KEY_PRESSED,
    KEY_RELEASED
};

enum ControlKeys : std::uint32_t
{
    FUNCTION = 0x00,
    NUM_LOCK = VK_NUMLOCK,
    SHIFT = VK_SHIFT,
    LSHIFT = VK_LSHIFT,
    RSHIFT = VK_RSHIFT,
    CONTROL = VK_CONTROL,
    LEFT_CONTROL = VK_LCONTROL,
    RIGHT_CONTROL = VK_RCONTROL,
    LEFT_ALT = VK_LMENU,
    RIGHT_ALT = VK_RMENU,

    COMMAND = 0x9D, //VK_META
    LEFT_WINDOWS = VK_LWIN,
    RIGHT_WINDOWS = VK_RWIN,
    APPS = VK_APPS,
    TAB = VK_TAB,
    BACK = VK_BACK,
    RETURN = VK_RETURN,

    SHIFT_DOWN_MASK = 0x0040,
    META_DOWN_MASK = 0x0100,
    ALT_GRAPH_DOWN_MASK = 0x0020,
    ALT_DOWN_MASK = 0x0200,
    CTRL_DOWN_MASK = 0x0080
};

static ControlKeys control_keys[] = {
    ControlKeys::FUNCTION,
    ControlKeys::NUM_LOCK,
    ControlKeys::LSHIFT,
    ControlKeys::RSHIFT,
    ControlKeys::LEFT_CONTROL,
    ControlKeys::RIGHT_CONTROL,
    ControlKeys::LEFT_ALT,
    ControlKeys::RIGHT_ALT,
    ControlKeys::COMMAND,
    ControlKeys::LEFT_WINDOWS,
    ControlKeys::RIGHT_WINDOWS,
    ControlKeys::APPS,
    ControlKeys::TAB,
    ControlKeys::BACK,
    ControlKeys::RETURN
};

NativeInputOutput::NativeInputOutput(NativeClient* client) noexcept : client(client), mutex(), input_thread(2), currently_held_key(-1), held_keys(), x(-1), y(-1), w(-1), h(-1), click_count(0), keyboard_speed(0), keyboard_repeat_delay(0), mouse_buttons()
{
    x = std::numeric_limits<std::int32_t>::min();
    y = std::numeric_limits<std::int32_t>::min();

    auto scale_to_range = [](float value, float fromMin, float fromMax, float toMin, float toMax) -> float {
        return (((toMax - toMin) * (value - fromMin)) / (fromMax - fromMin)) + toMin;
    };

    #if defined(_WIN32) || defined(_WIN64)
        #if defined(WINDOWS_SYSTEM_INPUT_INFO)
        //0 (approximately 250 ms delay) through 3 (approximately 1 second delay).
            std::int32_t keyboard_delay = 0;
            SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, reinterpret_cast<void*>(&keyboard_delay), 0);

            //Small variance in delay due to electrical signal, lag, hardware, etc..
            std::int32_t min_delay = Random::instance()->generate_random_int(248, 252);
            std::int32_t max_delay = Random::instance()->generate_random_int(998, 1002);
            this->keyboard_repeat_delay = scale_to_range(keyboard_delay, 0, 3, min_delay, max_delay);

            //0 (approximately 2.5 repetitions per second) through 31 (approximately 30 repetitions per second).
            //The actual repeat rates are hardware-dependent and may vary from a linear scale by as much as 20%
            std::int32_t keyboard_repeat_speed = 0;
            SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, reinterpret_cast<void*>(&keyboard_repeat_speed), 0);

            float repeat_rate = scale_to_range(keyboard_repeat_speed, 0, 31, 2.5, 30);
            repeat_rate += repeat_rate * Random::instance()->generate_random_float(0.0, 0.20);
            this->keyboard_speed = round(60.0 / repeat_rate);
        #else
        //Windows default is 250ms delay and 24ms repeat rate
        this->keyboard_repeat_delay = scale_to_range(0, 0, 3, 250, 1000) + Random::instance()->generate_random_int(-2, 2);

        float repeat_rate = scale_to_range(0, 0, 31, 2.5, 30);
        repeat_rate += repeat_rate * Random::instance()->generate_random_float(0.0, 0.20);
        this->keyboard_speed = round(60.0 / repeat_rate);
        #endif
    #elif defined(__APPLE__)
        //MacOS default is 225ms delay and 15ms repeat rate
        this->keyboard_repeat_delay = scale_to_range(15, 15, 120, 225, 1800) + Random::instance()->generate_random_int(-2, 2);

        float repeat_rate = scale_to_range(1, 2, 120, 30, 1800);
        repeat_rate += repeat_rate * Random::instance()->generate_random_float(0.0, 0.20);
        this->keyboard_speed = round(repeat_rate);
    #else
        //Linux default is 660ms delay and 25ms repeat rate
        this->keyboard_repeat_delay = scale_to_range(44, 15, 120, 225, 1800) + Random::instance()->generate_random_int(-2, 2);

        float repeat_rate = scale_to_range(2, 2, 120, 25, 1800);
        repeat_rate += repeat_rate * Random::instance()->generate_random_float(0.0, 0.20);
        this->keyboard_speed = round(repeat_rate);
    #endif
}

NativeInputOutput::~NativeInputOutput() noexcept
{
    this->input_thread.terminate();
}

void NativeInputOutput::hold_key(std::int32_t code) noexcept
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (!control_center)
    {
        return;
    }

    //Key is not already held..
    if (!this->is_key_held(code))
    {
        //Key is a control key
        if (std::find(std::begin(control_keys), std::end(control_keys), code) != std::end(control_keys))
        {
            //Control Keys only generate a single held event..
            held_keys.push_back(code);

            //PostEvent
            if (!this->has_focus())
            {
                this->gain_focus();
            }

            std::int32_t modifiers = GetActiveKeyModifiers();
            std::int32_t keycode = GetNativeKeyCode(code);
            this->PostEvent(KeyButton::KEY_PRESSED, keycode, modifiers);
        }
        else
        {
            //Key already being pressed so just replace it
            if (currently_held_key != -1 && this->keyboard_speed >= 0 && this->keyboard_repeat_delay >= 0)
            {
                currently_held_key = code;
                held_keys.push_back(code);

                //Post Event
                if (!this->has_focus())
                {
                    this->gain_focus();
                }

                std::int32_t modifiers = GetActiveKeyModifiers();
                std::int32_t keycode = GetNativeKeyCode(code);
                this->PostEvent(KeyButton::KEY_PRESSED, keycode, modifiers);

                if (this->keyboard_repeat_delay > 0)
                {
                    yield_thread(std::chrono::milliseconds(this->keyboard_repeat_delay));
                }
            }
            else
            {
                currently_held_key = code;
                held_keys.push_back(code);

                //Post Event
                if (!this->has_focus())
                {
                    this->gain_focus();
                }

                std::int32_t modifiers = GetActiveKeyModifiers();
                std::int32_t keycode = GetNativeKeyCode(code);
                this->PostEvent(KeyButton::KEY_PRESSED, keycode, modifiers);

                if (this->keyboard_speed >= 0 && this->keyboard_repeat_delay >= 0)
                {
                    input_thread.add_task([&](std::atomic_bool &stopped) {
                        if (this->keyboard_repeat_delay > 0)
                        {
                            yield_thread(std::chrono::milliseconds(this->keyboard_repeat_delay));
                        }

                        while (!stopped)
                        {
                            if (currently_held_key == -1)
                            {
                                break;
                            }

                            if (this->keyboard_speed > 0)
                            {
                                yield_thread(std::chrono::milliseconds(this->keyboard_speed));
                            }

                            std::int32_t code = currently_held_key;

                            //Post Event
                            if (!this->has_focus())
                            {
                                this->gain_focus();
                            }

                            std::int32_t modifiers = GetActiveKeyModifiers();
                            std::int32_t keycode = GetNativeKeyCode(code);
                            this->PostEvent(KeyButton::KEY_PRESSED, keycode, modifiers);
                        }
                    });
                }
            }
        }
    }
}

void NativeInputOutput::release_key(std::int32_t code) noexcept
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (!control_center)
    {
        return;
    }

    if (auto it = std::find(held_keys.begin(), held_keys.end(), code); it != held_keys.end())
    {
        if (std::find(std::begin(control_keys), std::end(control_keys), code) != std::end(control_keys))
        {
            held_keys.erase(it); //held_keys.erase(std::remove(held_keys.begin(), held_keys.end(), code), held_keys.end());

            //Post Event
            if (!this->has_focus())
            {
                this->gain_focus();
            }

            std::int32_t modifiers = GetActiveKeyModifiers();
            std::int32_t keycode = GetNativeKeyCode(code);
            this->PostEvent(KeyButton::KEY_RELEASED, keycode, modifiers);
        }
        else
        {
            //Remove the held key from the list..
            held_keys.erase(it);

            //Find the next non-control held-key..
            auto jt = std::find_if(held_keys.crbegin(), held_keys.crend(), [](std::int32_t key){
                return std::find(std::begin(control_keys), std::end(control_keys), key) == std::end(control_keys);
            });

            //Set the next currently held key to the first non-control key..
            currently_held_key = jt != held_keys.crend() ? *jt : -1;

            //Post Event
            if (!this->has_focus())
            {
                this->gain_focus();
            }

            std::int32_t modifiers = GetActiveKeyModifiers();
            std::int32_t keycode = GetNativeKeyCode(code);
            this->PostEvent(KeyButton::KEY_RELEASED, keycode, modifiers);
        }
    }
}

bool NativeInputOutput::is_key_held(std::int32_t code) const noexcept
{
    return std::find(std::begin(held_keys), std::end(held_keys), code) != std::end(held_keys);
}

bool NativeInputOutput::any_key_held(std::array<std::int32_t, 4>&& keys) const noexcept
{
    return std::any_of(std::cbegin(held_keys), std::cend(held_keys), [&](std::int32_t key){
        return std::find(std::cbegin(keys), std::cend(keys), key) != std::cend(keys);
    });
}

void NativeInputOutput::send_string(std::string string, std::int32_t keywait, std::int32_t keymodwait) const noexcept
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (!control_center)
    {
        return;
    }

    java::Component receiver = control_center->reflect_canvas();
    JNIEnv* env = receiver.getEnv();

    if (!this->has_focus(&receiver))
    {
        this->gain_focus(&receiver);
    }

    bool isShiftDown = false;

    for (std::size_t i = 0; i < string.length(); ++i)
    {
        char c = string[i];
        char n = i == string.length() - 1 ? '\0' : string[i + 1];
        std::int32_t modifiers = this->ModifiersForChar(c);

        //Modifier Key
        if (modifiers & ControlKeys::SHIFT_DOWN_MASK)
        {
            if (!isShiftDown)
            {
                isShiftDown = true;

                std::int32_t modifiers = ControlKeys::SHIFT_DOWN_MASK;
                std::int32_t keycode = GetNativeKeyCode(VK_LSHIFT);
                this->PostEvent(KeyButton::KEY_PRESSED, keycode, modifiers);

                yield_thread(std::chrono::milliseconds(lround(Random::instance()->generate_random_float(1.0, 1.1) * keymodwait)));
            }
        }

        //Character Key
        bool is_shift_key = false;
        std::int32_t code = static_cast<std::int32_t>(c);
        std::int32_t keycode = CharToKeyCode(code, is_shift_key);
        this->PostEvent(KeyButton::KEY_PRESSED, keycode, modifiers);

        yield_thread(std::chrono::milliseconds(lround(Random::instance()->generate_random_float(1.0, 1.1) * keywait)));

        this->PostEvent(KeyButton::KEY_RELEASED, keycode, modifiers);

        yield_thread(std::chrono::milliseconds(lround(Random::instance()->generate_random_float(1.0, 1.1) * keywait)));

        //Modifier Key
        if ((isShiftDown && i == string.length() - 1) || (n != '\0' && !(this->ModifiersForChar(n) & ControlKeys::SHIFT_DOWN_MASK)))
        {
            isShiftDown = false;

            std::int32_t modifiers = ControlKeys::SHIFT_DOWN_MASK;
            std::int32_t keycode = VK_LSHIFT;
            this->PostEvent(KeyButton::KEY_RELEASED, keycode, modifiers);

            yield_thread(std::chrono::milliseconds(lround(Random::instance()->generate_random_float(1.0, 1.1) * keymodwait)));
        }
    }
}

void NativeInputOutput::send_key(char key, std::int32_t key_down_time, std::int32_t key_up_time, std::int32_t modifier_down_time, std::int32_t modifier_up_time) const noexcept
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (!control_center)
    {
        return;
    }

    if (!this->has_focus())
    {
        this->gain_focus();
    }

    std::int32_t modifiers = this->ModifiersForChar(key);

    //Modifier Key is already pressed
    bool isShiftDown = GetActiveKeyModifiers() & ControlKeys::SHIFT_DOWN_MASK;

    //Modifier Key
    if (!isShiftDown && (modifiers & ControlKeys::SHIFT_DOWN_MASK))
    {
        std::int32_t modifiers = ControlKeys::SHIFT_DOWN_MASK;
        std::int32_t keycode = VK_LSHIFT;
        

        java::KeyEvent::Post(env,
                             &receiver,
                             java::KeyEvent::KeyCodes::KEY_PRESSED,
                             when,
                             modifiers,
                             keycode,
                             static_cast<jchar>(java::KeyEvent::KeyCodes::CHAR_UNDEFINED),
                             location);

        yield_thread(std::chrono::milliseconds(modifier_down_time));
    }

    //Character Key
    std::int32_t code = static_cast<std::int32_t>(key);
    std::int32_t keycode = CharToJavaKeyCode(code);

    java::KeyEvent::Post(env,
                         &receiver,
                         java::KeyEvent::KeyCodes::KEY_PRESSED,
                         when,
                         modifiers,
                         keycode,
                         static_cast<jchar>(key),
                         location);

    java::KeyEvent::Post(env,
                         &receiver,
                         java::KeyEvent::KeyCodes::KEY_TYPED,
                         when,
                         modifiers,
                         0,
                         static_cast<jchar>(key),
                         java::KeyEvent::KeyCodes::KEY_LOCATION_UNKNOWN);

    yield_thread(std::chrono::milliseconds(key_down_time));

    java::KeyEvent::Post(env,
                         &receiver,
                         java::KeyEvent::KeyCodes::KEY_RELEASED,
                         when,
                         modifiers,
                         keycode,
                         static_cast<jchar>(key),
                         location);

    yield_thread(std::chrono::milliseconds(key_up_time));

    //Modifier Key
    if (!isShiftDown && (modifiers & java::InputEvent::InputEventMasks::SHIFT_DOWN_MASK))
    {
        std::int32_t code = VK_LSHIFT;
        std::int32_t modifiers = java::InputEvent::InputEventMasks::SHIFT_DOWN_MASK;
        std::int32_t keycode = GetJavaKeyCode(code);

        java::KeyEvent::Post(env,
                             &receiver,
                             java::KeyEvent::KeyCodes::KEY_RELEASED,
                             when,
                             modifiers,
                             keycode,
                             static_cast<jchar>(java::KeyEvent::KeyCodes::CHAR_UNDEFINED),
                             location);

        yield_thread(std::chrono::milliseconds(modifier_up_time));
    }
}

void NativeInputOutput::key_send(std::string string, std::vector<std::int32_t> sleeptimes) const noexcept
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (!control_center)
    {
        return;
    }

    if (!this->has_focus())
    {
        this->gain_focus();
    }

    bool isShiftDown = false;
    std::int32_t sleepindex = 0;

    for (std::size_t i = 0; i < string.length(); ++i)
    {
        char c = string[i];
        char n = i == string.length() - 1 ? '\0' : string[i + 1];
        std::int32_t modifiers = this->ModifiersForChar(c);

        //Modifier Key
        if (modifiers & java::InputEvent::InputEventMasks::SHIFT_DOWN_MASK)
        {
            if (!isShiftDown)
            {
                isShiftDown = true;

                std::int32_t code = VK_LSHIFT;
                std::int32_t modifiers = java::InputEvent::InputEventMasks::SHIFT_DOWN_MASK;
                std::int32_t keycode = GetJavaKeyCode(code);

                java::KeyEvent::Post(env,
                                     &receiver,
                                     java::KeyEvent::KeyCodes::KEY_PRESSED,
                                     when,
                                     modifiers,
                                     keycode,
                                     static_cast<jchar>(java::KeyEvent::KeyCodes::CHAR_UNDEFINED),
                                     location);

                yield_thread(std::chrono::milliseconds(sleeptimes[sleepindex++]));
            }
        }

        //Character Key
        std::int32_t code = static_cast<std::int32_t>(c);
        std::int32_t keycode = CharToJavaKeyCode(code);

        java::KeyEvent::Post(env,
                             &receiver,
                             java::KeyEvent::KeyCodes::KEY_PRESSED,
                             when,
                             modifiers,
                             keycode,
                             static_cast<jchar>(c),
                             location);

        java::KeyEvent::Post(env,
                             &receiver,
                             java::KeyEvent::KeyCodes::KEY_TYPED,
                             when,
                             modifiers,
                             0,
                             static_cast<jchar>(c),
                             java::KeyEvent::KeyCodes::KEY_LOCATION_UNKNOWN);

        yield_thread(std::chrono::milliseconds(sleeptimes[sleepindex++]));

        java::KeyEvent::Post(env,
                             &receiver,
                             java::KeyEvent::KeyCodes::KEY_RELEASED,
                             when,
                             modifiers,
                             keycode,
                             static_cast<jchar>(c),
                             location);

        yield_thread(std::chrono::milliseconds(sleeptimes[sleepindex++]));

        //Modifier Key
        if ((isShiftDown && i == string.length() - 1) || (n != '\0' && !(this->ModifiersForChar(n) & java::InputEvent::InputEventMasks::SHIFT_DOWN_MASK)))
        {
            isShiftDown = false;

            std::int32_t code = VK_LSHIFT;
            std::int32_t modifiers = java::InputEvent::InputEventMasks::SHIFT_DOWN_MASK;
            std::int32_t keycode = GetJavaKeyCode(code);

            java::KeyEvent::Post(env,
                                 &receiver,
                                 java::KeyEvent::KeyCodes::KEY_RELEASED,
                                 when,
                                 modifiers,
                                 keycode,
                                 static_cast<jchar>(java::KeyEvent::KeyCodes::CHAR_UNDEFINED),
                                 location);

            yield_thread(std::chrono::milliseconds(sleeptimes[sleepindex++]));
        }
    }
}

bool NativeInputOutput::has_focus() const noexcept
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (!control_center)
    {
        return false;
    }

    return component.hasFocus();
}

void NativeInputOutput::gain_focus() const noexcept
{
    JNIEnv* env = component->getEnv();
    java::Window window = java::SunToolkit::getContainingWindow(component);

    if (window.get())
    {
        java::WindowEvent::Post(env, &window, java::WindowEvent::WindowEventCodes::WINDOW_ACTIVATED, 0, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(Random::instance()->generate_random_int(100, 250)));
        java::WindowEvent::Post(env, &window, java::WindowEvent::WindowEventCodes::WINDOW_GAINED_FOCUS, 0, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(Random::instance()->generate_random_int(100, 250)));
    }

    java::FocusEvent::Post(env, component, java::FocusEvent::FocusCodes::FOCUS_GAINED, false, java::FocusEvent::Cause::ACTIVATION);

    auto now = std::chrono::high_resolution_clock::now();
    while (!component->hasFocus() && elapsed_time<std::chrono::seconds>(now) < 2)
    {
        yield_thread(std::chrono::milliseconds(Random::instance()->generate_random_int(100, 250)));
    }
}

void NativeInputOutput::lose_focus() const noexcept
{
    java::Window window = java::SunToolkit::getContainingWindow(component);

    if (window.get())
    {
        java::WindowEvent::Post(env, &window, java::WindowEvent::WindowEventCodes::WINDOW_DEACTIVATED, 0, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(Random::instance()->generate_random_int(100, 250)));
    }

    java::FocusEvent::Post(env, component, java::FocusEvent::FocusCodes::FOCUS_LOST, true, java::FocusEvent::Cause::ACTIVATION);

    auto now = std::chrono::high_resolution_clock::now();
    while (component->hasFocus() && elapsed_time<std::chrono::seconds>(now) < 2)
    {
        yield_thread(std::chrono::milliseconds(Random::instance()->generate_random_int(100, 250)));
    }
}

bool NativeInputOutput::is_keyboard_input_enabled() const noexcept
{
    return event_queue->is_keyboard_input_enabled();
}

void NativeInputOutput::set_keyboard_input_enabled(bool enabled) const noexcept
{
    event_queue->set_keyboard_input_enabled(enabled);

    if (!enabled)
    {
        if (!this->has_focus())
        {
            this->gain_focus();
        }
    }
}

bool NativeInputOutput::is_mouse_input_enabled() const noexcept
{
    return event_queue->is_mouse_input_enabled();
}

void NativeInputOutput::set_mouse_input_enabled(bool enabled) const noexcept
{
    event_queue->set_mouse_input_enabled(enabled);

    if (!enabled)
    {
        if (!this->has_focus())
        {
            this->gain_focus();
        }
    }
}

std::int32_t NativeInputOutput::get_keyboard_speed() const noexcept
{
    return this->keyboard_speed;
}

void NativeInputOutput::set_keyboard_speed(std::int32_t speed) noexcept
{
    this->keyboard_speed = speed;
}

std::int32_t NativeInputOutput::get_keyboard_repeat_delay() const noexcept
{
    return this->keyboard_repeat_delay;
}

void NativeInputOutput::set_keyboard_repeat_delay(std::int32_t delay) noexcept
{
    this->keyboard_repeat_delay = delay;
}

void NativeInputOutput::get_mouse_position(std::int32_t* x, std::int32_t* y) noexcept
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (!control_center)
    {
        return;
    }

    if (this->x == std::numeric_limits<std::int32_t>::min() || this->y == std::numeric_limits<std::int32_t>::min())
    {
        java::Component receiver = control_center->reflect_canvas();
        receiver.getMousePosition(this->x, this->y);
        receiver.getSize(this->w, this->h);

        if (this->x == -1 || this->y == -1)
        {
            java::Component receiver = control_center->reflect_canvas();
            JNIEnv* env = receiver.getEnv();

            std::int32_t x = this->x;
            std::int32_t y = this->y;

            java::PointerInfo info = java::PointerInfo::getPointerInfo(env);
            info.getLocation(x, y);
            info.PointToScreen(env, x, y, &receiver);

            this->x = x;
            this->y = y;
        }
    }
    else if (!has_focus() && (!is_keyboard_input_enabled() || !is_mouse_input_enabled()))
    {
        java::Component receiver = control_center->reflect_canvas();
        this->handle_resize(&receiver);
    }

    *x = this->x;
    *y = this->y;
}

void NativeInputOutput::get_real_mouse_position(std::int32_t* x, std::int32_t* y) const noexcept
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (!control_center)
    {
        return;
    }

    java::Component receiver = control_center->reflect_canvas();
    JNIEnv* env = receiver.getEnv();

    java::PointerInfo info = java::PointerInfo::getPointerInfo(env);
    info.getLocation(*x, *y);
    info.PointToScreen(env, *x, *y, &receiver);
}

void NativeInputOutput::move_mouse(std::int32_t x, std::int32_t y) noexcept
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (!control_center)
    {
        return;
    }

    java::Component receiver = control_center->reflect_canvas();
    JNIEnv* env = receiver.getEnv();
    this->handle_resize(&receiver);

    std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    bool isRequestedPositionInsideComponent = !(x < 0 || y < 0 || x > static_cast<std::int32_t>(this->w) || y > static_cast<std::int32_t>(this->h));
    bool isMouseInsideComponent = !(this->x < 0 || this->y < 0 || this->x > static_cast<std::int32_t>(this->w) || this->y > static_cast<std::int32_t>(this->h));
    bool isDragging = mouse_buttons[0] || mouse_buttons[1] || mouse_buttons[2];

    //Button priority is 1 (left), 3 (right), 2 (middle)
    std::int32_t button = mouse_buttons[0] ? 1 : mouse_buttons[2] ? 3 : mouse_buttons[1] ? 2 : 0;
    std::int32_t buttonMask = (mouse_buttons[0] ? java::InputEvent::GetDownMaskForButton(mouse_buttons[0]) : 0) |
                              (mouse_buttons[1] ? java::InputEvent::GetDownMaskForButton(mouse_buttons[1]) : 0) |
                              (mouse_buttons[2] ? java::InputEvent::GetDownMaskForButton(mouse_buttons[2]) : 0);

    //Key extended masks
    buttonMask |= GetActiveKeyModifiers();

    if (isRequestedPositionInsideComponent && !isMouseInsideComponent)
    {
        //MOUSE_ENTERED
        this->x = x; this->y = y;
        java::MouseEvent::Post(env, &receiver, java::MouseEvent::MouseEventCodes::MOUSE_ENTERED, when, buttonMask, x, y, 0, false, 0);
        java::MouseEvent::Post(env, &receiver, java::MouseEvent::MouseEventCodes::MOUSE_MOVED, when, buttonMask, x, y, 0, false, 0);
    }
    else if (!isRequestedPositionInsideComponent && isMouseInsideComponent)
    {
        //MOUSE_EXITED
        this->x = x; this->y = y;
        java::MouseEvent::Post(env, &receiver, java::MouseEvent::MouseEventCodes::MOUSE_EXITED, when, buttonMask, x, y, 0, false, 0);
    }
    else if (isRequestedPositionInsideComponent && isMouseInsideComponent)
    {
        //MOUSE_MOVED OR MOUSE_DRAGGED
        if (isDragging)
        {
            this->x = x; this->y = y;
            java::MouseEvent::Post(env, &receiver, java::MouseEvent::MouseEventCodes::MOUSE_DRAGGED, when, buttonMask, x, y, click_count, false, button);
        }
        else
        {
            this->x = x; this->y = y;
            java::MouseEvent::Post(env, &receiver, java::MouseEvent::MouseEventCodes::MOUSE_MOVED, when, buttonMask, x, y, 0, false, 0);
        }
    }
    else if (!isRequestedPositionInsideComponent && !isMouseInsideComponent)
    {
        //MOUSE_DRAGGED OUTSIDE
        this->x = x; this->y = y;
        if (isDragging)
        {
            java::MouseEvent::Post(env, &receiver, java::MouseEvent::MouseEventCodes::MOUSE_DRAGGED, when, buttonMask, x, y, click_count, false, button);
        }
    }
}

void NativeInputOutput::hold_mouse(std::int32_t button) noexcept
{
    if (!this->is_mouse_held(button))
    {
        extern std::unique_ptr<ControlCenter> control_center;
        if (!control_center)
        {
            return;
        }

        java::Component receiver = control_center->reflect_canvas();
        JNIEnv* env = receiver.getEnv();
        this->handle_resize(&receiver);

        bool isRequestedPositionInsideComponent = !(this->x < 0 || this->y < 0 || this->x > static_cast<std::int32_t>(this->w) || this->y > static_cast<std::int32_t>(this->h));
        if (isRequestedPositionInsideComponent)
        {
            std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

            //Button priority is 1 (left), 3 (right), 2 (middle)
            mouse_buttons[SimbaMouseButtonToJava(button) - 1] = true;
            std::int32_t button = mouse_buttons[0] ? 1 : mouse_buttons[2] ? 3 : mouse_buttons[1] ? 2 : 0;
            std::int32_t buttonMask = (mouse_buttons[0] ? java::InputEvent::GetDownMaskForButton(mouse_buttons[0]) : 0) |
                                      (mouse_buttons[1] ? java::InputEvent::GetDownMaskForButton(mouse_buttons[1]) : 0) |
                                      (mouse_buttons[2] ? java::InputEvent::GetDownMaskForButton(mouse_buttons[2]) : 0);

            //Gain Focus
            if (!this->has_focus(&receiver))
            {
                this->gain_focus(&receiver);
            }

            //Key extended masks
            buttonMask |= GetActiveKeyModifiers();
            java::MouseEvent::Post(env, &receiver, java::MouseEvent::MouseEventCodes::MOUSE_PRESSED, when, buttonMask, this->x, this->y, click_count, false, button);
        }
        else
        {
            //Lose Focus
            if (this->has_focus(&receiver))
            {
                this->lose_focus(&receiver);
            }
        }
    }
}

void NativeInputOutput::release_mouse(std::int32_t button) noexcept
{
    if (this->is_mouse_held(button))
    {
        extern std::unique_ptr<ControlCenter> control_center;
        if (!control_center)
        {
            return;
        }

        java::Component receiver = control_center->reflect_canvas();
        JNIEnv* env = receiver.getEnv();
        this->handle_resize(&receiver);

        std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

        bool isRequestedPositionInsideComponent = !(this->x < 0 || this->y < 0 || this->x > static_cast<std::int32_t>(this->w) || this->y > static_cast<std::int32_t>(this->h));
        bool isMouseInsideComponent = !(this->x < 0 || this->y < 0 || this->x > static_cast<std::int32_t>(this->w) || this->y > static_cast<std::int32_t>(this->h));
        bool isDragging = mouse_buttons[0] || mouse_buttons[1] || mouse_buttons[2];

        //Button priority is 1 (left), 3 (right), 2 (middle)
        mouse_buttons[SimbaMouseButtonToJava(button) - 1] = false;

        std::int32_t button = mouse_buttons[0] ? 1 : mouse_buttons[2] ? 3 : mouse_buttons[1] ? 2 : 0;
        std::int32_t buttonMask = (mouse_buttons[0] ? java::InputEvent::GetDownMaskForButton(mouse_buttons[0]) : 0) |
                                  (mouse_buttons[1] ? java::InputEvent::GetDownMaskForButton(mouse_buttons[1]) : 0) |
                                  (mouse_buttons[2] ? java::InputEvent::GetDownMaskForButton(mouse_buttons[2]) : 0);

        //Key extended masks
        buttonMask |= GetActiveKeyModifiers();

        //MOUSE_RELEASED
        java::MouseEvent::Post(env, &receiver, java::MouseEvent::MouseEventCodes::MOUSE_RELEASED, when, buttonMask, this->x, this->y, click_count, false, button);

        if (!isDragging && isRequestedPositionInsideComponent && isMouseInsideComponent)
        {
            //MOUSE_CLICKED
            java::MouseEvent::Post(env, &receiver, java::MouseEvent::MouseEventCodes::MOUSE_CLICKED, when, buttonMask, this->x, this->y, click_count, false, button);
        }
    }
}

void NativeInputOutput::scroll_mouse(std::int32_t lines) noexcept
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (!control_center)
    {
        return;
    }

    java::Component receiver = control_center->reflect_canvas();
    JNIEnv* env = receiver.getEnv();
    this->handle_resize(&receiver);

    bool isRequestedPositionInsideComponent = !(this->x < 0 || this->y < 0 || this->x > static_cast<std::int32_t>(this->w) || this->y > static_cast<std::int32_t>(this->h));
    bool isMouseInsideComponent = !(this->x < 0 || this->y < 0 || this->x > static_cast<std::int32_t>(this->w) || this->y > static_cast<std::int32_t>(this->h));

    if (isRequestedPositionInsideComponent && isMouseInsideComponent)
    {
        //Relative Mouse Position
        std::int32_t cx = 0;
        std::int32_t cy = 0;
        receiver.getLocationOnScreen(cx, cy);

        if (cx == -1 || cy == -1)
        {
            cx = 0;
            cy = 0;
        }

        std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        std::int32_t modifiers = GetActiveKeyModifiers();

        //Technically, this should be gausian curve or bell-curve or parabola as the wheel speeds up and slows down..
        double precision = lines > 0 ? Random::instance()->generate_random_int(1, 9) : lines < 0 ? Random::instance()->generate_random_int(-9, -1) : Random::instance()->generate_random_double(-1, 1);
        precision /= 10.0;

        java::MouseWheelEvent::Post(env,
                                    &receiver,
                                    java::MouseEvent::MouseEventCodes::MOUSE_WHEEL,
                                    when,
                                    modifiers,
                                    this->x,
                                    this->y,
                                    cx + this->x,
                                    cy + this->y,
                                    0,
                                    false,
                                    java::MouseWheelEvent::MouseWheelEventCodes::WHEEL_UNIT_SCROLL,
                                    1,
                                    lines,
                                    lines + precision);
    }
    else if (isRequestedPositionInsideComponent && !isMouseInsideComponent)
    {
        std::int64_t when = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

        bool isDragging = mouse_buttons[0] || mouse_buttons[1] || mouse_buttons[2];
        if (isDragging)
        {
            return; //Developer is an idiot. We shouldn't even have to be here in the first place..
        }

        //Button priority is 1 (left), 3 (right), 2 (middle)
        std::int32_t buttonMask = (mouse_buttons[0] ? java::InputEvent::GetDownMaskForButton(mouse_buttons[0]) : 0) |
                                  (mouse_buttons[1] ? java::InputEvent::GetDownMaskForButton(mouse_buttons[1]) : 0) |
                                  (mouse_buttons[2] ? java::InputEvent::GetDownMaskForButton(mouse_buttons[2]) : 0);

        //Key extended masks
        buttonMask |= GetActiveKeyModifiers();


        //MOUSE_ENTERED
        java::MouseEvent::Post(env, &receiver, java::MouseEvent::MouseEventCodes::MOUSE_ENTERED, when, buttonMask, this->x, this->y, 0, false, 0);
        java::MouseEvent::Post(env, &receiver, java::MouseEvent::MouseEventCodes::MOUSE_MOVED, when, buttonMask, this->x, this->y, 0, false, 0);

        // Recursive call
        scroll_mouse(lines);
    }
}

bool NativeInputOutput::is_mouse_held(std::int32_t button) const noexcept
{
    switch (button)
    {
        case 1: return mouse_buttons[0];
        case 2: return mouse_buttons[1];
        case 3: return mouse_buttons[2];
        default: return false;
    }
}

void NativeInputOutput::stop_all_processing() noexcept
{
    std::int32_t x = 0;
    std::int32_t y = 0;

    this->get_mouse_position(&x, &y);
    this->release_mouse(0);
    this->release_mouse(1);
    this->release_mouse(2);

    for (std::int32_t key : std::vector<std::int32_t>(held_keys.begin(), held_keys.end()))
    {
        this->release_key(key);
    }

    this->set_keyboard_input_enabled(true);
    this->set_mouse_input_enabled(true);
}

char NativeInputOutput::KeyCodeToChar(std::int32_t keycode, bool shifted) const noexcept
{
    static const std::uint8_t mapping[256] = {
            [VK_BACK] = 0x0008,              [VK_TAB] = 0x0009,              [VK_RETURN] = 0x000D,              [VK_0] = 0x0030,
            [VK_1] = 0x0031,                 [VK_2] = 0x0032,                [VK_3] = 0x0033,                   [VK_4] = 0x0034,
            [VK_5] = 0x0035,                 [VK_6] = 0x0036,                [VK_7] = 0x0037,                   [VK_8] = 0x0038,
            [VK_9] = 0x0039,                 [VK_A] = 0x0061,                [VK_B] = 0x0062,                   [VK_C] = 0x0063,
            [VK_D] = 0x0064,                 [VK_E] = 0x0065,                [VK_F] = 0x0066,                   [VK_G] = 0x0067,
            [VK_H] = 0x0068,                 [VK_I] = 0x0069,                [VK_J] = 0x006A,                   [VK_K] = 0x006B,
            [VK_L] = 0x006C,                 [VK_M] = 0x006D,                [VK_N] = 0x006E,                   [VK_O] = 0x006F,
            [VK_P] = 0x0070,                 [VK_Q] = 0x0071,                [VK_R] = 0x0072,                   [VK_S] = 0x0073,
            [VK_T] = 0x0074,                 [VK_U] = 0x0075,                [VK_V] = 0x0076,                   [VK_W] = 0x0077,
            [VK_X] = 0x0078,                 [VK_Y] = 0x0079,                [VK_Z] = 0x007A,                   [VK_NUMPAD1] = 0x0031,
            [VK_NUMPAD2] = 0x0032,           [VK_NUMPAD3] = 0x0033,          [VK_NUMPAD4] = 0x0034,             [VK_NUMPAD5] = 0x0035,
            [VK_NUMPAD6] = 0x0036,           [VK_NUMPAD7] = 0x0037,          [VK_NUMPAD8] = 0x0038,             [VK_NUMPAD9] = 0x0039,
            [VK_MULTIPLY] = 0x002A,          [VK_ADD] = 0x002B,              [VK_SEPARATOR] = 0x002C,           [VK_SUBTRACT] = 0x002D,
            [VK_DECIMAL] = 0x002E,           [VK_DIVIDE] = 0x002F,           [VK_OEM_PLUS] = 0x002B,            [VK_OEM_COMMA] = 0x002C,
            [VK_OEM_MINUS] = 0x002D,         [VK_OEM_PERIOD] = 0x002E,       [VK_OEM_1] = 0x003B,               [VK_OEM_2] = 0x002F,
            [VK_OEM_3] = 0x60,               [VK_OEM_4] = 0x005B,            [VK_OEM_5] = 0x005C,               [VK_OEM_6] = 0x005D,
            [VK_OEM_7] = 0x0027,
    };

    if (shifted)
    {
        static const std::string normal_chars = "`1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,./";
        static const std::string shift_chars = "~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?";
        if (auto index = normal_chars.find(static_cast<char>(mapping[keycode])); index != std::string::npos)
        {
            return static_cast<char>(shift_chars[index]);
        }
    }

    return static_cast<char>(mapping[keycode]);
}

std::int32_t NativeInputOutput::CharToKeyCode(char chr, bool &shifted) const noexcept
{
    static const std::uint8_t mapping[256] = {
            [0x0008] = VK_BACK,              [0x0009] = VK_TAB,              [0x000D] = VK_RETURN,              [0x0030] = VK_0,
            [0x0031] = VK_1,                 [0x0032] = VK_2,                [0x0033] = VK_3,                   [0x0034] = VK_4,
            [0x0035] = VK_5,                 [0x0036] = VK_6,                [0x0037] = VK_7,                   [0x0038] = VK_8,
            [0x0039] = VK_9,                 [0x0061] = VK_A,                [0x0062] = VK_B,                   [0x0063] = VK_C,
            [0x0064] = VK_D,                 [0x0065] = VK_E,                [0x0066] = VK_F,                   [0x0067] = VK_G,
            [0x0068] = VK_H,                 [0x0069] = VK_I,                [0x006A] = VK_J,                   [0x006B] = VK_K,
            [0x006C] = VK_L,                 [0x006D] = VK_M,                [0x006E] = VK_N,                   [0x006F] = VK_O,
            [0x0070] = VK_P,                 [0x0071] = VK_Q,                [0x0072] = VK_R,                   [0x0073] = VK_S,
            [0x0074] = VK_T,                 [0x0075] = VK_U,                [0x0076] = VK_V,                   [0x0077] = VK_W,
            [0x0078] = VK_X,                 [0x0079] = VK_Y,                [0x007A] = VK_Z,                   [0x002A] = VK_MULTIPLY,
            [0x002B] = VK_ADD,               [0x002C] = VK_SEPARATOR,        [0x002D] = VK_SUBTRACT,            [0x002E] = VK_DECIMAL,
            [0x002F] = VK_DIVIDE,            [0x003B] = VK_OEM_1,            [0x60] = VK_OEM_3,                 [0x005B] = VK_OEM_4,
            [0x005C] = VK_OEM_5,             [0x005D] = VK_OEM_6,            [0x0027] = VK_OEM_7,
    };

    static const std::string normal_chars = "`1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,./";
    static const std::string shift_chars = "~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?";
    if (auto index = shift_chars.find(chr); index != std::string::npos)
    {
        shifted = true;
        return mapping[normal_chars[index]];
    }

    shifted = false;
    return mapping[static_cast<std::int32_t>(chr)];
}

std::int32_t NativeInputOutput::GetActiveKeyModifiers() const noexcept
{
    std::int32_t modifiers = 0;

    //Key extended masks
    if (this->any_key_held({ControlKeys::SHIFT, ControlKeys::LSHIFT, ControlKeys::RSHIFT}))
    {
        modifiers |= ControlKeys::SHIFT_DOWN_MASK;
    }

    if (this->any_key_held({ControlKeys::CONTROL, ControlKeys::LEFT_CONTROL, ControlKeys::RIGHT_CONTROL}))
    {
        modifiers |= ControlKeys::CTRL_DOWN_MASK;
    }

    if (this->is_key_held(ControlKeys::LEFT_ALT))
    {
        modifiers |= ControlKeys::ALT_DOWN_MASK;
    }

    if (this->is_key_held(ControlKeys::RIGHT_ALT))
    {
        modifiers |= ControlKeys::ALT_GRAPH_DOWN_MASK;
    }

    if (this->any_key_held({ControlKeys::LEFT_WINDOWS, ControlKeys::RIGHT_WINDOWS, ControlKeys::COMMAND}))
    {
        modifiers |= ControlKeys::META_DOWN_MASK;
    }
    return modifiers;
}

std::int32_t NativeInputOutput::ModifiersForChar(char c) const noexcept
{
    std::int32_t modifiers = 0;
    static const std::string shift_chars = "~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?";
    if (shift_chars.find(c) != std::string::npos)
    {
        modifiers |= ControlKeys::SHIFT_DOWN_MASK;
    }
    return modifiers;
}

std::int32_t NativeInputOutput::GetNativeKeyCode(std::int32_t keycode) const noexcept
{
    return keycode;
}

std::int32_t NativeInputOutput::SimbaMouseButtonToNative(std::int32_t button) const noexcept
{
    //Simba Button priority is 1 (left), 0 (right), 2 (middle)
    return button == 1 ? MouseButton::LEFT : button == 0 ? MouseButton::RIGHT : button == 2 ? MouseButton::MIDDLE : button;
}