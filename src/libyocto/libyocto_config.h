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
#define LIBYOCTO_NETWORK_ETH0_FILENAME      "10-eth0.network"
#define LIBYOCTO_NETWORK_ETH1_FILENAME      "10-eth1.network"
#define LIBYOCTO_NETWORK_WLAN0_FILENAME     "30-wlan0.network"
#define LIBYOCTO_DIGITAL_INPUT_COUNT        8
#undef LIBYOCTO_DIGITAL_OUTPUT_GPIO
#define LIBYOCTO_LED_WIFI_FILENAME          "minicard1-link"

#define LIBYOCTO_DIGITAL_OUTPUT_RBOX630
#define LIBYOCTO_DIGITAL_OUTPUT_COUNT   8

#define LIBYOCTO_COMCONFIG_RBOX630
#define LIBYOCTO_LED_RBOX630

#elif defined(GWS501)
#undef LIBYOCTO_WIFI
#undef LIBYOCTO_DIGITAL_INPUT_GPIO
#undef LIBYOCTO_DIGITAL_OUTPUT_GPIO
#undef LIBYOCTO_LED_ALARM
#undef LIBYOCTO_LED_WIFI

//From: meta-fareco/recipes-core/systemd/systemd-network/10-eth0.network
#define LIBYOCTO_NETWORK_ETH0_FILENAME  "10-eth0.network"

//From: meta-fareco/dynamic-layers/meta-axiomtek/recipes-core/systemd/systemd-network/rbox630/10-eth1.network
//                                /meta-gateworks/recipes-core/systemd/systemd-network/gw73xx/10-eth1.network
#define LIBYOCTO_NETWORK_ETH1_FILENAME  "10-eth1.network"

//From: meta-fareco/recipes-bsp/barebox/barebox/gws501/imx6ull-phytec-phycore-som-nand.dts
#define LIBYOCTO_LED_STATUS_FILENAME    "user-led1"

#elif defined(RADIPV3)
#undef LIBYOCTO_WIFI
#undef LIBYOCTO_LED_ALARM
#undef LIBYOCTO_LED_WIFI

//From: meta-fareco/recipes-core/systemd/systemd-conf/network/10-eth0.network
#define LIBYOCTO_NETWORK_ETH0_FILENAME      "10-eth0.network"

//From: meta-fareco/recipes-bsp/barebox/barebox/radipv3/imx6ull-phytec-phycore-som-nand.dts
#define LIBYOCTO_LED_STATUS_FILENAME        "user-led1"

#define LIBYOCTO_DIGITAL_INPUT_COUNT        3
#define LIBYOCTO_DIGITAL_OUTPUT_COUNT       5

#endif

// --------------------------------------------
// Aggregates
// --------------------------------------------

#if defined(LIBYOCTO_NETWORK) || defined(LIBYOCTO_WIFI)
#define LIBYOCTO_SSL
#define LIBYOCTO_SNMP
#define LIBYOCTO_NTP
#define LIBYOCTO_REMOTE_JOURNAL
#define LIBYOCTO_8021X
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
