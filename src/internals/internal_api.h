#ifndef INTERNAL_API_H
#define INTERNAL_API_H

#include "../keycodes.h"
#include "../wlClientState.h"
#include <xkbcommon/xkbcommon.h>

KeyCode _getOurKeyCode_from_xkb_keysym(xkb_keysym_t sym);
MouseButtonCode _getOurMouseBtnCode_from_linux_event_code(uint32_t btn);

void _registerKeyState(WaylandClientContext *clientState, KeyCode key,
                       KeyState state);
void _resetKeyState(WaylandClientContext *clientState);

void _registerMouseBtnState(WaylandClientContext *clientState,
                            MouseButtonCode btn, KeyState state);
void _resetMouseBtnState(WaylandClientContext *clientState);

void _updateMousePos(WaylandClientContext *clientState, float x, float y);

#endif // INTERNAL_API_H
