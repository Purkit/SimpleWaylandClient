#include "timer_api.h"
#include <bits/time.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

uint64_t posixGetTime_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec + (uint64_t)ts.tv_nsec * 1000000000.0;
}

uint64_t posixGetTime_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000.0 + (uint64_t)ts.tv_nsec * 1000000.0;
}

uint64_t posixGetTime_us() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000.0 + (uint64_t)ts.tv_nsec * 1000.0;
}

uint64_t posixGetTime_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000.0 + (uint64_t)ts.tv_nsec;
}
