#include "led_rbox.h"

#ifdef RBOX630

#include "cpld_rbox.h"
#include "logger.h"

#define LED_RBOX_MAX 7

int ledRboxSet(__u8 led, __u8 on)
{
    if (led > LED_RBOX_MAX)
    {
        LOGGER(LOGGER_LEVEL_WARNING, "Bad led id (%d)", led);
        return -1;
    }

    int fd;
    int rc;

    if ((rc = cpldRboxOpen(&fd)) == 0)
    {
        if ((rc = cpldRboxSelect(fd, CPLD_BANK_SYS)) == 0)
        {
            __u8 data;
            if ((rc = cpldRboxReadByte(fd, CPLD_COMMAND_LED, &data)) == 0)
            {
                if (on)
                {
                    data &= ~(1 << led);
                }
                else
                {
                    data |= 1 << led;
                }

                rc = cpldRboxWriteByte(fd, CPLD_COMMAND_LED, data);
            }
        }

        cpldRboxClose(fd);
    }

    return rc;
}
#endif  //RBOX630