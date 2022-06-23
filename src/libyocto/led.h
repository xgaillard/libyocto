#ifndef LED_SYSFS_H
#define LED_SYSFS_H

#define LED_SYSFS_TRIGGER_NONE        "none"
#define LED_SYSFS_TRIGGER_DEFAULT_ON  "default-on"
#define LED_SYSFS_TRIGGER_HEARTBEAT   "heartbeat"
#define LED_SYSFS_TRIGGER_PHY0TPT     "phy0tpt"

int ledSysSetTrigger(const char *card, const char *trigger);

#endif // LED_SYSFS_H
