#ifndef XDG_TOPLEVEL_RELAETD_EVENT_LISTENER
#define XDG_TOPLEVEL_RELAETD_EVENT_LISTENER

#include "../../callbacks/xdg-events/toplevel.h"

static const struct xdg_toplevel_listener xdg_toplevel_events_listener = {
    .configure = xdg_toplevel_configure_event_handler,
    .close = xdg_toplevel_close_event_handler,
    .configure_bounds = xdg_toplevel_configure_bounds_event_handler,
    .wm_capabilities = wm_capabilities_broadcast_event_handler,
};

#endif // ! XDG_TOPLEVEL_RELAETD_EVENT_LISTENER
