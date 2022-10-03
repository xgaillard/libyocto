#ifndef LIBYOCTO_CONFIG_H
#define LIBYOCTO_CONFIG_H

// --------------------------------------------
// Common
// --------------------------------------------
#define LIBYOCTO_NETWORK
#define LIBYOCTO_WIFI

//#define LIBYOCTO_COMCONFIG_RBOX630

#define LIBYOCTO_DIGITAL_INPUT_GPIO
#define LIBYOCTO_DIGITAL_OUTPUT_GPIO
//#define LIBYOCTO_DIGITAL_OUTPUT_RBOX630

#define LIBYOCTO_LED_SYSFS
//LIBYOCTO_LED_RBOX630

#define LIBYOCTO_LED_STATUS
#define LIBYOCTO_LED_ALARM
#define LIBYOCTO_LED_WIFI

// --------------------------------------------
// Machine specific
// --------------------------------------------

#if defined(AMD64)
#undef LIBYOCTO_WIFI

#undef LIBYOCTO_DIGITAL_INPUT_GPIO
#undef LIBYOCTO_DIGITAL_OUTPUT_GPIO

#undef LIBYOCTO_LED_SYSFS
#undef LIBYOCTO_LED_STATUS
#undef LIBYOCTO_LED_ALARM
#undef LIBYOCTO_LED_WIFI

#elif defined(RBOX630)
#define LIBYOCTO_NETWORK_ETH0_FILENAME      "20-eth0.network"
#define LIBYOCTO_NETWORK_ETH1_FILENAME      "30-eth1.network"
#define LIBYOCTO_NETWORK_WLAN0_FILENAME     "40-wlan0.network"
#define LIBYOCTO_DIGITAL_INPUT_COUNT        8
#undef LIBYOCTO_DIGITAL_OUTPUT_GPIO
#define LIBYOCTO_LED_WIFI_FILENAME          "card1-link"

#define LIBYOCTO_COMCONFIG_RBOX630
#define LIBYOCTO_DIGITAL_OUTPUT_RBOX630
#define LIBYOCTO_LED_RBOX630

#elif defined(GWS501)
#undef LIBYOCTO_WIFI
#undef LIBYOCTO_DIGITAL_INPUT_GPIO
#undef LIBYOCTO_DIGITAL_OUTPUT_GPIO
#undef LIBYOCTO_LED_ALARM
#undef LIBYOCTO_LED_WIFI

#define LIBYOCTO_NETWORK_ETH0_FILENAME  "10-eth0.network"
#define LIBYOCTO_NETWORK_ETH1_FILENAME  "10-eth1.network"
#define LIBYOCTO_LED_STATUS_FILENAME    "user-led1"

#elif defined(RADIPV3)
#undef LIBYOCTO_WIFI
#undef LIBYOCTO_LED_ALARM
#undef LIBYOCTO_LED_WIFI

#define LIBYOCTO_NETWORK_ETH0_FILENAME      "10-eth0.network"
#define LIBYOCTO_LED_STATUS_FILENAME        "user-led1"
#define LIBYOCTO_DIGITAL_INPUT_COUNT        3
#define LIBYOCTO_DIGITAL_OUTPUT_COUNT       5

#endif

// --------------------------------------------
// Aggregates
// --------------------------------------------

#if defined(LIBYOCTO_NETWORK) || defined(LIBYOCTO_WIFI)
#define LIBYOCTO_SNMP
#endif

#if defined(LIBYOCTO_DIGITAL_INPUT_GPIO)
#define LIBYOCTO_DIGITAL_INPUT
#endif

#if defined(LIBYOCTO_DIGITAL_OUTPUT_GPIO) || defined(LIBYOCTO_DIGITAL_OUTPUT_RBOX630)
#define LIBYOCTO_DIGITAL_OUTPUT
#endif

// --------------------------------------------
// Checks
// --------------------------------------------
#if (defined(LIBYOCTO_LED_STATUS) || defined(LIBYOCTO_LED_ALARM) || defined(LIBYOCTO_LED_WIFI)) && \
    !(defined(LIBYOCTO_LED_SYSFS) || defined(LIBYOCTO_LED_RBOX630))
#error "LIBYOCTO_LED_* is defined but no support is provided"
#endif

/*
#if defined(LIBYOCTO_LED_STATUS) && !defined(LIBYOCTO_LED_STATUS_FILENAME)
#error "LIBYOCTO_LED_STATUS* is defined but no filename is provided"
#endif
#if defined(LIBYOCTO_LED_SYSFS) && defined(LIBYOCTO_LED_ALARM) && !defined(LIBYOCTO_LED_ALARM_FILENAME)
#error "LIBYOCTO_LED_ALARM* is defined but no filename is provided"
#endif
#if defined(LIBYOCTO_LED_SYSFS) && defined(LIBYOCTO_LED_WIFI) && !defined(LIBYOCTO_LED_WIFI_FILENAME)
#error "LIBYOCTO_LED_WIFI* is defined but no filename is provided"
#endif
*/

#endif // LIBYOCTO_CONFIG_H
