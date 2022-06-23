#ifndef GPIO_RBOX_H
#define GPIO_RBOX_H

#include <linux/types.h>

int gpioRboxReadAll(__u8 *data);
int gpioRboxRead(int id, __u8 *on);

int gpioRboxWriteAll(__u8 data);
int gpioRboxWrite(int id, __u8 on);

#endif // GPIO_RBOX_H