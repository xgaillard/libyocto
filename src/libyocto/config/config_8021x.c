#include "config_8021x.h"

#include "config/config.h"

#include <assert.h>
#include <stdio.h>

// From: meta-fareco/recipes-extended/libyocto/libyocto-config/wpa_supplicant/wpa_supplicant-wired-eth0.conf
#define CONFIG_8021X_WPA_FILENAME   CONFIG_RWDIR_CONF"/wpa_supplicant/wpa_supplicant-wired-eth0.conf"

#define CONFIG_8021X_KEY_FILENAME   CONFIG_RWDIR_SECURE"/radius/private/radius.key.pem"
#define CONFIG_8021X_CERT_FILENAME  CONFIG_RWDIR_SECURE"/radius/certs/radius.cert.pem"
#define CONFIG_8021X_CA_FILENAME    CONFIG_RWDIR_SECURE"/radius/certs/ca.cert.pem"

#define CONFIG_RADIUS_CONTENT "\
ctrl_interface=/var/run/wpa_supplicant\n\
ctrl_interface_group=0\n\
\n\
eapol_version=2\n\
ap_scan=0\n\
\n\
network={\n\
    key_mgmt=IEEE8021X\n\
    eap=TLS\n\
    identity=\"%s\"\n\
    private_key=\"" CONFIG_8021X_KEY_FILENAME "\"\n\
    client_cert=\"" CONFIG_8021X_CERT_FILENAME "\"\n\
%s%s\
}"

#define CONFIG_RADIUS_PASSWORD "\
    private_key_passwd=\"%s\"\n\
"

#define CONFIG_RADIUS_CA "\
    ca_cert=\""CONFIG_8021X_CA_FILENAME"\"\n\
"

#define CONFIG_8021X_KEY_MINSIZE 1024
#define CONFIG_8021X_KEY_MAXSIZE 4096
#define CONFIG_8021X_CERT_MINSIZE 1024
#define CONFIG_8021X_CERT_MAXSIZE 4096
#define CONFIG_8021X_CA_MINSIZE 1024
#define CONFIG_8021X_CA_MAXSIZE 4096

int config8021xWrite(const char *identity, const char *privateKey, const char *privateKeyPassword, const char *certificate, const char *certificateAuthority)
{
    assert(identity);
    assert(privateKey);
    assert(certificate);

    if (
        configCheckSize(privateKey, CONFIG_8021X_KEY_MINSIZE, CONFIG_8021X_KEY_MAXSIZE) < 0 ||
        configWriteFile(CONFIG_8021X_KEY_FILENAME, privateKey) < 0)
    {
        return -1;
    }

    if (
        configCheckSize(certificate, CONFIG_8021X_CERT_MINSIZE, CONFIG_8021X_CERT_MAXSIZE) < 0 ||
        configWriteFile(CONFIG_8021X_CERT_FILENAME, certificate) < 0)
    {
        return -1;
    }

    if (certificateAuthority != NULL)
    {
        if (
            configCheckSize(certificateAuthority, CONFIG_8021X_CA_MINSIZE, CONFIG_8021X_CA_MAXSIZE) < 0 ||
            configWriteFile(CONFIG_8021X_CA_FILENAME, certificateAuthority) < 0)
        {
            return -1;
        }
    }

    char bufferKeyPassword[512];
    if (privateKeyPassword != NULL)
    {
        snprintf(bufferKeyPassword, sizeof(bufferKeyPassword), CONFIG_RADIUS_PASSWORD, privateKeyPassword);
    }
    else
    {
        bufferKeyPassword[0] = '\0';
    }

    return configWriteFile(
        CONFIG_8021X_WPA_FILENAME,
        CONFIG_RADIUS_CONTENT,
        identity,
        privateKeyPassword != NULL ? bufferKeyPassword : "",
        certificateAuthority != NULL ? CONFIG_RADIUS_CA : "");
}
