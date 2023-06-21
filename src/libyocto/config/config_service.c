#include "config_service.h"

#include "config.h"
#include "logger.h"

#include <stdio.h>

//From: meta-fareco/recipes-extended/libyocto/libyocto-config/systemd/chronyd.service.d/90-fareco.conf
#define CONFIG_SERVICE_NTP_FILENAME     CONFIG_RWDIR_CONF"/services/ntp"

//From: meta-fareco/recipes-extended/libyocto/libyocto-config/systemd/systemd-journal-upload.service.d/90-fareco.conf
#define CONFIG_SERVICE_JOURNAL_FILENAME CONFIG_RWDIR_CONF"/services/journal-upload"

//From: meta-fareco/recipes-extended/libyocto/libyocto-config/systemd/wpa_supplicant-wired@.service.d/90-fareco.conf
#define CONFIG_SERVICE_8021X_FILENAME   CONFIG_RWDIR_CONF"/services/wpa-wired"

static const char *_serviceName(ConfigService type)
{
    switch (type)
    {
    case CONFIG_SERVICE_NTP:
        return CONFIG_SERVICE_NTP_FILENAME;
        break;
    case CONFIG_SERVICE_JOURNAL:
        return CONFIG_SERVICE_JOURNAL_FILENAME;
        break;
    case CONFIG_SERVICE_8021X:
        return CONFIG_SERVICE_8021X_FILENAME;
        break;
    default:
        LOGGER(LOGGER_LEVEL_ERROR, "Unknow service id: %d)", type);
        break;
    }

    return NULL;
}

int configServiceEnable(ConfigService type, int enable)
{
    if (enable == 0) {
        return configDeleteFile(_serviceName(type));
    }

    return configWriteFile(_serviceName(type), "");
}

int configServiceIsEnabled(ConfigService type)
{
    return configIsFile(_serviceName(type));
}
