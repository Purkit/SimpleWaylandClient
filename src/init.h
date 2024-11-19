#ifndef WAYLAND_CLIENT_INIT_H
#define WAYLAND_CLIENT_INIT_H

#include <EGL/egl.h>
#include <wayland-egl-backend.h>
#include <wayland-egl-core.h>
#include <wayland-egl.h>

#include <stdio.h>
// #define DEBUG 1
#define VERBOSE 1
#include "utility.h"
#include "wayland.h"

int wayland_client_initialize(WaylandClientContext *wlClientState);
int egl_create_opengl_context(WaylandClientContext *clientState);

#endif // !WAYLAND_CLIENT_INIT_H
