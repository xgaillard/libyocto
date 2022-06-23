#include "config.h"
#include "logger.h"
#include "libyocto_config.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <glib.h>

#define CONFIG_NETWORK_DIR "/etc/systemd/network/"
#define CONFIG_NETWORK_FILENAME_MAXLEN 64

#define CONFIG_NETWORK_GROUP_NETWORK "Network"
#define CONFIG_NETWORK_KEY_ADDRESS "Address"
#define CONFIG_NETWORK_KEY_GATEWAY "Gateway"

#define CONFIG_WIFI_DIR "/etc/"
#define CONFIG_WIFI_FILENAME "hostapd.conf"
#define CONFIG_WIFI_FILENAME_LEN 12

#define CONFIG_WIFI_TMP_FILE_PATH "/tmp/" CONFIG_WIFI_FILENAME "-XXXXXX"
#define CONFIG_WIFI_TMP_FILE_PATH_LEN (5 + CONFIG_WIFI_FILENAME_LEN + 7)

#define CONFIG_WIFI_VARIABLE_SSID "ssid"
#define CONFIG_WIFI_VARIABLE_SSID_LEN 4
#define CONFIG_WIFI_VARIABLE_PASSPHRASE "wpa_passphrase"
#define CONFIG_WIFI_VARIABLE_PASSPHRASE_LEN 14

#define CONFIG_BUFFER_SIZE 4096

#define LOGGER_ERROR_GLIB(error, msg) LOGGER(LOGGER_LEVEL_ERROR, "(Glib %d) %s: %s)", (error)->code, (error)->message, msg)

static int _configNetworkGetFilename(const char *interface, char *filename)
{
    if (strcmp(interface, "eth0") == 0)
    {
        strcat(filename, LIBYOCTO_NETWORK_ETH0_FILENAME);
    }
#ifdef LIBYOCTO_NETWORK_ETH1_FILENAME
    else if (strcmp(interface, "eth1") == 0)
    {
        strcat(filename, LIBYOCTO_NETWORK_ETH1_FILENAME);
    }
#endif // LIBYOCTO_NETWORK_ETH1_FILENAME
#ifdef LIBYOCTO_NETWORK_WLAN0_FILENAME
    else if (strcmp(interface, "wlan0") == 0)
    {
        strcat(filename, LIBYOCTO_NETWORK_WLAN0_FILENAME);
    }
#endif // LIBYOCTO_NETWORK_WLAN0_FILENAME
    else
    {
        LOGGER(LOGGER_LEVEL_WARNING, "Unknown interface %s", interface);
        return -1;
    }

    return 0;
}

int configNetworkWrite(const char *interface, const char *cidrAddress, const char *gateway)
{
    assert(interface);

    char filename[CONFIG_NETWORK_FILENAME_MAXLEN] = CONFIG_NETWORK_DIR;
    if (_configNetworkGetFilename(interface, filename) < 0)
    {
        return -1;
    }

    g_autoptr(GError) error = NULL;
    g_autoptr(GKeyFile) keyfile = g_key_file_new();

    if (!g_key_file_load_from_file(keyfile, filename, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error))
    {
        LOGGER_ERROR_GLIB(error, filename);
        return -1;
    }

    int needToWrite = 0;

    if (cidrAddress[0] != '\0')
    {
        g_key_file_set_string(keyfile, CONFIG_NETWORK_GROUP_NETWORK, CONFIG_NETWORK_KEY_ADDRESS, cidrAddress);
        needToWrite |= 1;
    }
    if (gateway[0] != '\0')
    {
        g_key_file_set_string(keyfile, CONFIG_NETWORK_GROUP_NETWORK, CONFIG_NETWORK_KEY_GATEWAY, gateway);
        needToWrite |= 1;
    }
    else
    {
        g_key_file_remove_key(keyfile, CONFIG_NETWORK_GROUP_NETWORK, CONFIG_NETWORK_KEY_GATEWAY, NULL);
        needToWrite |= 1;
    }
    if (needToWrite && !g_key_file_save_to_file(keyfile, filename, &error))
    {
        LOGGER_ERROR_GLIB(error, filename);
        return -1;
    }

    return 0;
}

static int _configNetworkGetValue(GKeyFile *keyfile, const char *group, const char *key, GError *error, char *value, size_t len)
{
    g_autofree gchar *result = g_key_file_get_string(keyfile, group, key, &error);

    if (result == NULL)
    {
        value[0] = '\0';
        LOGGER(LOGGER_LEVEL_WARNING, "Group '%s' does not contain key '%s'", group, key);
    }
    else
    {
        size_t resultLen = strlen(result);
        if (len < resultLen)
        {
            LOGGER(LOGGER_LEVEL_ERROR, "Size of buffer too short (%ld/%ld) for key: %s", len, resultLen, key);
            return -1;
        }

        strcpy(value, result);
    }

    return 0;
}

int configNetworkRead(const char *interface, char *cidrAddress, size_t cidrAddressLen, char *gateway, size_t gatewayLen)
{
    assert(interface);

    char filename[CONFIG_NETWORK_FILENAME_MAXLEN] = CONFIG_NETWORK_DIR;

    if (_configNetworkGetFilename(interface, filename) < 0)
    {
        return -1;
    }

    g_autoptr(GError) error = NULL;
    g_autoptr(GKeyFile) keyfile = g_key_file_new();

    if (!g_key_file_load_from_file(keyfile, filename, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error))
    {
        LOGGER_ERROR_GLIB(error, filename);
        return -1;
    }

    if (_configNetworkGetValue(keyfile, CONFIG_NETWORK_GROUP_NETWORK, CONFIG_NETWORK_KEY_ADDRESS, error, cidrAddress, cidrAddressLen) < 0)
    {
        return -1;
    }

    if (_configNetworkGetValue(keyfile, CONFIG_NETWORK_GROUP_NETWORK, CONFIG_NETWORK_KEY_GATEWAY, error, gateway, gatewayLen) < 0)
    {
        return -1;
    }

    return 0;
}

//---------- Wifi ----------

int _configCopyTmpFile(int tmpFileDescriptor, const char *tmpFilename, const char *destFilename)
{
    // Reset tmp file
    if ((lseek(tmpFileDescriptor, 0, SEEK_SET)) < 0)
    {
        LOGGER_ERROR(errno, tmpFilename);
        return -1;
    }

    // Open destination
    FILE *destFile = fopen(destFilename, "w");

    if (destFile == NULL)
    {
        LOGGER_ERROR(errno, destFilename);
        return -1;
    }

    // Read tmp file and write to destination
    int rc = 0;
    char buffer[256];
    ssize_t count;

    while ((count = read(tmpFileDescriptor, buffer, sizeof(buffer))) > 0)
    {
        if (fwrite(buffer, count, 1, destFile) != 1)
        {
            if (ferror(destFile))
            {
                rc = -1;
                LOGGER(LOGGER_LEVEL_ERROR, destFilename);
                break;
            }
        }
    }

    if (count < 0)
    {
        rc = -1;
        LOGGER_ERROR(errno, tmpFilename);
    }

    fclose(destFile);

    return rc;
}

int _configWifiReplaceValue(char *pos, const char *value, int variableLen)
{
    size_t valueLen = strlen(value);

    if (variableLen + valueLen + 1 > CONFIG_BUFFER_SIZE)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "Size of buffer too short (%d/%d) for value: %s", CONFIG_BUFFER_SIZE, variableLen + valueLen + 1, value);
        return -1;
    }

    // Replace value
    strcpy(pos, value);

    // Add newline
    pos += valueLen;
    *pos = '\n';
    *(++pos) = '\0';

    return 0;
}

int _configWifiCopyValue(char *value, size_t valueLen, char *pos)
{
    size_t len = strlen(pos);
    assert(len);

    if (valueLen < len)
    {
        pos[len - 1] = '\0';
        LOGGER(LOGGER_LEVEL_ERROR, "Size of buffer too short (%d/%d) for value: %s", valueLen, len, pos);
        return -1;
    }

    // Copy value
    memcpy(value, pos, len - 1);

    // Remove newline
    value[len - 1] = '\0';

    return 0;
}

int configWifiRead(char *ssid, size_t ssidLen, char *passphrase, size_t passphraseLen)
{
    const char *originalFilename = CONFIG_WIFI_DIR CONFIG_WIFI_FILENAME;
    FILE *originalFile = fopen(originalFilename, "r");

    if (originalFile == NULL)
    {
        LOGGER_ERROR(errno, originalFilename);
        return -1;
    }

    char buffer[CONFIG_BUFFER_SIZE];
    int rc = 0;
    int nbValue = 2;

    while (rc == 0 && nbValue > 0 && fgets(buffer, sizeof(buffer), originalFile))
    {
        if (buffer[0] != '#' && buffer[0] != '\n')
        {
            char *pos = strchr(buffer, '=');

            if (pos == NULL)
            {
                LOGGER(LOGGER_LEVEL_WARNING, "%s: Invalid line '%s'", originalFile, buffer);
            }
            else
            {
                pos++;
                if (strncmp(buffer, CONFIG_WIFI_VARIABLE_SSID, CONFIG_WIFI_VARIABLE_SSID_LEN) == 0)
                {
                    if ((rc = _configWifiCopyValue(ssid, ssidLen, pos)) < 0)
                    {
                        nbValue--;
                    }
                }
                else if (strncmp(buffer, CONFIG_WIFI_VARIABLE_PASSPHRASE, CONFIG_WIFI_VARIABLE_PASSPHRASE_LEN) == 0)
                {
                    if ((rc = _configWifiCopyValue(passphrase, passphraseLen, pos)) < 0)
                    {
                        nbValue--;
                    }
                }
            }
        }
    }

    fclose(originalFile);

    return rc == 0 && nbValue == 0;
}

int configWifiWrite(const char *ssid, const char *passphrase)
{
    assert(ssid);
    assert(passphrase);

    // FIXME Check ssid and passphrase
    size_t passphraseLen = strlen(passphrase);

    if (passphraseLen < 8 || passphraseLen > 63)
    {
        return -1;
    }

    // Create tmp file
    char tmpFileName[CONFIG_WIFI_TMP_FILE_PATH_LEN + 1] = CONFIG_WIFI_TMP_FILE_PATH;
    int tmpFileDescriptor = mkstemp(tmpFileName);

    if (tmpFileDescriptor < 0)
    {
        LOGGER_ERROR(errno, tmpFileName);
        return -1;
    }

    // Read file
    int rc = 0;
    const char *originalFilename = CONFIG_WIFI_DIR CONFIG_WIFI_FILENAME;
    FILE *originalFile = fopen(originalFilename, "r");

    if (originalFile == NULL)
    {
        LOGGER_ERROR(errno, originalFilename);
        rc = -1;
    }
    else
    {
        char buffer[CONFIG_BUFFER_SIZE];
        char *result = NULL;

        while ((result = fgets(buffer, sizeof(buffer), originalFile)) != NULL)
        {
            if (buffer[0] != '#' && buffer[0] != '\n')
            {
                char *pos = strchr(buffer, '=');

                if (pos == NULL)
                {
                    LOGGER(LOGGER_LEVEL_WARNING, "%s: Invalid line '%s'", originalFile, buffer);
                }
                else
                {
                    pos++;
                    if (strncmp(buffer, CONFIG_WIFI_VARIABLE_SSID, CONFIG_WIFI_VARIABLE_SSID_LEN) == 0)
                    {
                        if ((rc = _configWifiReplaceValue(pos, ssid, CONFIG_WIFI_VARIABLE_SSID_LEN + 1)) < 0)
                        {
                            break;
                        }
                    }
                    else if (strncmp(buffer, CONFIG_WIFI_VARIABLE_PASSPHRASE, CONFIG_WIFI_VARIABLE_PASSPHRASE_LEN) == 0)
                    {
                        if ((rc = _configWifiReplaceValue(pos, passphrase, CONFIG_WIFI_VARIABLE_PASSPHRASE_LEN + 1)) < 0)
                        {
                            break;
                        }
                    }
                }
            }

            if ((rc = write(tmpFileDescriptor, buffer, strlen(buffer))) < 0)
            {
                LOGGER_ERROR(errno, tmpFileName);
                break;
            }
        }

        if (result == NULL && ferror(originalFile))
        {
            LOGGER(LOGGER_LEVEL_ERROR, "Failed to read from file: %s", originalFilename);
        }

        fclose(originalFile);

        if (rc >= 0)
        {
            if ((rc = remove(originalFilename)) < 0)
            {
                LOGGER_ERROR(errno, originalFilename);
            }
            else
            {
                rc = _configCopyTmpFile(tmpFileDescriptor, tmpFileName, originalFilename);
            }
        }
    }

    unlink(tmpFileName);

    return rc < 0 ? -1 : 0;
}
