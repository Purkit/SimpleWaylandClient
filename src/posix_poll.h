#ifndef POSIX_POLL_H
#define POSIX_POLL_H

#include <errno.h>
#define __USE_GNU
#include <poll.h>
#include <signal.h>

int _posixPoll(struct pollfd *fds, nfds_t count, double *timeout);

#endif // POSIX_POLL_H
