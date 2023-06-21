#include "config_ssl.h"

#include "config.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

//TODO Compute sensible values
#define CONFIG_SSL_KEY_MINSIZE 1024
#define CONFIG_SSL_KEY_MAXSIZE 4096

// From: meta-fareco/recipes-extended/lighttpd/lighttpd/lighttpd.conf
// From: meta-fareco/recipes-support/stunnel/stunnel/stunnel.conf
#define CONFIG_SSL_KEY_FILENAME                 CONFIG_RWDIR_SECURE"/ssl/private/ssl.key.pem"
#define CONFIG_SSL_CERT_FILENAME                CONFIG_RWDIR_SECURE"/ssl/certs/ssl.cert.pem"

// From: meta-fareco/recipes-core/systemd/systemd-conf/journal-upload.conf.d/20-default.conf
#define CONFIG_SSL_REMOTE_JOURNAL_KEY_FILENAME  CONFIG_RWDIR_SECURE"/journal-upload/private/ssl.key.pem"
#define CONFIG_SSL_REMOTE_JOURNAL_CERT_FILENAME CONFIG_RWDIR_SECURE"/journal-upload/certs/ssl.cert.pem"

int configSslWrite(const char *privateKey, const char *certificate)
{
    assert(privateKey);
    assert(certificate);

    if (
        configCheckSize(privateKey, CONFIG_SSL_KEY_MINSIZE, CONFIG_SSL_KEY_MAXSIZE) < 0 ||
        configCheckSize(certificate, CONFIG_SSL_KEY_MINSIZE, CONFIG_SSL_KEY_MAXSIZE) < 0)
    {
        return -1;
    }

    // Copy key and cert to ssl and remoteJournal folders (owner permissions)
    //FIXME chmod/fchmodat systemd-journal
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
