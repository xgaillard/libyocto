
#include "logger.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define LOGGER_ERROR_MESSAGE "Error: "
#define LOGGER_ERROR_MESSAGE_LEN (7 + 1)

int loggerLevel = LOGGER_LEVEL_ERROR;

void logger(const char* file, size_t line, int level, const char *fmt, ...)
{
    if (level > loggerLevel)
    {
        return;
    }

    char buffer[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    switch (level)
    {
    case LOGGER_LEVEL_DEBUG:
        printf("[DEBUG] %s\n", buffer);
        fflush(stdout);
        break;
    case LOGGER_LEVEL_INFO:
        printf("[INFO] %s\n", buffer);
        fflush(stdout);
        break;
    case LOGGER_LEVEL_WARNING:
        printf("[WARNING] %s\n", buffer);
        fflush(stdout);
        break;
    case LOGGER_LEVEL_ERROR:
        fprintf(stderr, "[ERROR] %s\n  file: %s @%lu\n", buffer, file, (unsigned long)line);
        fflush(stderr);
        break;
    }
}

void loggerErrorCode(const char* file, size_t line, int code, const char* msg) {
    logger(file, line, LOGGER_LEVEL_ERROR, "(%d) %s: %s", code, strerror(code), msg);
}

void loggerSetLevel(int level)
{
    loggerLevel = level;
}
