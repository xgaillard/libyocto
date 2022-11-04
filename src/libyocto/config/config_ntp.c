#include "config_ntp.h"

#include "config/config.h"

#include <stdio.h>
#include <assert.h>

//From: meta-fareco/recipes-extended/libyocto/libyocto-config/chrony/fareco.conf
#define CONFIG_NTP_FILENAME "/etc/fareco/chrony.d/fareco.conf"

//From: meta-fareco/recipes-support/chrony/chrony/chrony.conf
#define CONFIG_NTP_KEYS_FILENAME "/etc/security/chrony/chrony.keys"

int configNtpWrite(const char *ip, const int keyIndex)
{
    assert(ip);

    char buffer[512];
    if (keyIndex > 0) {
        snprintf(buffer, sizeof(buffer), "key %d", keyIndex);
    } else {
        buffer[0] = '\0';
    }
    
    return configWriteFile(CONFIG_NTP_FILENAME, "server %s iburst %s", ip, buffer) >= 0 ? 0 : -1;
}

int configNtpKeysWrite(const char *content)
{
    assert(content);

    return configWriteFile(CONFIG_NTP_KEYS_FILENAME, content) >= 0 ? 0 : -1;
}
