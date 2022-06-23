#ifndef COM_RBOX_H
#define COM_RBOX_H

#include <linux/types.h>

#define COM_TYPE_RS232  1
#define COM_TYPE_RS422_485_4W  2
#define COM_TYPE_RS485_2W  3

int comRboxConfigWrite(int id, __u8 type, __u8 termination);
int comRboxConfigRead(int id, __u8* type, __u8* termination);

#endif // COM_RBOX_H