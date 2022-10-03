#ifndef CONFIG_NETWORK_H
#define CONFIG_NETWORK_H

#include <stddef.h>

int configNetworkRead(const char *interface, char *cidrAddress, size_t cidrAddressLen, char *gateway, size_t gatewayLen);
int configNetworkWrite(const char *interface, const char *cirdAddress, const char *gateway);

#endif // CONFIG_NETWORK_H