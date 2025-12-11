#ifndef RELATIVE_POINTER_EVENT_CALLBACKS
#define RELATIVE_POINTER_EVENT_CALLBACKS

#include "../../../utility.h"
#include <wayland-util.h>

static void on_relative_motion(
    void *data, struct zwp_relative_pointer_v1 *zwp_relative_pointer_v1,
    uint32_t utime_hi, uint32_t utime_lo, wl_fixed_t dx, wl_fixed_t dy,
    wl_fixed_t dx_unaccel, wl_fixed_t dy_unaccel) {

    double _dx = wl_fixed_to_double(dx);
    double _dy = wl_fixed_to_double(dy);

    double na_dx = wl_fixed_to_double(dx_unaccel);
    double na_dy = wl_fixed_to_double(dy_unaccel);

    verbose("\nrelative pointer event: (%f %f) (%f %f)", _dx, _dy, na_dx,
            na_dy);
}

#endif // RELATIVE_POINTER_EVENT_CALLBACKS
