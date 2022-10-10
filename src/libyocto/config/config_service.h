#ifndef CONFIG_SERVICE_H
#define CONFIG_SERVICE_H

typedef enum {
    CONFIG_SERVICE_NTP,
    CONFIG_SERVICE_JOURNAL
    //CONFIG_SERVICE_8021X
} ConfigService;

int configServiceEnable(ConfigService type, int enable);

int configServiceIsEnabled(ConfigService type);

#endif // CONFIG_SERVICE_H