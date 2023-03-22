#include "config_ntp.h"

#include "config/config.h"

#include <stdio.h>
#include <assert.h>

//From: meta-fareco/recipes-extended/libyocto/libyocto-config/chrony/fareco.conf
#define CONFIG_NTP_FILENAME CONFIG_RWDIR_CONF"/chrony.d/fareco.conf"

//From: meta-fareco/recipes-support/chrony/chrony/chrony.conf
#define CONFIG_NTP_KEYS_FILENAME CONFIG_RWDIR_SECURE"/chrony/chrony.keys"

int configNtpWrite(const char *ip, const int keyIndex)
{
    assert(ip);

    int rc;

    if (keyIndex > 0) {
        rc = configWriteFile(CONFIG_NTP_FILENAME, "server %s iburst key %d", ip, keyIndex);
    } else {
        rc = configWriteFile(CONFIG_NTP_FILENAME, "server %s iburst", ip);
    }

    return rc >= 0 ? 0 : -1;
}

int configNtpKeysWrite(const char *content)
{
    assert(content);

    return configWriteFile(CONFIG_NTP_KEYS_FILENAME, content) >= 0 ? 0 : -1;
}
