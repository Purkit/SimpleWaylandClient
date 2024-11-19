#include "init.h"
#include "protocols/xdg-decoration-protocol.h"
#include "protocols/xdg-shell-client-protocol.h"
#include <string.h>
#include <xkbcommon/xkbcommon.h>

#include "input.h"
#include "utility.h"
#include "wayland.h"

static void xdg_wm_base_ping_handler(void *data,
                                     struct xdg_wm_base *xdg_wm_base,
                                     uint32_t serial) {
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping_handler,
};

static void xdg_surface_configure_event_handler(void *data,
                                                struct xdg_surface *xdg_surface,
                                                uint32_t serial) {
    verbose("xdg_surface::configure event recieved!\n");
    struct WaylandClientContext *state = data;
    xdg_surface_ack_configure(xdg_surface, serial);

    wl_surface_commit(state->wl_surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure_event_handler,
};

static void xdg_toplevel_configure_event_handler(
    void *data, struct xdg_toplevel *xdg_toplevel, int32_t width,
    int32_t height, struct wl_array *states) {
    verbose("xdg_toplevel::configure event recieved!\n");
    struct WaylandClientContext *state = data;
    state->width = width;
    state->height = height;
    // on_resize(state);
    // state->on_resize_callback(state);
}

static void
xdg_toplevel_close_event_handler(void *data,
                                 struct xdg_toplevel *xdg_toplevel) {
    verbose("close button clicked!\n");
    struct WaylandClientContext *state = data;
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

static const struct xdg_toplevel_listener xdg_toplevel_events_listener = {
    .configure = xdg_toplevel_configure_event_handler,
    .close = xdg_toplevel_close_event_handler,
    .configure_bounds = xdg_toplevel_configure_bounds_event_handler,
    .wm_capabilities = wm_capabilities_broadcast_event_handler,
};

static void registry_global_event_handler(void *data,
                                          struct wl_registry *registry,
                                          uint32_t name,
                                          const char *interface_string,
                                          uint32_t version) {
    WaylandClientContext *state = (WaylandClientContext *)data;
    if (strcmp(interface_string, wl_compositor_interface.name) == 0) {
        state->compositor =
            wl_registry_bind(registry, name, &wl_compositor_interface, 6);
        verbose("got the compositor !!\n");
        if (state->compositor == NULL) {
            verbose("state->compositor == NULL\n");
        }
    } else if (strcmp(interface_string, wl_shm_interface.name) == 0) {
        state->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (strcmp(interface_string, xdg_wm_base_interface.name) == 0) {
        state->xdg_wm_base =
            wl_registry_bind(registry, name, &xdg_wm_base_interface, 6);
        xdg_wm_base_add_listener(state->xdg_wm_base, &xdg_wm_base_listener,
                                 state);
    } else if (strcmp(interface_string, wl_seat_interface.name) == 0) {
        state->wl_seat =
            wl_registry_bind(registry, name, &wl_seat_interface, 9);
        wl_seat_add_listener(state->wl_seat, &wl_seat_listener, state);
    } else if (strcmp(interface_string,
                      zxdg_decoration_manager_v1_interface.name) == 0) {
        state->xdg_decoration_manager = wl_registry_bind(
            registry, name, &zxdg_decoration_manager_v1_interface, 1);
    }
}

static void registry_global_remove(void *data, struct wl_registry *registry,
                                   uint32_t name) {
    // handle this
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_global_event_handler,
    .global_remove = registry_global_remove,
};

int wayland_client_initialize(WaylandClientContext *wlClientState) {
    wlClientState->display = wl_display_connect(NULL);
    if (!wlClientState->display) {
        verbose("Failed to connect to the wayland compositor!\n");
        return 0;
    }
    wlClientState->registry = wl_display_get_registry(wlClientState->display);
    wlClientState->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    wl_registry_add_listener(wlClientState->registry, &registry_listener,
                             wlClientState);
    wl_display_roundtrip(wlClientState->display);
    wlClientState->wl_surface =
        wl_compositor_create_surface(wlClientState->compositor);
    wlClientState->xdg_surface = xdg_wm_base_get_xdg_surface(
        wlClientState->xdg_wm_base, wlClientState->wl_surface);
    xdg_surface_add_listener(wlClientState->xdg_surface, &xdg_surface_listener,
                             wlClientState);
    wlClientState->xdg_toplevel =
        xdg_surface_get_toplevel(wlClientState->xdg_surface);
    xdg_toplevel_add_listener(wlClientState->xdg_toplevel,
                              &xdg_toplevel_events_listener, wlClientState);
    xdg_toplevel_set_title(wlClientState->xdg_toplevel, "hello wayland new");
    wlClientState->xdg_toplevel_decoration =
        zxdg_decoration_manager_v1_get_toplevel_decoration(
            wlClientState->xdg_decoration_manager, wlClientState->xdg_toplevel);
    wl_surface_commit(wlClientState->wl_surface);
    return 1;
}

static const EGLint egl_attrib_list[] = {
    EGL_RED_SIZE,
    8,
    EGL_GREEN_SIZE,
    8,
    EGL_BLUE_SIZE,
    8,
    EGL_ALPHA_SIZE,
    8,
    EGL_BUFFER_SIZE,
    32,
    EGL_DEPTH_SIZE,
    0,
    EGL_STENCIL_SIZE,
    0,
    EGL_SAMPLES,
    0,
    EGL_SURFACE_TYPE,
    EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE,
    EGL_OPENGL_BIT,
    EGL_CONFIG_CAVEAT,
    EGL_NONE,
    EGL_MAX_SWAP_INTERVAL,
    1,
    EGL_NONE,
};

int egl_create_opengl_context(WaylandClientContext *clientState) {

    // Step 1: Initialize egl
    clientState->egl_display =
        eglGetDisplay((EGLNativeDisplayType)clientState->display);
    EGLint egl_major, egl_minor;
    eglInitialize(clientState->egl_display, &egl_major, &egl_minor);

    // Step 2: Select an appropriate configuration
    EGLint no_of_matching_configs;
    if (!eglChooseConfig(clientState->egl_display, egl_attrib_list,
                         &clientState->egl_config, 1,
                         &no_of_matching_configs)) {
        ERROR("failed to get valid egl config!\n");
    }
    if (no_of_matching_configs == 0) {
        ERROR("No matching egl config found!");
        ERROR("Failed to obtain OpenGL context!");
        return -1;
    }

    // Step 3: Create a EGL Surface
    clientState->egl_window = wl_egl_window_create(
        clientState->wl_surface, clientState->width, clientState->height);
    const EGLint egl_surface_attrib_list[] = {
        EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_LINEAR,
        EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
        EGL_NONE,
    };
    clientState->egl_surface = eglCreateWindowSurface(
        clientState->egl_display, clientState->egl_config,
        (EGLNativeWindowType)clientState->egl_window, egl_surface_attrib_list);

    // Step 4: Bind the OpenGL API
    eglBindAPI(EGL_OPENGL_API);

    // Step 5: Create a OpenGL Context and make it current
    const EGLint opengl_context_attribs[] = {
        EGL_CONTEXT_MAJOR_VERSION,
        4,
        EGL_CONTEXT_MINOR_VERSION,
        6,
        EGL_CONTEXT_OPENGL_PROFILE_MASK,
        EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
        EGL_NONE,
    };
    clientState->egl_context =
        eglCreateContext(clientState->egl_display, clientState->egl_config,
                         EGL_NO_CONTEXT, opengl_context_attribs);
    eglMakeCurrent(clientState->egl_display, clientState->egl_surface,
                   clientState->egl_surface, clientState->egl_context);

    return 1;
}
