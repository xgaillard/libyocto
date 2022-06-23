#ifndef CPLD_RBOX_H
#define CPLD_RBOX_H

#include <linux/types.h>

#define CPLD_BANK_CONF 0
#define CPLD_BANK_SYS 1
#define CPLD_BANK_COM 2
#define CPLD_BANK_USB 3
#define CPLD_BANK_WDT 4
#define CPLD_BANK_GPI 5
#define CPLD_BANK_GPO 6
#define CPLD_BANK_CAN 7

#define CPLD_COMMAND_BANK_SEL 0x4E
#define CPLD_COMMAND_COM 0x50
#define CPLD_COMMAND_GPO 0x50
#define CPLD_COMMAND_LED 0x51

/**
 * Opens and locks the cpld device.
 * \returns 0 if ok, -1 if error.
 */
int cpldRboxOpen(int *fd);

/**
 * Closes and releases the cpld device.
 * \returns 0 if ok, -1 if error.
 */
void cpldRboxClose(int fd); 

/**
 * Selects a bank on the cpld device.
 * \returns 0 if ok, -1 if error.
 */
int cpldRboxSelect(int fd, __u8 bank);

/**
 * Writes a byte on the cpld device.
 * \returns 0 if ok, -1 if error.
 */
int cpldRboxWriteByte(int fd, __u8 cmd, __u8 data);

/**
 * Reads a byte on the cpld device.
 * \returns 0 if ok, -1 if error.
 */
int cpldRboxReadByte(int fd, __u8 cmd, __u8 *data);

#endif // CPLD_RBOX_H
