#ifndef SERVICE_H
#define SERVICE_H

#include <stdint.h>

int serviceManage(const char *service, const char *action);

int serviceGetNbRestart(const char *service, uint32_t* count);

int serviceSetTime(int64_t timestamp);

#endif // SERVICE_H