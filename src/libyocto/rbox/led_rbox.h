#ifndef LED_RBOX_H
#define LED_RBOX_H

#include <linux/types.h>

#define LED_RBOX_ACTREADY   0
#define LED_RBOX_ALARM      6

int ledRboxSet(__u8 led, __u8 on);

#endif // LED_RBOX_H
