#include "config_remotejournal.h"

#include "config.h"

#include <assert.h>

// TODO Compute sensible values
#define CONFIG_REMOTE_JOURNAL_CERT_MINSIZE 1024
#define CONFIG_REMOTE_JOURNAL_CERT_MAXSIZE 4096

//From: meta-fareco/recipes-core/systemd/systemd-conf/journal-upload.conf.d/20-default.conf
#define CONFIG_REMOTE_JOURNAL_CERT_FILENAME "/etc/security/journal-upload/certs/ca.cert.pem"

//From: meta-fareco/recipes-core/systemd/systemd-conf/journal-upload.conf.d/10-url.conf
#define CONFIG_REMOTE_JOURNAL_CONF_FILENAME "/etc/systemd/journal-upload.conf.d/10-url.conf"

int configRemoteJournalWrite(const char *ip, const char *certificate)
{
    assert(ip || certificate);

    if (ip != NULL)
    {
        if (
            configCheckIpV4(ip) < 0 ||
            configWriteFile(CONFIG_REMOTE_JOURNAL_CONF_FILENAME, "[Upload]\nURL=https://%s", ip) < 0)
        {
            return -1;
        }
    }

    if (certificate != NULL)
    {
        if (
            configCheckSize(certificate, CONFIG_REMOTE_JOURNAL_CERT_MINSIZE, CONFIG_REMOTE_JOURNAL_CERT_MAXSIZE) < 0 ||
            configWriteFile(CONFIG_REMOTE_JOURNAL_CERT_FILENAME, certificate) < 0)
        {
            return -2;
        }
    }

    return 0;
}
