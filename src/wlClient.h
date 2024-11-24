#ifndef WAYLAND_H
#define WAYLAND_H

#include "wlClientState.h"

#include <stdio.h>
// #define DEBUG 1
#define VERBOSE 1
#include "utility.h"

int wayland_client_initialize(WaylandClientContext *wlClientState);
int egl_create_opengl_context(WaylandClientContext *clientState);

void wayland_client_shutdown(WaylandClientContext *wlClientState);

#endif // WAYLAND_H
