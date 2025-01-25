#ifndef XDG_TOPLEVEL_RELATED_EVENT_CALLBACKS
#define XDG_TOPLEVEL_RELATED_EVENT_CALLBACKS

#include "../../../wlClientState.h"
#include "../../protocols/xdg-shell-client-protocol.h"

#define VERBOSE 1
#include "../../../utility.h"

static void xdg_toplevel_configure_event_handler(
    void *data, struct xdg_toplevel *xdg_toplevel, int32_t width,
    int32_t height, struct wl_array *states) {
    verbose("xdg_toplevel::configure event recieved!\n");
    struct WaylandClientContext *state = (struct WaylandClientContext *)data;
    state->width = width;
    state->height = height;
    // on_resize(state);
    state->resize(state);
}

static void
xdg_toplevel_close_event_handler(void *data,
                                 struct xdg_toplevel *xdg_toplevel) {
    verbose("close button clicked!\n");
    struct WaylandClientContext *state = (struct WaylandClientContext *)data;
    state->shouldClose = true;
}

static void
wm_capabilities_broadcast_event_handler(void *data,
                                        struct xdg_toplevel *xdg_toplevel,
                                        struct wl_array *capabilities) {
    // Process the list of cabapilities supported by the compositor
    // and enable or disable the equivalent UI features.
    verbose("xdg_toplevel::wm_capabilities_broadcast event fired!\n");
}

static void
xdg_toplevel_configure_bounds_event_handler(void *data,
                                            struct xdg_toplevel *xdg_toplevel,
                                            int32_t width, int32_t height) {
    // TODO: handle later !
}

#endif // ! XDG_TOPLEVEL_RELATED_EVENT_CALLBACKS
