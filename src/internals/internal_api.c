#include "internal_api.h"
#include <string.h>
#include <xkbcommon/xkbcommon-keysyms.h>

KeyCode _getOurKeyCode_from_xkb_keysym(xkb_keysym_t sym) {
    switch (sym) {
        case XKB_KEY_a:
        case XKB_KEY_A: return KEY_A;
        case XKB_KEY_b:
        case XKB_KEY_B: return KEY_B;
        case XKB_KEY_c:
        case XKB_KEY_C: return KEY_C;
        case XKB_KEY_d:
        case XKB_KEY_D: return KEY_D;
        case XKB_KEY_e:
        case XKB_KEY_E: return KEY_E;
        case XKB_KEY_f:
        case XKB_KEY_F: return KEY_F;
        case XKB_KEY_g:
        case XKB_KEY_G: return KEY_G;
        case XKB_KEY_h:
        case XKB_KEY_H: return KEY_H;
        case XKB_KEY_i:
        case XKB_KEY_I: return KEY_I;
        case XKB_KEY_j:
        case XKB_KEY_J: return KEY_J;
        case XKB_KEY_k:
        case XKB_KEY_K: return KEY_K;
        case XKB_KEY_l:
        case XKB_KEY_L: return KEY_L;
        case XKB_KEY_m:
        case XKB_KEY_M: return KEY_M;
        case XKB_KEY_n:
        case XKB_KEY_N: return KEY_N;
        case XKB_KEY_o:
        case XKB_KEY_O: return KEY_O;
        case XKB_KEY_p:
        case XKB_KEY_P: return KEY_P;
        case XKB_KEY_q:
        case XKB_KEY_Q: return KEY_Q;
        case XKB_KEY_r:
        case XKB_KEY_R: return KEY_R;
        case XKB_KEY_s:
        case XKB_KEY_S: return KEY_S;
        case XKB_KEY_t:
        case XKB_KEY_T: return KEY_T;
        case XKB_KEY_u:
        case XKB_KEY_U: return KEY_U;
        case XKB_KEY_v:
        case XKB_KEY_V: return KEY_V;
        case XKB_KEY_w:
        case XKB_KEY_W: return KEY_W;
        case XKB_KEY_x:
        case XKB_KEY_X: return KEY_X;
        case XKB_KEY_y:
        case XKB_KEY_Y: return KEY_Y;
        case XKB_KEY_z:
        case XKB_KEY_Z: return KEY_Z;

        case XKB_KEY_0: return KEY_0;
        case XKB_KEY_1: return KEY_1;
        case XKB_KEY_2: return KEY_2;
        case XKB_KEY_3: return KEY_3;
        case XKB_KEY_4: return KEY_4;
        case XKB_KEY_5: return KEY_5;
        case XKB_KEY_6: return KEY_6;
        case XKB_KEY_7: return KEY_7;
        case XKB_KEY_8: return KEY_8;
        case XKB_KEY_9: return KEY_9;

        case XKB_KEY_F1:  return KEY_F1;
        case XKB_KEY_F2:  return KEY_F2;
        case XKB_KEY_F3:  return KEY_F3;
        case XKB_KEY_F4:  return KEY_F4;
        case XKB_KEY_F5:  return KEY_F5;
        case XKB_KEY_F6:  return KEY_F6;
        case XKB_KEY_F7:  return KEY_F7;
        case XKB_KEY_F8:  return KEY_F8;
        case XKB_KEY_F9:  return KEY_F9;
        case XKB_KEY_F10: return KEY_F10;
        case XKB_KEY_F11: return KEY_F11;
        case XKB_KEY_F12: return KEY_F12;

        case XKB_KEY_semicolon:    return KEY_SEMICOLON;
        case XKB_KEY_equal:        return KEY_EQUAL;
        case XKB_KEY_bracketleft:  return KEY_LEFT_BRACKET;
        case XKB_KEY_bracketright: return KEY_RIGHT_BRACKET;
        case XKB_KEY_slash:        return KEY_SLASH;
        case XKB_KEY_backslash:    return KEY_BACKSLASH;
        case XKB_KEY_grave:        return KEY_GRAVE_ACCENT;
        case XKB_KEY_apostrophe:   return KEY_APOSTROPHE;
        case XKB_KEY_comma:        return KEY_COMMA;
        case XKB_KEY_minus:        return KEY_MINUS;
        case XKB_KEY_period:       return KEY_PERIOD;

        case XKB_KEY_rightarrow: return KEY_RIGHT_ARROW;
        case XKB_KEY_leftarrow:  return KEY_LEFT_ARROW;
        case XKB_KEY_uparrow:    return KEY_UP_ARROW;
        case XKB_KEY_downarrow:  return KEY_DOWN_ARROW;

        case XKB_KEY_Insert:    return KEY_INSERT;
        case XKB_KEY_Delete:    return KEY_DELETE;
        case XKB_KEY_Home:      return KEY_HOME;
        case XKB_KEY_End:       return KEY_END;
        case XKB_KEY_Page_Up:   return KEY_PAGE_UP;
        case XKB_KEY_Page_Down: return KEY_PAGE_DOWN;

        case XKB_KEY_Escape:    return KEY_ESCAPE;
        case XKB_KEY_Return:    return KEY_ENTER;
        case XKB_KEY_Tab:       return KEY_TAB;
        case XKB_KEY_BackSpace: return KEY_BACKSPACE;
        case XKB_KEY_space:     return KEY_SPACE;
        case XKB_KEY_Shift_L:   return KEY_LEFT_SHIFT;
        case XKB_KEY_Shift_R:   return KEY_RIGHT_SHIFT;
        case XKB_KEY_Control_L: return KEY_LEFT_CONTROL;
        case XKB_KEY_Control_R: return KEY_RIGHT_CONTROL;
        case XKB_KEY_Alt_L:     return KEY_LEFT_ALT;
        case XKB_KEY_Alt_R:     return KEY_RIGHT_ALT;
        case XKB_KEY_Super_L:   return KEY_LEFT_SUPER;
        case XKB_KEY_Super_R:   return KEY_RIGHT_SUPER;

        case XKB_KEY_Caps_Lock:   return KEY_CAPS_LOCK;
        case XKB_KEY_Scroll_Lock: return KEY_SCROLL_LOCK;
        case XKB_KEY_Num_Lock:    return KEY_NUM_LOCK;
        case XKB_KEY_Print:       return KEY_PRINT_SCREEN;
        case XKB_KEY_Pause:       return KEY_PAUSE;

        case XKB_KEY_KP_0:        return KEY_NP_0;
        case XKB_KEY_KP_1:        return KEY_NP_1;
        case XKB_KEY_KP_2:        return KEY_NP_2;
        case XKB_KEY_KP_3:        return KEY_NP_3;
        case XKB_KEY_KP_4:        return KEY_NP_4;
        case XKB_KEY_KP_5:        return KEY_NP_5;
        case XKB_KEY_KP_6:        return KEY_NP_6;
        case XKB_KEY_KP_7:        return KEY_NP_7;
        case XKB_KEY_KP_8:        return KEY_NP_8;
        case XKB_KEY_KP_9:        return KEY_NP_9;
        case XKB_KEY_KP_Decimal:  return KEY_NP_DECIMAL;
        case XKB_KEY_KP_Divide:   return KEY_NP_DIVIDE;
        case XKB_KEY_KP_Add:      return KEY_NP_ADD;
        case XKB_KEY_KP_Multiply: return KEY_NP_MULTIPLY;
        case XKB_KEY_KP_Subtract: return KEY_NP_SUBTRACT;
        case XKB_KEY_KP_Enter:    return KEY_NP_ENTER;
        case XKB_KEY_KP_Equal:    return KEY_NP_EQUAL;
    }

    return KEY_UNKNOWN;
}

void _registerKeyState(WaylandClientContext *clientState, KeyCode key,
                       KeyState state) {
    clientState->keyState[key] = state;
}

void _resetKeyState(WaylandClientContext *clientState) {
    memset(clientState->keyState, 0, sizeof(clientState->keyState));
}

void _registerMouseBtnState(WaylandClientContext *clientState, MouseButtons btn,
                            KeyState state) {
    clientState->mouseButtonState[btn] = state;
}

void _resetMouseBtnState(WaylandClientContext *clientState) {
    memset(clientState->mouseButtonState, 0,
           sizeof(clientState->mouseButtonState));
}
