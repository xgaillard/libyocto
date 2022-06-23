#include "cpld_rbox.h"

#ifdef RBOX630

#include "logger.h"

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/shm.h>
#include <i2c/smbus.h>

#define CPLD_DEVICE "/dev/i2c-1"
#define CPLD_I2C_ADDRESS 0x77

#define CPLD_SHM_ADDRESS 0x3088

typedef struct
{
    __u8 flag;
    pthread_mutex_t mutex;
} ShmData;

ShmData *shmd = NULL;
int shmId;
int shmInitOk = 0;

/**
 * Creates a shared process lock.
 * \returns 0 if ok, -1 if error.
 */
static int _initShmLock(int address)
{
    int rc = 0;

    if (!shmInitOk)
    {
        shmId = shmget(address, 1 * sizeof(ShmData), IPC_CREAT | 0660);
        if (shmId == -1)
        {
            LOGGER_ERROR(errno, "");
            return -1;
        }

        shmd = shmat(shmId, NULL, 0);
        if (shmd == (void *)-1)
        {
            LOGGER_ERROR(errno, "");
            return -1;
        }

        if (shmd->flag != 1)
        {
            //Mutex not configured
            pthread_mutexattr_t mutexSharedAttr;
            if ((rc = pthread_mutexattr_init(&mutexSharedAttr)) != 0)
            {
                LOGGER_ERROR(rc, "");
            }
            else
            {
                if ((rc = pthread_mutexattr_setpshared(&mutexSharedAttr, PTHREAD_PROCESS_SHARED)) != 0)
                {
                    LOGGER_ERROR(rc, "");
                }
                else
                {
                    if ((rc = pthread_mutex_init(&shmd->mutex, &mutexSharedAttr)) != 0)
                    {
                        LOGGER_ERROR(rc, "");
                    }
                    else
                    {
                        shmd->flag = 1;

                        LOGGER(LOGGER_LEVEL_DEBUG, "Mutex configured");
                    }
                }
            }
        }
        else
        {
            LOGGER(LOGGER_LEVEL_DEBUG, "Mutex already configured");
        }

        shmdt(shmd);
        shmd = NULL;

        if (rc == 0)
        {
            shmInitOk = 1;
        }
    }

    return rc;
}

int cpldRboxOpen(int *fd)
{
    *fd = open(CPLD_DEVICE, O_RDWR);
    if (*fd == -1)
    {
        LOGGER_ERROR(errno, CPLD_DEVICE);
        return -1;
    }

    int rc = ioctl(*fd, I2C_SLAVE, CPLD_I2C_ADDRESS);
    if (rc == -1)
    {
        LOGGER_ERROR(errno, "");
    }
    else
    {
        if ((rc = _initShmLock(CPLD_SHM_ADDRESS)) == 0)
        {
            if ((shmd = shmat(shmId, NULL, 0)) == (void *)-1)
            {
                rc = -1;
                LOGGER_ERROR(errno, "");
            }
            else
            {
                if ((rc = pthread_mutex_lock(&shmd->mutex)) != 0)
                {
                    shmdt(shmd);
                    shmd = NULL;
                    LOGGER_ERROR(rc, "");
                }
            }
        }
    }

    if (rc < 0)
    {
        close(*fd);
        return -1;
    }

    return 0;
}

void cpldRboxClose(int fd)
{
    if (shmd != NULL)
    {
        int rc = pthread_mutex_unlock(&shmd->mutex);
        if (rc != 0)
        {
            LOGGER_ERROR(rc, "");
        }

        shmdt(shmd);
        shmd = NULL;

        LOGGER(LOGGER_LEVEL_DEBUG, "Detaching shm");
    }

    if (fd != -1)
    {
        LOGGER(LOGGER_LEVEL_DEBUG, "Closing cpld");
        close(fd);
    }
}

int cpldRboxSelect(int fd, __u8 bank)
{
    if ((cpldRboxWriteByte(fd, CPLD_COMMAND_BANK_SEL, bank)) == -1)
    {
        return -1;
    }

    usleep(900);

    return 0;
}

int cpldRboxWriteByte(int fd, __u8 cmd, __u8 data)
{
    __s32 rc = i2c_smbus_write_byte_data(fd, cmd, data);
    
    if (rc != 0)
    {
        LOGGER_ERROR(rc, "");
        return -1;
    }

    return 0;
}

int cpldRboxReadByte(int fd, __u8 cmd, __u8 *data)
{
    __s32 rc = i2c_smbus_read_byte_data(fd, cmd);
    
    if (rc < 0)
    {
        LOGGER_ERROR(rc, "");
        return -1;
    }

    *data = rc;
    
    return 0;
}
#endif  //RBOX630
