
#include "gpio_rbox.h"

#ifdef RBOX630

#include "cpld_rbox.h"
#include "logger.h"

#define GPIO_RBOX_ID_MAX   7

int gpioRboxReadAll(__u8 *data)
{
    int fd;
    int rc = cpldRboxOpen(&fd);

    if (rc == 0)
    {
        if ((rc = cpldRboxSelect(fd, CPLD_BANK_GPO)) == 0)
        {
            if ((rc = cpldRboxReadByte(fd, CPLD_COMMAND_GPO, data)) == 0)
            {
                *data = ~*data;
            }
        }

        cpldRboxClose(fd);
    }

    return rc;
}

int gpioRboxRead(int id, __u8 *on)
{
    if (id < 0 || id > GPIO_RBOX_ID_MAX)
    {
        LOGGER(LOGGER_LEVEL_WARNING, "Bad gpio id=%d", id);
        return -1;
    }

    int rc = gpioRboxReadAll(on);
    
    if (rc == 0)
    {
        *on = (*on >> id) & 0x01;
    }

    return rc;
}

int gpioRboxWriteAll(__u8 data)
{
    int fd;
    int rc = cpldRboxOpen(&fd);

    if (rc == 0)
    {
        if ((rc = cpldRboxSelect(fd, CPLD_BANK_GPO)) == 0)
        {
            data = ~data;
            rc = cpldRboxWriteByte(fd, CPLD_COMMAND_GPO, data);
        }

        cpldRboxClose(fd);
    }

    return rc;
}

int gpioRboxWrite(int id, __u8 on)
{
    if (id < 0 || id > GPIO_RBOX_ID_MAX)
    {
        LOGGER(LOGGER_LEVEL_WARNING, "Bad gpio id=%d", id);
        return -1;
    }

    int fd;
    int rc = cpldRboxOpen(&fd);

    if (rc == 0)
    {
        if ((rc = cpldRboxSelect(fd, CPLD_BANK_GPO)) == 0)
        {
            __u8 data;
            if ((rc = cpldRboxReadByte(fd, CPLD_COMMAND_GPO, &data)) == 0)
            {
                data = ~data;

                if (on)
                {
                    data |= 1 << id;
                }
                else
                {
                    data &= ~(1 << id);
                }

                data = ~data;

                rc = cpldRboxWriteByte(fd, CPLD_COMMAND_GPO, data);
            }
        }

        cpldRboxClose(fd);
    }

    return rc;
}
#endif  //RBOX630