#include "config_service.h"

#include "config.h"
#include "logger.h"

#include <stdio.h>

//From: meta-fareco/recipes-extended/radsirius/radsirius/systemd/chronyd.service.d/90-fareco.conf
#define CONFIG_SERVICE_NTP_FILENAME "/etc/fareco/services/ntp"

//From: meta-fareco/recipes-extended/radsirius/radsirius/systemd/systemd-journal-upload.service.d/90-fareco.conf
#define CONFIG_SERVICE_JOURNAL_FILENAME "/etc/fareco/services/journal"

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
        /*
    case CONFIG_SERVICE_8021X:
        return "wpa_supplicant@eth0.service";
        break;
        */
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
