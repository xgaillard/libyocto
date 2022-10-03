#ifndef CONFIG_H
#define CONFIG_H

int configCopyTmpFile(int tmpFileDescriptor, const char *tmpFilename, const char *destFilename);

int configWriteFile(const char *filename, const char *format, ...);

#endif // CONFIG_H