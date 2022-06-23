#ifndef GPIO_SYS_H
#define GPIO_SYS_H

#include <linux/types.h>

int gpioInit();
void gpioUninit();

int gpioInputReadAll(__u8 *data);
int gpioInputRead(int index, __u8 *on);

int gpioOutputReadAll(__u8 *data);
int gpioOutputRead(int index, __u8 *on);

int gpioOutputWriteAll(__u8 data);
int gpioOutputWrite(int index, __u8 on);

#endif // GPIO_SYS_H