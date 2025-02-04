#include "posix_poll.h"
#include "timer/timer_api.h"
#include <stdint.h>
#include <sys/poll.h>
#include <time.h>

int _posixPoll(struct pollfd *fds, nfds_t count, double *timeout) {
    while (1) {
        if (timeout) {
            const uint64_t start = posixGetTime_ns();

            const uint64_t seconds   = (time_t)*timeout;
            const long nanoseconds   = (long)((*timeout - seconds) * 1e9);
            const struct timespec ts = {seconds, nanoseconds};

            const int result = ppoll(fds, count, &ts, NULL);

            const int poll_err = errno;

            *timeout = *timeout - (posixGetTime_ns() - start) / 1000000000.0;

            if (result > 0)
                return 1;
            else if (result == -1 && errno != EINTR && errno != EAGAIN)
                return 0; // for errno = EINTR and EAGAIN we retry
            else if (*timeout <= 0.0)
                return 0;
        } else { // if timeout is = NULL
            const int result = poll(fds, count, -1);
            if (result > 0)
                return 1;
            else if (result == -1 && errno != EINTR && errno != EAGAIN)
                return 0; // for errno = EINTR and EAGAIN we retry
        }
    }
}
