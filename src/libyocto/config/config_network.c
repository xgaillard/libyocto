#include "config_network.h"
#include "logger.h"
#include "libyocto_config.h"

#include <assert.h>
#include <glib.h>

#define CONFIG_NETWORK_DIR "/etc/systemd/network/"
#define CONFIG_NETWORK_FILENAME_MAXLEN 64

#define CONFIG_NETWORK_GROUP_NETWORK "Network"
#define CONFIG_NETWORK_KEY_ADDRESS "Address"
#define CONFIG_NETWORK_KEY_GATEWAY "Gateway"

static int _configNetworkGetFilename(const char *interface, char *filename)
{
#ifdef LIBYOCTO_NETWORK_ETH0_FILENAME
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
#else //LIBYOCTO_NETWORK_ETH0_FILENAME
    (void)interface;(void)filename;
    return -1;
#endif //LIBYOCTO_NETWORK_ETH0_FILENAME
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
