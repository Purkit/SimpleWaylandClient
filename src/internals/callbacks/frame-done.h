#ifndef WAYLAND_SURFACE_FRAME_DONE_CALLBACK
#define WAYLAND_SURFACE_FRAME_DONE_CALLBACK

// #define VERBOSE 1
#include "../../utility.h"
#include "../../wlClientState.h"
#include <wayland-client-protocol.h>

void wl_surface_frame_done(void *data, struct wl_callback *cb, uint32_t time);

#endif // ! WAYLAND_SURFACE_FRAME_DONE_CALLBACK
