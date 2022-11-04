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

// From: meta-fareco/recipes-extended/lighttpd/lighttpd/lighttpd.conf
// From: meta-fareco/recipes-support/stunnel/stunnel/stunnel.conf
#define CONFIG_SSL_KEY_FILENAME "/etc/security/ssl/private/ssl.key.pem"
#define CONFIG_SSL_CERT_FILENAME "/etc/security/ssl/certs/ssl.cert.pem"

#define CONFIG_SSL_REMOTE_JOURNAL_KEY_FILENAME "/etc/security/journal-upload/private/ssl.key.pem"
#define CONFIG_SSL_REMOTE_JOURNAL_CERT_FILENAME "/etc/security/journal-upload/certs/ssl.cert.pem"

int configSslWrite(const char *privateKey, const char *certificate)
{
    assert(privateKey);
    assert(certificate);

    // FIXME COPY to journal also
    if (
        configCheckSize(privateKey, CONFIG_SSL_KEY_MINSIZE, CONFIG_SSL_KEY_MAXSIZE) < 0 ||
        configCheckSize(certificate, CONFIG_SSL_KEY_MINSIZE, CONFIG_SSL_KEY_MAXSIZE) < 0)
    {
        return -1;
    }

    //Copy key and cert to ssl and remoteJournal folders (owner permissions)
    if (
        configWriteFile(CONFIG_SSL_KEY_FILENAME, privateKey) < 0 ||
        configWriteFile(CONFIG_SSL_CERT_FILENAME, certificate) < 0 ||
        configWriteFile(CONFIG_SSL_REMOTE_JOURNAL_KEY_FILENAME, privateKey) < 0 ||
        configWriteFile(CONFIG_SSL_REMOTE_JOURNAL_CERT_FILENAME, certificate) < 0)
    {
        return -1;
    }

    return 0;
}
