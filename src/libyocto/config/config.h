#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

int configCheckSize(const char* data, size_t min, size_t max);
int configCheckIpV4(const char* ip);

int configWriteFile(const char *filename, const char *format, ...);
int configDeleteFile(const char *filename);
int configIsFile(const char *filename);

int configCopyTmpFile(int tmpFileDescriptor, const char *tmpFilename, const char *destFilename);

#endif // CONFIG_H