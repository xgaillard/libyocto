#ifndef CONFIG_WIFI_H
#define CONFIG_WIFI_H

#include <stddef.h>

int configWifiRead(char *ssid, size_t ssidLen, char *passphrase, size_t passphraseLen);
int configWifiWrite(const char* ssid, const char* passphrase);

#endif // CONFIG_WIFI_H