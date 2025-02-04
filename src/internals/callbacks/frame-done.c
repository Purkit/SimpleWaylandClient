#include "frame-done.h"

struct wl_callback_listener wl_surface_frame_listener = {
    .done = wl_surface_frame_done,
};

void wl_surface_frame_done(void *data, struct wl_callback *cb, uint32_t time) {
    if (cb) {
        wl_callback_destroy(cb);
    }
    struct WaylandClientContext *state = (struct WaylandClientContext *)data;
    state->redraw_signal_callback      = wl_surface_frame(state->wl_surface);
    wl_callback_add_listener(state->redraw_signal_callback,
                             &wl_surface_frame_listener, state);

    float time_secs = time * 1e-3f;
    float elapsed   = time_secs - state->last_frame;
    float fps       = 1.0f / elapsed;
    verbose("dt=%f, fps=%f\n", elapsed, fps);

    state->render(state, time_secs);
    wl_surface_commit(state->wl_surface);
    state->last_frame = time_secs;
}
