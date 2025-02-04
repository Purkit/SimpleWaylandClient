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
int getKeyState(KeyCode key);
int getMouseButtonState(MouseButtons btn);

int isKeyPressed(KeyCode key);
int isKeyReleased(KeyCode key);
int isMouseButtonPressed(MouseButtons btn);
int isMouseButtonReleased(MouseButtons btn);

void getMousePos(float *x, float *y);
float getMouseX();
float getMouseY();

#endif // WAYLAND_H
