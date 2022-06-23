#include "led.h"
#include "logger.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#define LED_SYSFS_FILENAME "/sys/class/leds/%s/trigger"
#define LED_SYSFS_FILENAME_LEN_MIN (16 + 8)

int ledSysSetTrigger(const char *card, const char *trigger)
{
    assert(card);
    assert(trigger);

    //Format led trigger path
    char ledSysFilename[LED_SYSFS_FILENAME_LEN_MIN + strlen(card) + 1];
    snprintf(ledSysFilename, sizeof(ledSysFilename), LED_SYSFS_FILENAME, card);

    //Open trigger
    int fd = open(ledSysFilename, O_RDWR);
    if (fd == -1)
    {
        LOGGER_ERROR(errno, ledSysFilename);
        return -1;
    }

    //Write trigger
    int count = strlen(trigger);
    int rc = write(fd, trigger, count);
    
    if (rc == -1)
    {
        LOGGER_ERROR(errno, ledSysFilename);
    }
    else if (rc < count)
    {
        //No enough written
        LOGGER(LOGGER_LEVEL_ERROR, "Failed writing %s: %d/%d bytes", ledSysFilename, rc, count);
        rc = -1;
    }
    else
    {
        rc = 0;
    }

    close(fd);

    return rc;
}
