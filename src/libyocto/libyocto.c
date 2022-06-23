#include "libyocto.h"
#include "libyocto_config.h"

#include "logger.h"
#include "daemon.h"
#include "service.h"
#include "config.h"

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

#if defined(LIBYOCTO_DIGITAL_INPUT_GPIO) ||  defined(LIBYOCTO_DIGITAL_OUTPUT_GPIO)
    rc =  gpioInit();
#endif

    return rc;
}

void yoctoUninit()
{
#if defined(LIBYOCTO_DIGITAL_INPUT_GPIO) ||  defined(LIBYOCTO_DIGITAL_OUTPUT_GPIO)
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
    (void)id; (void)on;
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
    return  gpioOutputRead(id, on);
#endif
#else
    (void)id; (void)on;
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
    (void)id; (void)on;
    return -1;
#endif
}

//---------- LEDS ----------

int yoctoLedAlarmOff()
{
#ifdef LIBYOCTO_LED_ALARM
#ifdef LIBYOCTO_LED_RBOX630
    return ledRboxSet(LED_RBOX_ALARM, 0);
#else
    return ledSysSetTrigger(LIBYOCTO_LED_ALARM_FILENAME, LED_SYSFS_TRIGGER_NONE);
#endif
#else
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
    (void)id;(void)type;(void)termination;
    return -1;
#endif
}

int yoctoComConfigRead(int id, uint8_t *type, uint8_t *termination)
{
#ifdef LIBYOCTO_COMCONFIG_RBOX630
    return comRboxConfigRead(id, type, termination);
#else
    (void)id;(void)type;(void)termination;
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

//---------- CONFIG ----------

int yoctoConfigNetworkRead(const char *interface, char *cidrAddress, size_t cidrAddressLen, char *gateway, size_t gatewayLen)
{
    return configNetworkRead(interface, cidrAddress, cidrAddressLen, gateway, gatewayLen);
}

int yoctoConfigNetworkWrite(const char *interface, const char *cidrAddress, const char *gateway)
{
    return configNetworkWrite(interface, cidrAddress, gateway);
}

int yoctoConfigWifiRead(char *ssid, size_t ssidLen, char *passphrase, size_t passphraseLen)
{
#ifdef LIBYOCTO_WIFI
    return configWifiRead(ssid, ssidLen, passphrase, passphraseLen);
#else
    (void)ssid;(void)ssidLen;(void)passphrase;(void)passphraseLen;
    return -1;
#endif
}

int yoctoConfigWifiWrite(const char *ssid, const char *passphrase)
{
#ifdef LIBYOCTO_WIFI
    return configWifiWrite(ssid, passphrase);
#else
    (void)ssid;(void)passphrase;
    return -1;
#endif
}
