#include "config.h"

#include "logger.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>

int configCheckSize(const char *data, size_t min, size_t max)
{
    size_t length = strlen(data);

    if (length >= min && (max == 0 || length <= max))
    {
        return 0;
    }

    LOGGER(LOGGER_LEVEL_ERROR, "String is the wrong size: %u <= %u <= %u)", min, length, max);
    return -1;
}

int configWriteFile(const char *filename, const char *format, ...)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL)
    {
        LOGGER_ERROR(errno, filename);
        return -1;
    }

    int rc = 0;

    va_list args;
    va_start(args, format);
    rc = vfprintf(file, format, args);
    va_end(args);

    if (rc < 0)
    {
        if (ferror(file))
        {
            LOGGER(LOGGER_LEVEL_ERROR, "Failed to write to: %s", filename);
        }
        else
        {
            LOGGER_ERROR(errno, filename);
        }
    }

    fclose(file);
    return rc;
}

int configCopyTmpFile(int tmpFileDescriptor, const char *tmpFilename, const char *destFilename)
{
    // Reset tmp file
    if ((lseek(tmpFileDescriptor, 0, SEEK_SET)) < 0)
    {
        LOGGER_ERROR(errno, tmpFilename);
        return -1;
    }

    // Open destination
    FILE *destFile = fopen(destFilename, "w");

    if (destFile == NULL)
    {
        LOGGER_ERROR(errno, destFilename);
        return -1;
    }

    // Read tmp file and write to destination
    int rc = 0;
    char buffer[256];
    ssize_t count;

    while ((count = read(tmpFileDescriptor, buffer, sizeof(buffer))) > 0)
    {
        if (fwrite(buffer, count, 1, destFile) != 1)
        {
            if (ferror(destFile))
            {
                rc = -1;
                LOGGER(LOGGER_LEVEL_ERROR, destFilename);
                break;
            }
        }
    }

    if (count < 0)
    {
        rc = -1;
        LOGGER_ERROR(errno, tmpFilename);
    }

    fclose(destFile);

    return rc;
}
