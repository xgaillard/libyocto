#include "config_snmp.h"
#include "logger.h"
#include "libyocto_config.h"

#include "config.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#define CONFIG_SNMP_PASSWORD_MINSIZE 8
#define CONFIG_SNMP_PASSWORD_MAXSIZE 32

#define CONFIG_SNMP_FILENAME_SECURITY "/etc/fareco/net-snmp/snmpd.conf"
#define CONFIG_SNMP_FILENAME_USER "/etc/fareco/net-snmp/users.conf"

int configSnmpWrite(const char *user, const char *auth, const char *priv, const char *view)
{
    assert(user);
    assert(auth);
    assert(priv);
    assert(view);

    // TODO More checks
    if (
        configCheckSize(auth, CONFIG_SNMP_PASSWORD_MINSIZE, CONFIG_SNMP_PASSWORD_MAXSIZE) < 0 ||
        configCheckSize(priv, CONFIG_SNMP_PASSWORD_MINSIZE, CONFIG_SNMP_PASSWORD_MAXSIZE))
    {
        LOGGER(LOGGER_LEVEL_ERROR, "Password too short");
        return -1;
    }

    if (
        configWriteFile(CONFIG_SNMP_FILENAME_SECURITY, "createuser '%s' SHA-256 '%s' DES '%s'", user, auth, priv) < 0 ||
        configWriteFile(CONFIG_SNMP_FILENAME_USER, "rouser '%s' authpriv -V '%s'", user, view) < 0)
    {
        return -1;
    }

    return 0;
}
