#ifndef XDG_TOPLEVEL_RELATED_EVENT_CALLBACKS
#define XDG_TOPLEVEL_RELATED_EVENT_CALLBACKS

#include "../../../wlClientState.h"
#include "../../protocols/xdg-shell-client-protocol.h"

#define VERBOSE 1
#include "../../../utility.h"

static void xdg_toplevel_configure_event_handler(
    void *data, struct xdg_toplevel *xdg_toplevel, int32_t width,
    int32_t height, struct wl_array *states) {

    verbose("xdg toplevel configure called\n");

    struct WaylandClientContext *state = (struct WaylandClientContext *)data;

    /*state->pending.activated = false;*/
    /*state->pending.maximized = false;*/
    /**/
    /*enum xdg_toplevel_state *st;*/
    /*for (st = (enum xdg_toplevel_state *)(states)->data;*/
    /*     (states)->size != 0 &&*/
    /*     (const char *)st < ((const char *)(states)->data + (states)->size);*/
    /*     (st)++) {*/
    /*    if (*st == XDG_TOPLEVEL_STATE_MAXIMIZED) {*/
    /*        verbose("xdg toplevel MAXIMIZED recieved !!\n");*/
    /*        state->pending.maximized = true;*/
    /*    }*/
    /*    if (*st == XDG_TOPLEVEL_STATE_FULLSCREEN)*/
    /*        state->fullscreen = true;*/
    /*    if (*st == XDG_TOPLEVEL_STATE_ACTIVATED) {*/
    /*        verbose("xdg toplevel ACTIVATED recieved !!\n");*/
    /*        state->pending.activated = true;*/
    /*    }*/
    /*    if (*st == XDG_TOPLEVEL_STATE_SUSPENDED) {*/
    /*        verbose("xdg toplevel SUSPENDED recieved !!\n");*/
    /*    }*/
    /*    if (*st == XDG_TOPLEVEL_STATE_RESIZING) {*/
    /*        verbose("xdg toplevel RESIZING recieved !!\n");*/
    /*    }*/
    /*}*/

    if (width && height) {
        state->width  = width;
        state->height = height;

        if (state->callbacks.resize)
            state->callbacks.resize(state);
    }
}

static void
xdg_toplevel_close_event_handler(void *data,
                                 struct xdg_toplevel *xdg_toplevel) {
    verbose("\nclose button clicked!");
    struct WaylandClientContext *state = (struct WaylandClientContext *)data;
    state->shouldClose                 = true;
    if (state->callbacks.window_close) {
        state->callbacks.window_close(state);
    }
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
