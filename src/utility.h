#ifndef LOG_H
#define LOG_H

#ifdef DEBUG
#define debug(fmt, ...)                                                        \
    fprintf(stderr, "[%s:%d]", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif // DEBUG

#ifdef VERBOSE
#define verbose(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define verbose(fmt, ...)
#endif // VERBOSE

#endif // LOG_H
