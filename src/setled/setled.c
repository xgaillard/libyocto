#include "libyocto.h"
#include "libyocto_config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LED_ALARM   "alarm"
#define LED_STATUS  "status"
#define LED_WIFI    "wifi"

#define STATE_ON    "on"
#define STATE_OFF   "off"

void exitUsage(const char *progname)
{
    //TODO Only show available leds
    printf("\
Usage: %s %s|%s|%s %s|%s\n\
",
           progname,
           LED_ALARM,
           LED_STATUS,
           LED_WIFI,
           STATE_ON,
           STATE_OFF);

    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        exitUsage(argv[0]);
    }

    int on;
    if (strcmp(argv[2], STATE_ON) == 0)
    {
        on = 1;
    }
    else if (strcmp(argv[2], STATE_OFF) == 0)
    {
        on = 0;
    }
    else
    {
        exitUsage(argv[0]);
    }

    int rc = -1;

    if (strcmp(argv[1], LED_ALARM) == 0)
    {
#ifdef LIBYOCTO_LED_ALARM
        rc = on ? yoctoLedAlarmOn() : yoctoLedAlarmOff();
#endif  //LIBYOCTO_LIBYOCTO_LED_ALARM
    }
    else if (strcmp(argv[1], LED_STATUS) == 0)
    {
#ifdef LIBYOCTO_LED_STATUS
        rc = on ? yoctoLedStatusOn() : yoctoLedStatusBlink();
#endif  //LIBYOCTO_LED_STATUS
    }
    else if (strcmp(argv[1], LED_WIFI) == 0)
    {
#ifdef LIBYOCTO_LED_WIFI
        rc = on ? yoctoLedWifiOn() : yoctoLedWifiOff();
#endif  //LIBYOCTO_LED_WIFI
    }
    else
    {
        exitUsage(argv[0]);
    }

    return rc < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}