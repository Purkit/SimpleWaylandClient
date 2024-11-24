#ifndef XDG_SURFACE_RELATED_EVENT_LISTENERS
#define XDG_SURFACE_RELATED_EVENT_LISTENERS

#include "../../callbacks/xdg-events/surface.h"

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure_event_handler,
};

#endif // ! XDG_SURFACE_RELATED_EVENT_LISTENERS
