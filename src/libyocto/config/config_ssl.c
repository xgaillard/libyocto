#include "config_ssl.h"

#include "config.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

// TODO Compute sensible values
#define CONFIG_SSL_KEY_MINSIZE 1024
#define CONFIG_SSL_KEY_MAXSIZE 4096

#define CONFIG_SSL_FILENAME_KEY "/etc/security/ssl/private/ssl.key.pem"
#define CONFIG_SSL_FILENAME_CERT "/etc/security/ssl/certs/ssl.cert.pem"

int configSslWrite(const char *primaryKey, const char *certificate)
{
    assert(primaryKey);
    assert(certificate);

    if (
        configCheckSize(primaryKey, CONFIG_SSL_KEY_MINSIZE, CONFIG_SSL_KEY_MAXSIZE) < 0 ||
        configCheckSize(certificate, CONFIG_SSL_KEY_MINSIZE, CONFIG_SSL_KEY_MAXSIZE) < 0)
    {

        return -1;
    }

   if (
        configWriteFile(CONFIG_SSL_FILENAME_KEY, primaryKey) < 0 ||
        configWriteFile(CONFIG_SSL_FILENAME_CERT, certificate) < 0)
    {
        return -1;
    }

    return 0;
}
