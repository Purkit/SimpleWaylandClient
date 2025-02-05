#ifndef TIMER_API_H
#define TIMER_API_H

#include <stdint.h>

double posixGetTime_sec();
uint64_t posixGetTime_ms();
uint64_t posixGetTime_us();
uint64_t posixGetTime_ns();

#endif // TIMER_API_H
