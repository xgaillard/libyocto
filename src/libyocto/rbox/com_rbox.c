#include "com_rbox.h"

#ifdef RBOX630

#include "cpld_rbox.h"
#include "logger.h"

#define COM_PORT_MIN 1
#define COM_PORT_MAX 4

#define COM_TYPE_MIN COM_TYPE_RS232
#define COM_TYPE_MAX COM_TYPE_RS485_2W

#define COM_CONFIG_TYPE_MASK 0x03 //Bits 0 and 1
#define COM_CONFIG_TERM_BIT 2     //Bit 2
#define COM_CONFIG_TERM_MASK (1 << COM_CONFIG_TERM_BIT)

static int _comRboxCheckId(int id)
{
    if (id < COM_PORT_MIN || id > COM_PORT_MAX)
    {
        LOGGER(LOGGER_LEVEL_WARNING, "Bad com id=%d", id);
        return -1;
    }

    return 0;
}

static int _comRboxCheckType(__u8 type)
{
    if (type < COM_TYPE_MIN || type > COM_TYPE_MAX)
    {
        LOGGER(LOGGER_LEVEL_WARNING, "Bad com type=%d", type);
        return -1;
    }

    return 0;
}

static int _comRboxGetCmd(int id)
{
    return CPLD_COMMAND_COM + id - 1;
}

int comRboxConfigWrite(int id, __u8 type, __u8 termination)
{
    if (_comRboxCheckId(id) == -1 || _comRboxCheckType(type) == -1)
    {
        return -1;
    }

    int fd;
    int rc = cpldRboxOpen(&fd);
    if (rc == 0)
    {
        if ((rc = cpldRboxSelect(fd, CPLD_BANK_COM)) == 0)
        {
            __u8 data;
            __u8 cmd = _comRboxGetCmd(id);
            if ((rc = cpldRboxReadByte(fd, cmd, &data)) == 0)
            {
                //Erase and set bits
                data = (data & ~(COM_CONFIG_TYPE_MASK | COM_CONFIG_TERM_MASK)) | type | (termination ? COM_CONFIG_TERM_MASK : 0);

                rc = cpldRboxWriteByte(fd, cmd, data);
            }
        }

        cpldRboxClose(fd);
    }

    return rc;
}

int comRboxConfigRead(int id, __u8 *type, __u8 *termination)
{
    if (_comRboxCheckId(id) == -1)
    {
        return -1;
    }

    int fd;
    int rc = cpldRboxOpen(&fd);
    if (rc == 0)
    {
        if ((rc = cpldRboxSelect(fd, CPLD_BANK_COM)) == 0)
        {
            __u8 data;
            if ((rc = cpldRboxReadByte(fd, _comRboxGetCmd(id), &data)) == 0)
            {
                //Bit 0 and 1
                *type = data & COM_CONFIG_TYPE_MASK;

                //Bit 2
                *termination = (data >> COM_CONFIG_TERM_BIT) & 0x01;
            }
        }

        cpldRboxClose(fd);
    }

    return rc;
}
#endif  //RBOX630
