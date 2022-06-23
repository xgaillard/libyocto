#ifndef LIBYOCTO_H
#define LIBYOCTO_H

#include <stdint.h>
#include <stddef.h>

/**
 * Compilation Options:
 * make MACHINE=[RBOX630|GWS501|RADIPV3]
 */

//TODO Add eeprom
//TODO Add checkAxiomtek

int yoctoInit();
void yoctoUninit();

//---------- LOG ----------

/**
 * Disable log output.
 */
void yoctoSetLogNone();

/**
 * Enable log output for errors.
 */
void yoctoSetLogError();

/**
 * Enable log output for errors and warnings.
 */
void yoctoSetLogWarning();

/**
 * Enable log output for informations, errors and warnings.
 */
void yoctoSetLogInfo();

/**
 * Enable log output for all.
 */
void yoctoSetLogDebug();

//---------- GPIO INPUT ----------

/**
 * Reads all digital input.
 * \param[out]  data    RBOX630:    Digital input state bit0=DI0 ... bit7=DI7 (1=open 0=closed)
 *                      RADIPV3:    bit0=Processeur Boucle
 *                                  bit1=Defaut Alim
 *                                  bit2=Parasurtenseur
 * \returns 0 if ok, -1 if error.
 */
int yoctoDigitalInputReadAll(uint8_t *data);

/**
 * Reads a digital input.
 * \param[in]   id      RBOX630:    Digital input number (0-7)
 *                      RADIPV3:    0=Processeur Boucle
 *                                  1=Defaut Alim
 *                                  2=Parasurtenseur
 * \param[out]  on      Digital input state (1=open 0=closed)
 * \returns 0 if ok, -1 if error.
 */
int yoctoDigitalInputRead(int id, uint8_t *on);

//---------- GPIO OUTPUT ----------

/**
 * Reads all digital output.
 * \param[out]  data    RBOX630:    Digital output state bit0=DO0 ... bit7=DO7 (1=open 0=closed)
 *                      RADIPV3:    bit0=Alim Local
 *                                  bit1=Alim Deport2
 *                                  bit2=Alim Deport1
 *                                  bit3=Alim Ext
 *                                  bit4=Alim 24V Isole
 * \returns 0 if ok, -1 if error.
 */
int yoctoDigitalOutputReadAll(uint8_t *data);

/**
 * Reads a digital output.
 * \param[in]   id  RBOX630:    Digital output number (0-7)
 *                  RADIPV3:    0=Alim Local
 *                              1=Alim Deport2
 *                              2=Alim Deport1
 *                              3=Alim Ext
 *                              4=Alim 24V Isole
 * \param[out]  on  Digital output state (1=open 0=closed)
 * \returns 0 if ok, -1 if error.
 */
int yoctoDigitalOutputRead(int id, uint8_t *on);

/**
 * Sets all digital output.
 * \param[in]  data     RBOX630:    Digital output state bit0=DO0 ... bit7=DO7 (1=open 0=closed)
 *                      RADIPV3:    bit0=Alim Local
 *                                  bit1=Alim Deport2
 *                                  bit2=Alim Deport1
 *                                  bit3=Alim Ext
 *                                  bit4=Alim 24V Isole
 * \returns 0 if ok, -1 if error.
 */
int yoctoDigitalOutputWriteAll(uint8_t data);

/**
 * Sets a digital output.
 * \param[in]   id  RBOX630:    Digital output number (0-7).
 *                  RADIPV3:    0=Alim Local
 *                              1=Alim Deport2
 *                              2=Alim Deport1
 *                              3=Alim Ext
 *                              4=Alim 24V Isole
 * \param[in]   on  Digital output state (1=open 0=closed)
 * \returns 0 if ok, -1 if error.
 */
int yoctoDigitalOutputWrite(int id, uint8_t on);

//---------- LEDS ----------

/**
 * Sets alarm led off.
 * \returns 0 if ok, -1 if error.
 */
int yoctoLedAlarmOff();

/**
 * Sets alarm led on.
 * \returns 0 if ok, -1 if error.
 */
int yoctoLedAlarmOn();

/**
 * Sets status led off.
 * \returns 0 if ok, -1 if error.
 */
int yoctoLedStatusBlink();    

/**
 * Sets status led on.
 * \returns 0 if ok, -1 if error.
 */
int yoctoLedStatusOn();

/**
 * Sets wifi led off.
 * \returns 0 if ok, -1 if error.
 */
int yoctoLedWifiOn();

/**
 * Sets wifi led on.
 * \returns 0 if ok, -1 if error.
 */
int yoctoLedWifiOff();

//---------- COM ----------

/**
 * Writes the configuration of a com port.
 * \param[in]   id          Com port number (1-4).
 * \param[in]   type        Com port type (1=RS232, 2=RS422_485_4W, 3=RS485_2W)
 * \param[in]   termination Com port termination (1=enabled 0=disabled)
 * \returns 0 if ok, -1 if error.
 */
int yoctoComConfigWrite(int id, uint8_t type, uint8_t termination);

/**
 * Reads the configuration of a com port.
 * \param[in]   id          Com port number (1-4).
 * \param[out]  type        Com port type (1=RS232, 2=RS422_485_4W, 3=RS485_2W)
 * \param[out]  termination Com port termination (1=enabled 0=disabled)
 * \returns 0 if ok, -1 if error.
 */
int yoctoComConfigRead(int id, uint8_t *type, uint8_t *termination);

//---------- DAEMON ----------

/**
 * Notify the systemd watchdog that the program is running.
 * \returns 0 if ok, -1 if error.
 */
int yoctoDaemonWatchdogNotify();

//---------- SERVICE ----------

/**
 * Stops a systemd service.
 * \param[in]   service Name of the service (myservice.service)
 * \returns 0 if ok, -1 if error.
 */
int yoctoServiceStop(const char *service);

/**
 * Starts a systemd service.
 * \param[in]   service Name of the service (myservice.service)
 * \returns 0 if ok, -1 if error.
 */
int yoctoServiceStart(const char *service);

/**
 * Restarts a systemd service.
 * \param[in]   service Name of the service (myservice.service)
 * \returns 0 if ok, -1 if error.
 */
int yoctoServiceRestart(const char *service);

/**
 * Reads the number of time a service has been restarted because of failure since the last request.
 * This number resets to 0 when the system boots
 * \param[in]   service Name of the service (myservice.service)
 * \param[out]  count   Number of times the service has been restarted
 * \returns 0 if ok, -1 if error.
 */
int yoctoServiceNbRestart(const char *service, uint32_t *count);

/**
 * Sets the system time (and saves it to the rtc).
 * \param[in]   timestampMicroSecond Unix timestamp in microSeconds
 * \returns 0 if ok, -1 if error.
 */
int yoctoServiceSetTime(int64_t timestampMicroSecond);

//---------- CONFIG ----------

//TODO Add chrony configuration

/**
 * Reads the configuration of a network interface.
 * \param[in]   interface       Name of the network interface (eth0, eth1, wlan0).
 * \param[out]  cidrAddress     Ip address with cidr mask (X.X.X.X/XX)
 * \param[in]   cidrAddressLen  Length of the cidrAddress buffer
 * \param[out]  gateway         Ip address of the gateway with cidr mask (X.X.X.X/XX)
 * \param[in]   gatewayLen      Length of the gateway buffer
 * \returns 0 if ok, -1 if error.
 */
int yoctoConfigNetworkRead(const char *interface, char *cidrAddress, size_t cidrAddressLen, char *gateway, size_t gatewayLen);

/**
 * Writes the configuration of a network interface.
 * \param[in]   interface       Name of the network interface (eth0, eth1, wlan0).
 * \param[in]   cidrAddress     Ip address with cidr mask (X.X.X.X/XX)
 * \param[in]   gateway         Ip address of the gateway with cidr mask (X.X.X.X/XX)
* \returns 0 if ok, -1 if error.
 */
int yoctoConfigNetworkWrite(const char *interface, const char *cidrAddress, const char *gateway);

/**
 * Reads the configuration of the wifi access point.
 * \param[out]  ssid            Ip address with cidr mask (X.X.X.X/XX)
 * \param[in]   cidrAddressLen  Length of the cidrAddress buffer
 * \param[out]  gateway         Ip address of the gateway with cidr mask (X.X.X.X/XX)
 * \param[in]   gatewayLen      Length of the gateway buffer
 * \returns 0 if ok, -1 if error.
 */
int yoctoConfigWifiRead(char *ssid, size_t ssidLen, char *passphrase, size_t passphraseLen);

/**
 * Writes the configuration of the wifi access point.
 * \param[in]   ssid            Ip address with cidr mask (X.X.X.X/XX)
 * \param[in]   gateway         Ip address of the gateway with cidr mask (X.X.X.X/XX)
* \returns 0 if ok, -1 if error.
 */
int yoctoConfigWifiWrite(const char *ssid, const char *passphrase);

#endif // LIBYOCTO_H
