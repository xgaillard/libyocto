#ifndef CONFIG_NTP_H
#define CONFIG_NTP_H

int configNtpWrite(const char *ip, const int keyIndex);

int configNtpKeysWrite(const char *content);

#endif // CONFIG_NTP_H