#ifndef OUR_CUSTOM_KEYCODES_H
#define OUR_CUSTOM_KEYCODES_H

typedef enum KeyState { UNPRESSED = 0, PRESSED = 1 } KeyState;

/*typedef enum KeyAction { KEY_PRESSED = 1, KEY_RELEASED = -1 } KeyAction;*/

typedef enum KeyCode {
    KEY_UNKNOWN = 0,

    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,

    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,

    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,

    KEY_SEMICOLON,     /* ; */
    KEY_EQUAL,         /* = */
    KEY_LEFT_BRACKET,  /* [ */
    KEY_BACKSLASH,     /* \ */
    KEY_RIGHT_BRACKET, /* ] */
    KEY_GRAVE_ACCENT,  /* ` */
    KEY_APOSTROPHE,
    KEY_COMMA,
    KEY_MINUS,
    KEY_PERIOD,
    KEY_SLASH,

    KEY_RIGHT_ARROW,
    KEY_LEFT_ARROW,
    KEY_DOWN_ARROW,
    KEY_UP_ARROW,

    KEY_INSERT,
    KEY_DELETE,
    KEY_HOME,
    KEY_END,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,

    KEY_ESCAPE,
    KEY_ENTER,
    KEY_TAB,
    KEY_BACKSPACE,
    KEY_SPACE,
    KEY_LEFT_SHIFT,
    KEY_LEFT_CONTROL,
    KEY_LEFT_ALT,
    KEY_LEFT_SUPER,
    KEY_RIGHT_SHIFT,
    KEY_RIGHT_CONTROL,
    KEY_RIGHT_ALT,
    KEY_RIGHT_SUPER,

    KEY_CAPS_LOCK,
    KEY_SCROLL_LOCK,
    KEY_NUM_LOCK,
    KEY_PRINT_SCREEN,
    KEY_PAUSE,

    KEY_NP_0,
    KEY_NP_1,
    KEY_NP_2,
    KEY_NP_3,
    KEY_NP_4,
    KEY_NP_5,
    KEY_NP_6,
    KEY_NP_7,
    KEY_NP_8,
    KEY_NP_9,
    KEY_NP_DECIMAL,
    KEY_NP_DIVIDE,
    KEY_NP_MULTIPLY,
    KEY_NP_SUBTRACT,
    KEY_NP_ADD,
    KEY_NP_ENTER,
    KEY_NP_EQUAL,

    KEY_COUNT
} KeyCode;

typedef enum MouseButtons {
    MOUSE_BUTTON_LEFT   = 0,
    MOUSE_BUTTON_RIGHT  = 1,
    MOUSE_BUTTON_MIDDLE = 2,
    MOUSE_BUTTON_COUNT
} MouseButtons;

typedef enum ModifierKeys {
    SHIFT,
    CONTROL,
    SUPER,
    ALT,
    CAPS_LOCK,
    NUM_LOCK
} ModifierKeys;

#endif
