#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

int configNetworkRead(const char *interface, char *cidrAddress, size_t cidrAddressLen, char *gateway, size_t gatewayLen);
int configNetworkWrite(const char *interface, const char *cirdAddress, const char *gateway);

int configWifiRead(char *ssid, size_t ssidLen, char *passphrase, size_t passphraseLen);
int configWifiWrite(const char* ssid, const char* passphrase);

#endif // CONFIG_H