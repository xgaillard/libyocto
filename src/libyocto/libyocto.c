#include "libyocto.h"
#include "libyocto_config.h"

#include "logger.h"
#include "daemon.h"
#include "service.h"

#ifdef LIBYOCTO_NETWORK
#include "config/config_network.h"
#endif

#ifdef LIBYOCTO_WIFI
#include "config/config_wifi.h"
#endif

#if defined(LIBYOCTO_NETWORK) || defined(LIBYOCTO_WIFI)
#ifdef LIBYOCTO_SSL
#include "config/config_ssl.h"
#endif
#ifdef LIBYOCTO_SNMP
#include "config/config_snmp.h"
#endif
#ifdef LIBYOCTO_NTP
#include "config/config_ntp.h"
#endif
#ifdef LIBYOCTO_REMOTE_JOURNAL
#include "config/config_remotejournal.h"
#endif
#ifdef LIBYOCTO_8021X
#include "config/config_8021x.h"
#endif
#if defined(LIBYOCTO_NTP) || defined(LIBYOCTO_REMOTE_JOURNAL) || defined(LIBYOCTO_8021X)
#include "config/config_service.h"
#endif
#endif

#if defined(LIBYOCTO_DIGITAL_INPUT_GPIO) || defined(LIBYOCTO_DIGITAL_OUTPUT_GPIO)
#include "gpio.h"
#endif

#ifdef LIBYOCTO_DIGITAL_OUTPUT_RBOX630
#include "gpio_rbox.h"
#endif

#ifdef LIBYOCTO_LED_SYSFS
#include "led.h"
#endif

#ifdef LIBYOCTO_LED_RBOX630
#include "led_rbox.h"
#endif

#ifdef LIBYOCTO_COMCONFIG_RBOX630
#include "com_rbox.h"
#endif

int yoctoInit()
{
    int rc = 0;

#if defined(LIBYOCTO_DIGITAL_INPUT_GPIO) || defined(LIBYOCTO_DIGITAL_OUTPUT_GPIO)
    rc = gpioInit();
#endif

    return rc;
}

void yoctoUninit()
{
#if defined(LIBYOCTO_DIGITAL_INPUT_GPIO) || defined(LIBYOCTO_DIGITAL_OUTPUT_GPIO)
    gpioUninit();
#endif
}

//---------- LOG ----------

void yoctoSetLogNone()
{
    loggerSetLevel(LOGGER_LEVEL_NONE);
}

void yoctoSetLogError()
{
    loggerSetLevel(LOGGER_LEVEL_ERROR);
}

void yoctoSetLogWarning()
{
    loggerSetLevel(LOGGER_LEVEL_WARNING);
}

void yoctoSetLogInfo()
{
    loggerSetLevel(LOGGER_LEVEL_INFO);
}

void yoctoSetLogDebug()
{
    loggerSetLevel(LOGGER_LEVEL_DEBUG);
}

//---------- GPIO INPUT ----------

int yoctoDigitalInputReadAll(uint8_t *data)
{
#ifdef LIBYOCTO_DIGITAL_INPUT_GPIO
    return gpioInputReadAll(data);
#else
    (void)data;
    return -1;
#endif
}

int yoctoDigitalInputRead(int id, uint8_t *on)
{
#ifdef LIBYOCTO_DIGITAL_INPUT_GPIO
    return gpioInputRead(id, on);
#else
    (void)id;
    (void)on;
    return -1;
#endif
}

//---------- GPIO OUTPUT ----------

int yoctoDigitalOutputReadAll(uint8_t *data)
{
#ifdef LIBYOCTO_DIGITAL_OUTPUT
#ifdef LIBYOCTO_DIGITAL_OUTPUT_RBOX630
    return gpioRboxReadAll(data);
#else
    return gpioOutputReadAll(data);
#endif
#else
    (void)data;
    return -1;
#endif
}

int yoctoDigitalOutputRead(int id, uint8_t *on)
{
#ifdef LIBYOCTO_DIGITAL_OUTPUT
#ifdef LIBYOCTO_DIGITAL_OUTPUT_RBOX630
    return gpioRboxRead(id, on);
#else
    return gpioOutputRead(id, on);
#endif
#else
    (void)id;
    (void)on;
    return -1;
#endif
}

int yoctoDigitalOutputWriteAll(uint8_t data)
{
#ifdef LIBYOCTO_DIGITAL_OUTPUT
#ifdef LIBYOCTO_DIGITAL_OUTPUT_RBOX630
    return gpioRboxWriteAll(data);
#else
    return gpioOutputWriteAll(data);
#endif
#else
    (void)data;
    return -1;
#endif
}

int yoctoDigitalOutputWrite(int id, uint8_t on)
{
#ifdef LIBYOCTO_DIGITAL_OUTPUT
#ifdef LIBYOCTO_DIGITAL_OUTPUT_RBOX630
    return gpioRboxWrite(id, on);
#else
    return gpioOutputWrite(id, on);
#endif
#else
    (void)id;
    (void)on;
    return -1;
#endif
}

int yoctoLedAlarmOn()
{
#ifdef LIBYOCTO_LED_ALARM
#ifdef LIBYOCTO_LED_RBOX630
    return ledRboxSet(LED_RBOX_ALARM, 1);
#else
    return ledSysSetTrigger(LIBYOCTO_LED_ALARM_FILENAME, LED_SYSFS_TRIGGER_DEFAULT_ON);
#endif
#else
    return -1;
#endif
}

int yoctoLedStatusBlink()
{
#ifdef LIBYOCTO_LED_STATUS
#ifdef LIBYOCTO_LED_RBOX630
    return ledRboxSet(LED_RBOX_ACTREADY, 1);
#else
    return ledSysSetTrigger(LIBYOCTO_LED_STATUS_FILENAME, LED_SYSFS_TRIGGER_HEARTBEAT);
#endif
#else
    return -1;
#endif
}

int yoctoLedStatusOn()
{
#ifdef LIBYOCTO_LED_STATUS
#ifdef LIBYOCTO_LED_RBOX630
    return ledRboxSet(LED_RBOX_ACTREADY, 0);
#else
    return ledSysSetTrigger(LIBYOCTO_LED_STATUS_FILENAME, LED_SYSFS_TRIGGER_DEFAULT_ON);
#endif
#else
    return -1;
#endif
}

int yoctoLedWifiOn()
{
#ifdef LIBYOCTO_LED_WIFI
    return ledSysSetTrigger(LIBYOCTO_LED_WIFI_FILENAME, LED_SYSFS_TRIGGER_PHY0TPT);
#else
    return -1;
#endif
}

int yoctoLedWifiOff()
{
#ifdef LIBYOCTO_LED_WIFI
    return ledSysSetTrigger(LIBYOCTO_LED_WIFI_FILENAME, LED_SYSFS_TRIGGER_NONE);
#else
    return -1;
#endif
}

//---------- COM ----------

int yoctoComConfigWrite(int id, uint8_t type, uint8_t termination)
{
#ifdef LIBYOCTO_COMCONFIG_RBOX630
    return comRboxConfigWrite(id, type, termination);
#else
    (void)id;
    (void)type;
    (void)termination;
    return -1;
#endif
}

int yoctoComConfigRead(int id, uint8_t *type, uint8_t *termination)
{
#ifdef LIBYOCTO_COMCONFIG_RBOX630
    return comRboxConfigRead(id, type, termination);
#else
    (void)id;
    (void)type;
    (void)termination;
    return -1;
#endif
}

//---------- DAEMON ----------

int yoctoDaemonWatchdogNotify()
{
    return daemonWatchdogNotify();
}

//---------- SERVICE ----------

int yoctoServiceStop(const char *service)
{
    return serviceManage(service, "StopUnit");
}

int yoctoServiceStart(const char *service)
{
    return serviceManage(service, "StartUnit");
}

int yoctoServiceRestart(const char *service)
{
    return serviceManage(service, "RestartUnit");
}

int yoctoServiceNbRestart(const char *service, uint32_t *count)
{
    return serviceGetNbRestart(service, count);
}

int yoctoServiceSetTime(int64_t timestampMicroSecond)
{
    return serviceSetTime(timestampMicroSecond);
}

//---------- CONFIG NETWORK ----------

int yoctoConfigNetworkRead(const char *interface, char *cidrAddress, size_t cidrAddressLen, char *gateway, size_t gatewayLen)
{
#ifdef LIBYOCTO_NETWORK
    return configNetworkRead(interface, cidrAddress, cidrAddressLen, gateway, gatewayLen);
#else
    (void)interface;
    (void)cidrAddress;
    (void)cidrAddressLen;
    (void)gateway;
    (void)gatewayLen;
    return -1;
#endif
}

int yoctoConfigNetworkWrite(const char *interface, const char *cidrAddress, const char *gateway)
{
#ifdef LIBYOCTO_NETWORK
    return configNetworkWrite(interface, cidrAddress, gateway);
#else
    (void)interface;
    (void)cidrAddress;
    (void)gateway;
    return -1;
#endif
}

//---------- CONFIG WIFI ----------

int yoctoConfigWifiRead(char *ssid, size_t ssidLen, char *passphrase, size_t passphraseLen)
{
#ifdef LIBYOCTO_WIFI
    return configWifiRead(ssid, ssidLen, passphrase, passphraseLen);
#else
    (void)ssid;
    (void)ssidLen;
    (void)passphrase;
    (void)passphraseLen;
    return -1;
#endif
}

int yoctoConfigWifiWrite(const char *ssid, const char *passphrase)
{
#ifdef LIBYOCTO_WIFI
    return configWifiWrite(ssid, passphrase);
#else
    (void)ssid;
    (void)passphrase;
    return -1;
#endif
}

//---------- CONFIG SSL ----------

int yoctoConfigSsl(const char *privateKey, const char *certificate)
{
#ifdef LIBYOCTO_SSL
    return configSslWrite(privateKey, certificate);
#else
    (void)privateKey;
    (void)certificate;
    return -1;
#endif
}

//---------- CONFIG NTP ----------

int yoctoConfigNtpWrite(const char *ip, const int key)
{
#ifdef LIBYOCTO_NTP
    return configNtpWrite(ip, key);
#else
    (void)ip;
    (void)key;
    return -1;
#endif
}

int yoctoConfigNtpKeysWrite(const char *content)
{
#ifdef LIBYOCTO_NTP
    return configNtpKeysWrite(content);
#else
    (void)content;
    return -1;
#endif
}

int yoctoConfigNtpEnable(int enable)
{
#ifdef LIBYOCTO_NTP
    return configServiceEnable(CONFIG_SERVICE_NTP, enable);
#else
    (void)enable;
    return -1;
#endif
}

int yoctoConfigNtpEnabled()
{
#ifdef LIBYOCTO_NTP
    return configServiceIsEnabled(CONFIG_SERVICE_NTP);
#else
    return -1;
#endif
}

//---------- CONFIG SNMP ----------

int yoctoConfigSnmp(const char *user, const char *auth, const char *priv, const char *view)
{
#ifdef LIBYOCTO_SNMP
    int rc = 0;

    // Stop snmpd
    if ((rc = yoctoServiceStop(CONFIG_SNMP_SERVICE) >= 0))
    {
        // Save user
        rc = configSnmpWrite(user, auth, priv, view);
    }

    // Start snmpd
    if (yoctoServiceStart(CONFIG_SNMP_SERVICE) < 0)
    {
        return -1;
    }

    return rc;
#else
    (void)user;
    (void)auth;
    (void)priv;
    (void)view;
    return -1;
#endif
}

//---------- CONFIG REMOTE JOURNAL ----------

int yoctoConfigRemoteJournalWrite(const char *ip, const char *certificate)
{
#ifdef LIBYOCTO_REMOTE_JOURNAL
    return configRemoteJournalWrite(ip, certificate);
#else
    (void)ip;
    (void)certificate;
    return -1;
#endif
}

int yoctoConfigRemoteJournalEnable(int enable)
{
#ifdef LIBYOCTO_REMOTE_JOURNAL
    return configServiceEnable(CONFIG_SERVICE_JOURNAL, enable);
#else
    (void)enable;
    return -1;
#endif
}

int yoctoConfigRemoteJournalEnabled()
{
#ifdef LIBYOCTO_REMOTE_JOURNAL
    return configServiceIsEnabled(CONFIG_SERVICE_JOURNAL);
#else
    return -1;
#endif
}

//---------- CONFIG 802.1x ----------

int yoctoConfig8021xWrite(const char *identity, const char *privateKey, const char *privateKeyPassword, const char *certificate, const char *certificateAuthority)
{
#ifdef LIBYOCTO_8021X
    return config8021xWrite(identity, privateKey, privateKeyPassword, certificate, certificateAuthority);
#else
    return -1;
#endif
}

int yoctoConfig8021xEnable(int enable)
{
#ifdef LIBYOCTO_REMOTE_JOURNAL
    return configServiceEnable(CONFIG_SERVICE_8021X, enable);
#else
    (void)enable;
    return -1;
#endif
}

int yoctoConfig8021xEnabled()
{
#ifdef LIBYOCTO_REMOTE_JOURNAL
    return configServiceIsEnabled(CONFIG_SERVICE_8021X);
#else
    return -1;
#endif
}

