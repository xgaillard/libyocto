#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

int configCheckSize(const char* data, size_t min, size_t max);

int configWriteFile(const char *filename, const char *format, ...);

int configCopyTmpFile(int tmpFileDescriptor, const char *tmpFilename, const char *destFilename);

#endif // CONFIG_H