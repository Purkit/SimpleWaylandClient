#ifndef WAYLAND_H
#define WAYLAND_H

#include "keycodes.h"
#include "wlClientState.h"

#include <stdint.h>
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
                             MouseButtonCode btn);

bool isKeyPressed(WaylandClientContext *clientState, KeyCode key);
bool isKeyReleased(WaylandClientContext *clientState, KeyCode key);
bool isMouseButtonPressed(WaylandClientContext *clientState,
                          MouseButtonCode btn);
bool isMouseButtonReleased(WaylandClientContext *clientState,
                           MouseButtonCode btn);

void getMousePos(WaylandClientContext *clientState, float *x, float *y);
float getMouseX(WaylandClientContext *clientState);
float getMouseY(WaylandClientContext *clientState);

// Register callbacks
void setKeyEventCallback(WaylandClientContext *clientState);

void set_window_title(WaylandClientContext *clientState, const char *title);
void maximize_window(WaylandClientContext *clientState);
void unmaximize_window(WaylandClientContext *clientState);
void minimize_window(WaylandClientContext *clientState);
void set_maximum_size(WaylandClientContext *clientState, int32_t width,
                      int32_t height);
void set_minimum_size(WaylandClientContext *clientState, int32_t width,
                      int32_t height);
void make_fullscreen(WaylandClientContext *clientState);
void undo_fullscreen(WaylandClientContext *clientState);

#endif // WAYLAND_H
