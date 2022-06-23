#include "service.h"
#include "logger.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <systemd/sd-bus.h>

#define SERVICE_TMPFILE_PREFIX "/tmp/yocto-"
#define SERVICE_TMPFILE_PREFIX_LEN 10

#define LOGGER_ERROR_SDBUS(code, error, msg) LOGGER(LOGGER_LEVEL_ERROR, "(Systemd %d) %s: %s", code, (error)->message, msg)

//---------- Helpers ----------

static int _serviceOpenBusSystem(sd_bus **bus)
{
    int rc = sd_bus_default_system(bus);

    if (rc < 0)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "(Systemd %d) Failed to open system bus", rc);
    }

    return rc;
}

static int _serviceReadMessage(sd_bus_message *message, const char *types, void **reply)
{
    int rc = sd_bus_message_read(message, types, reply);

    if (rc < 0)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "(Systemd %d) Failed to read message", rc);
    }

    return rc;
}

//busctl call org.freedesktop.systemd1 /org/freedesktop/systemd1 org.freedesktop.systemd1.Manager GetUnit s c4s.service
static int _serviceGetUnitPath(sd_bus *bus, sd_bus_error *error, sd_bus_message **message, const char *service, const char **unitPath)
{
    int rc = sd_bus_call_method(
        bus,
        "org.freedesktop.systemd1",
        "/org/freedesktop/systemd1",
        "org.freedesktop.systemd1.Manager",
        "GetUnit",
        error,
        message,
        "s",
        service);

    if (rc < 0)
    {
        LOGGER_ERROR_SDBUS(rc, error, service);
    }
    else
    {
        if ((rc = _serviceReadMessage(*message, "o", (void *)unitPath)) >= 0)
        {
            LOGGER(LOGGER_LEVEL_DEBUG, "Unit path for service %s = %s", service, *unitPath);
        }
    }

    return rc;
}

//---------- Manage ----------

int serviceManage(const char *service, const char *action)
{
    sd_bus *bus = NULL;
    int rc = _serviceOpenBusSystem(&bus);

    if (rc >= 0)
    {
        sd_bus_error error = SD_BUS_ERROR_NULL;
        sd_bus_message *message = NULL;

        rc = sd_bus_call_method(
            bus,
            "org.freedesktop.systemd1",
            "/org/freedesktop/systemd1",
            "org.freedesktop.systemd1.Manager",
            action,
            &error,
            &message,
            "ss",
            service,
            "replace");

        if (rc < 0)
        {
            LOGGER_ERROR_SDBUS(rc, &error, service);
        }
        else
        {
            const char *reply;
            if ((rc = _serviceReadMessage(message, "o", (void *)&reply)) >= 0)
            {
                LOGGER(LOGGER_LEVEL_DEBUG, "%s successful for service %s Job=%s", action, service, reply);
            }
        }

        sd_bus_message_unref(message);
        sd_bus_error_free(&error);
    }

    sd_bus_unref(bus);

    return rc < 0 ? -1 : 0;
}

//---------- NbRestart ----------

int _serviceGetLastNbRestart(const char *unitPath, uint32_t *newValue)
{
    char filename[SERVICE_TMPFILE_PREFIX_LEN + strlen(unitPath) + 1];
    strcpy(filename, SERVICE_TMPFILE_PREFIX);

    //Check unit path
    char *unitName = strrchr(unitPath, '/');
    if (unitName == NULL)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "Bad unit path: %s", unitPath);
        return -1;
    }
    strcat(filename, unitName + 1);

    int rc = 0;
    uint32_t oldValue = 0;
    FILE *tmpFile = fopen(filename, "r");
    if (tmpFile != NULL)
    {
        //Read the old value
        if (fread(&oldValue, sizeof(oldValue), 1, tmpFile) != 1)
        {
            if (ferror(tmpFile))
            {
                LOGGER(LOGGER_LEVEL_ERROR, "Failed reading from file: %s", filename);
            }
            else
            {
                LOGGER(LOGGER_LEVEL_ERROR, "Failed reading from empty file: %s", filename);
            }
            rc = -1;
        }

        fclose(tmpFile);
    }

    //Save the new value if changed
    if (rc == 0)
    {
        uint32_t result = 0;
        if (oldValue < *newValue)
        {
            result = *newValue - oldValue;

            tmpFile = fopen(filename, "w");
            if (tmpFile == NULL)
            {
                LOGGER_ERROR(errno, filename);
                return -1;
            }

            //Save newValue
            if (fwrite(newValue, sizeof(uint32_t), 1, tmpFile) != 1)
            {
                if (ferror(tmpFile))
                {
                    LOGGER(LOGGER_LEVEL_ERROR, "Failed to write to: %s", filename);
                }
                else
                {
                    LOGGER(LOGGER_LEVEL_ERROR, "Failed to write (EOF) to: %s", filename);
                }
                rc = -1;
            }

            fclose(tmpFile);
        }

        //Return *newValue - oldValue or O if oldValue >= *newValue
        *newValue = result;
    }

    return rc < 0 ? -1 : 0;
}

//busctl get-property org.freedesktop.systemd1 /org/freedesktop/systemd1/unit/c4s_2eservice org.freedesktop.systemd1.Service NRestarts
int serviceGetNbRestart(const char *service, uint32_t *count)
{
    sd_bus *bus = NULL;
    int rc = _serviceOpenBusSystem(&bus);

    if (rc >= 0)
    {
        sd_bus_error error = SD_BUS_ERROR_NULL;
        sd_bus_message *message = NULL;
        const char *unitPath;

        if ((rc = _serviceGetUnitPath(bus, &error, &message, service, &unitPath)) >= 0)
        {
            rc = sd_bus_get_property_trivial(
                bus,
                "org.freedesktop.systemd1",
                unitPath,
                "org.freedesktop.systemd1.Service",
                "NRestarts",
                &error,
                'u',
                count);

            if (rc < 0)
            {
                LOGGER_ERROR_SDBUS(rc, &error, unitPath);
            }
            else
            {
                rc = _serviceGetLastNbRestart(unitPath, count);
            }
        }

        sd_bus_message_unref(message);
        sd_bus_error_free(&error);
    }

    sd_bus_unref(bus);

    return rc < 0 ? -1 : 0;
}

//---------- Time ----------

int _serviceSetNtp(sd_bus *bus, sd_bus_error *error, sd_bus_message **message, int enabled)
{
    int rc = sd_bus_call_method(
        bus,
        "org.freedesktop.timedate1",
        "/org/freedesktop/timedate1",
        "org.freedesktop.timedate1",
        "SetNTP",
        error,
        message,
        "bb",
        enabled,
        0);

    if (rc < 0)
    {
        LOGGER_ERROR_SDBUS(rc, error, "timedate1");
    }

    return rc;
}

int _serviceGetNtp(sd_bus *bus, sd_bus_error *error, int *enabled)
{
    int rc = sd_bus_get_property_trivial(
        bus,
        "org.freedesktop.timedate1",
        "/org/freedesktop/timedate1",
        "org.freedesktop.timedate1",
        "NTP",
        error,
        'b',
        enabled);

    if (rc < 0)
    {
        LOGGER_ERROR_SDBUS(rc, error, "timedate1");
    }

    return rc;
}

int serviceSetTime(int64_t timestamp)
{
    sd_bus *bus = NULL;
    int rc = _serviceOpenBusSystem(&bus);

    if (rc >= 0)
    {
        sd_bus_error error = SD_BUS_ERROR_NULL;
        sd_bus_message *message = NULL;
        int ntpEnabled = 0;

        if ((rc = _serviceGetNtp(bus, &error, &ntpEnabled)) >= 0)
        {
            if (ntpEnabled)
            {
                //Disable it
                rc = _serviceSetNtp(bus, &error, &message, 0);
            }

            if (rc >= 0)
            {
                //Set time
                rc = sd_bus_call_method(
                    bus,
                    "org.freedesktop.timedate1",
                    "/org/freedesktop/timedate1",
                    "org.freedesktop.timedate1",
                    "SetTime",
                    &error,
                    &message,
                    "xbb",
                    timestamp,
                    0,
                    0);

                if (rc < 0)
                {
                    LOGGER_ERROR_SDBUS(rc, &error, "timedate1");
                }
            }

            if (ntpEnabled)
            {
                //Reeanable it
                _serviceSetNtp(bus, &error, &message, 1);
            }
        }

        sd_bus_message_unref(message);
        sd_bus_error_free(&error);
    }

    sd_bus_unref(bus);

    return rc < 0 ? -1 : 0;
}
