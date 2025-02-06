#ifndef WAYLAND_H
#define WAYLAND_H

#include "keycodes.h"
#include "wlClientState.h"

#include <stdio.h>
// #define DEBUG 1
#define VERBOSE 1
#include "utility.h"

int wayland_client_initialize(WaylandClientContext *wlClientState);

void wayland_poll_events(WaylandClientContext *wlClientState);
void wayland_wait_for_event(WaylandClientContext *wlClientState);
void wayland_wait_for_event_till(WaylandClientContext *wlClientState,
                                 double timeout);

int egl_create_opengl_context(WaylandClientContext *clientState);

void wayland_init_rendering(WaylandClientContext *wlClientState);

void wayland_client_shutdown(WaylandClientContext *wlClientState);

// Input polling
KeyState getKeyState(WaylandClientContext *clientState, KeyCode key);
KeyState getMouseButtonState(WaylandClientContext *clientState,
                             MouseButtons btn);

bool isKeyPressed(WaylandClientContext *clientState, KeyCode key);
bool isKeyReleased(WaylandClientContext *clientState, KeyCode key);
bool isMouseButtonPressed(WaylandClientContext *clientState, MouseButtons btn);
bool isMouseButtonReleased(WaylandClientContext *clientState, MouseButtons btn);

void getMousePos(WaylandClientContext *clientState, float *x, float *y);
float getMouseX(WaylandClientContext *clientState);
float getMouseY(WaylandClientContext *clientState);

#endif // WAYLAND_H
