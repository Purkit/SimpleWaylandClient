#include "wlClient.h"

#include <wayland-egl-backend.h>
#include <wayland-egl-core.h>
#include <wayland-egl.h>

#include <string.h>
#include <xkbcommon/xkbcommon.h>

#include "internals/listeners/registry.h"
#include "internals/listeners/xdg-events/surface.h"
#include "internals/listeners/xdg-events/toplevel.h"

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

void wayland_client_shutdown(WaylandClientContext *wlClientState) {

    eglMakeCurrent(wlClientState->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                   EGL_NO_CONTEXT);
    eglDestroyContext(wlClientState->egl_display, wlClientState->egl_context);
    eglDestroySurface(wlClientState->egl_display, wlClientState->egl_surface);
    wl_egl_window_destroy(wlClientState->egl_window);
    eglTerminate(wlClientState->egl_display);

    zxdg_toplevel_decoration_v1_destroy(wlClientState->xdg_toplevel_decoration);
    zxdg_decoration_manager_v1_destroy(wlClientState->xdg_decoration_manager);
    xdg_toplevel_destroy(wlClientState->xdg_toplevel);
    xdg_surface_destroy(wlClientState->xdg_surface);
    xdg_wm_base_destroy(wlClientState->xdg_wm_base);
    wl_surface_destroy(wlClientState->wl_surface);
    if (wlClientState->wl_keyboard != NULL) {
        wl_keyboard_release(wlClientState->wl_keyboard);
    }
    if (wlClientState->wl_pointer != NULL) {
        wl_pointer_release(wlClientState->wl_pointer);
    }
    if (wlClientState->wl_touch != NULL) {
        wl_touch_release(wlClientState->wl_touch);
    }
    wl_seat_release(wlClientState->wl_seat);
    wl_display_disconnect(wlClientState->display);
}
