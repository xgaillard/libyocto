#include "daemon.h"
#include "logger.h"

#include <systemd/sd-daemon.h>

int daemonWatchdogNotify() 
{
    int rc = sd_notify(0, "WATCHDOG=1");

    if (rc < 0) {
        LOGGER_ERROR(rc, "Failed to ping watchdog");
        return -1;
    }

    return 0;
}
