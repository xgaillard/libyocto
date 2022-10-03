#ifndef LOGGER_H
#define LOGGER_H

#include <stddef.h>

#define LOGGER_LEVEL_NONE 0
#define LOGGER_LEVEL_ERROR 1
#define LOGGER_LEVEL_WARNING 2
#define LOGGER_LEVEL_INFO 3
#define LOGGER_LEVEL_DEBUG 4

#define LOGGER(a, ...) logger(__FILE__, __LINE__, a, __VA_ARGS__)
#define LOGGER_ERROR(a, b) loggerErrorCode(__FILE__, __LINE__, a, b)

#define LOGGER_ERROR_GLIB(error, msg) LOGGER(LOGGER_LEVEL_ERROR, "(Glib %d) %s: %s)", (error)->code, (error)->message, msg)

void loggerSetLevel(int level);

void logger(const char *file, size_t line, int level, const char *fmt, ...);
void loggerErrorCode(const char *file, size_t line, int code, const char *msg);

#endif // LOGGER_H