#include "libyocto.h"
#include "libyocto_config.h"

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

#define WAIT_TIME_SECOND 2

void message(const char *prefix, int wait, int result, const char *fmt, ...)
{
    char buffer[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    printf("%s: %d\t%s\n", prefix, result, buffer);
    fflush(stdout);

    if (wait != 0)
    {
        sleep(wait);
    }
}

void messageSection(const char *msg)
{
    printf("\n--- %s ---\n", msg);
}

#ifdef LIBYOCTO_WIFI
int testConfigWifi()
{
    messageSection("ConfigWifi");

    int status = EXIT_SUCCESS;
    int rc;
    char ssid[128], passphrase[63 + 1];
    const char *newSsid = "myNewSsid";
    const char *newPassphrase = "12345678";

    if ((rc = yoctoConfigWifiRead(ssid, sizeof(ssid), passphrase, sizeof(passphrase))) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigWifiRead", 0, rc, "ssid=%s pwd=%s", ssid, passphrase);

    if ((rc = yoctoConfigWifiWrite(newSsid, newPassphrase)) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigWifiWrite", 0, rc, "ssid=%s pwd=%s", newSsid, newPassphrase);

    if ((rc = yoctoConfigWifiRead(ssid, sizeof(ssid), passphrase, sizeof(passphrase))) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigWifiRead", 0, rc, "ssid=%s pwd=%s", ssid, passphrase);

    return status;
}
#endif // LIBYOCTO_WIFI

#ifdef LIBYOCTO_NETWORK
int testConfigNetwork()
{
    messageSection("ConfigNetwork");

    int status = EXIT_SUCCESS;
    int rc;
    char address[128], gateway[128];
    const char *newAddress = "10.0.0.10/24";
    const char *newGateway = "10.0.0.1";

    if ((rc = yoctoConfigNetworkRead("eth0", address, sizeof(address), gateway, sizeof(gateway))) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigNetworkRead", 0, rc, "address=%s gateway=%s", address, gateway);

    if ((rc = yoctoConfigNetworkWrite("eth0", newAddress, newGateway)) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigNetworkWrite", 0, rc, "address=%s gateway=%s", newAddress, newGateway);

    if ((rc = yoctoConfigNetworkRead("eth0", address, sizeof(address), gateway, sizeof(gateway))) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigNetworkRead", 0, rc, "address=%s gateway=%s", address, gateway);

    return status;
}
#endif // LIBYOCTO_NETWORK

int testServiceSetTime()
{
    messageSection("ServiceSetTime");

    int status = EXIT_SUCCESS;
    int rc;

    int64_t newTime = 1622816867000000ll;

    if ((rc = yoctoServiceSetTime(newTime)) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ServiceSetTime", 0, rc, "");

    return status;
}

int testServiceNbRestart()
{
    messageSection("ServiceNbRestart");

    int status = EXIT_SUCCESS;
    int rc;

    uint32_t nbRestart;
    const char *service = "c4s.service";

    if ((rc = yoctoServiceNbRestart(service, &nbRestart)) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ServiceNbRestart", 0, rc, "service=%s count=%d", service, nbRestart);

    return status;
}

int testServiceManage()
{
    messageSection("ServiceManage");

    int status = EXIT_SUCCESS;
    int rc;

    const char *service = "hostapd.service";

    if ((rc = yoctoServiceStart(service)) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ServiceStart", 0, rc, "service=%s", service);

    if ((rc = yoctoServiceRestart(service)) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ServiceRestart", 0, rc, "service=%s", service);

    if ((rc = yoctoServiceStop(service)) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ServiceStop", 0, rc, "service=%s", service);

    return status;
}

#ifdef LIBYOCTO_DIGITAL_INPUT
int testDigitalInput()
{
    messageSection("DigitalInput");

    int status = EXIT_SUCCESS;
    int rc;

    uint8_t data = 0;

    for (int i = 0; i < LIBYOCTO_DIGITAL_INPUT_COUNT; i++)
    {
        if ((rc = yoctoDigitalInputRead(i, &data)) != 0)
        {
            status = EXIT_FAILURE;
        }
        message("DigitalInputRead", 0, rc, "id=%d data=%d", i, data);
    }

    if ((rc = yoctoDigitalInputReadAll(&data)) != 0)
    {
        status = EXIT_FAILURE;
    }
    message("DigitalInputReadAll", 0, rc, "0X%02X", data);

    return status;
}
#endif // LIBYOCTO_DIGITAL_INPUT

#ifdef LIBYOCTO_DIGITAL_OUTPUT
int testDigitalOutput()
{
    messageSection("DigitalOutput");

    int status = EXIT_SUCCESS;
    int rc;

    for (int index = 0; index < LIBYOCTO_DIGITAL_OUTPUT_COUNT; index++)
    {
        if ((rc = yoctoDigitalOutputWrite(index, 1)) != 0)
        {
            status = EXIT_FAILURE;
        }
        message("DigitalOutputWrite", 0, rc, "id=%d data=1", index);
    }

    for (int index = 0; index < LIBYOCTO_DIGITAL_OUTPUT_COUNT; index++)
    {
        if ((rc = yoctoDigitalOutputWrite(index, 0)) != 0)
        {
            status = EXIT_FAILURE;
        }
        message("DigitalOutputWrite", 0, rc, "id=%d data=0", index);
    }

    uint8_t data = 0xFF;
    if ((rc = yoctoDigitalOutputWriteAll(data)) != 0)
    {
        status = EXIT_FAILURE;
    }
    message("DigitalOutputWriteAll", 0, rc, "data=0X%02X", data);

    if ((rc = yoctoDigitalOutputReadAll(&data)) != 0)
    {
        status = EXIT_FAILURE;
    }
    message("DigitalOutputReadAll", 0, rc, "data=0X%02X", data);

    for (int i = 0; i < LIBYOCTO_DIGITAL_OUTPUT_COUNT; i++)
    {
        if ((rc = yoctoDigitalOutputRead(i, &data)) != 0)
        {
            status = EXIT_FAILURE;
        }
        message("DigitalOutputRead", 0, rc, "id=%d data=%d", i, data);

        data = 0;
        if ((rc = yoctoDigitalOutputWrite(i, data)) != 0)
        {
            status = EXIT_FAILURE;
        }
        message("DigitalOutputWrite", 0, rc, "id=%d data=%d", i, data);

        if ((rc = yoctoDigitalOutputRead(i, &data)) != 0)
        {
            status = EXIT_FAILURE;
        }
        message("DigitalOutputRead", 0, rc, "id=%d data=%d", i, data);
    }

    if ((rc = yoctoDigitalOutputReadAll(&data)) != 0)
    {
        status = EXIT_FAILURE;
    }
    message("DigitalOutputReadAll", 0, rc, "data=0X%02X", data);

    return status;
}
#endif // LIBYOCTO_DIGITAL_OUTPUT

#ifdef LIBYOCTO_COMCONFIG_RBOX630
int testCom()
{
    messageSection("Com");

    int status = EXIT_SUCCESS;
    int rc;

    for (int port = 1; port <= 4; port++)
    {
        for (int type = 3; type >= 1; type--)
        {
            for (int term = 1; term >= 0; term--)
            {
                uint8_t comType = type;
                uint8_t comTerm = term;

                if ((rc = yoctoComConfigWrite(port, comType, comTerm)) != 0)
                {
                    status = EXIT_FAILURE;
                }
                message("ComConfigWrite", 0, rc, "id=%d type=%d term=%d", port, comType, comTerm);

                if ((rc = yoctoComConfigRead(port, &comType, &comTerm)) != 0)
                {
                    status = EXIT_FAILURE;
                }
                message("ComConfigRead", 0, rc, "id=%d type=%d term=%d", port, comType, comTerm);
            }
        }
    }

    return status;
}
#endif //  LIBYOCTO_COMCONFIG_RBOX630

int testLed()
{
    messageSection("Led");

    int status = EXIT_SUCCESS;
    int rc;

#ifdef LIBYOCTO_LED_WIFI
    if ((rc = yoctoLedWifiOn()) != 0)
    {
        status = EXIT_FAILURE;
    }
    message("LedWifiOn", WAIT_TIME_SECOND, rc, "");

    if ((rc = yoctoLedWifiOff()) != 0)
    {
        status = EXIT_FAILURE;
    }
    message("LedWifiOff", WAIT_TIME_SECOND, rc, "");
#endif // LIBYOCTO_LED_WIFI

#ifdef LIBYOCTO_LED_ALARM
    if ((rc = yoctoLedAlarmOn()) != 0)
    {
        status = EXIT_FAILURE;
    }
    message("LedAlarmOn", WAIT_TIME_SECOND, rc, "");

    if ((rc = yoctoLedAlarmOff()) != 0)
    {
        status = EXIT_FAILURE;
    }
    message("LedAlarmOff", WAIT_TIME_SECOND, rc, "");
#endif // LIBYOCTO_LED_ALARM

#ifdef LIBYOCTO_LED_STATUS
    if ((rc = yoctoLedStatusOn()) != 0)
    {
        status = EXIT_FAILURE;
    }
    message("LedStatusOn", WAIT_TIME_SECOND, rc, "");

    if ((rc = yoctoLedStatusBlink()) != 0)
    {
        status = EXIT_FAILURE;
    }
    message("LedStatusOff", 0, rc, "");
#endif // LIBYOCTO_LED_STATUS

    return status;
}

void printBinary(uint8_t data)
{
    for (int i = sizeof(char) * 7; i >= 0; i--)
        printf("%d", (data & (1 << i)) >> i);
    putc('\n', stdout);
}

int main()
{
    yoctoSetLogWarning();

    int status = EXIT_SUCCESS;
    /*
    #ifdef LIBYOCTO_NETWORK
       status = testConfigNetwork();
    #endif //LIBYOCTO_NETWORK

    #ifdef LIBYOCTO_WIFI
       status = testConfigWifi();
    #endif //LIBYOCTO_WIFI

       status = testServiceSetTime();
       status = testServiceNbRestart();
       status = testServiceManage();

    #ifdef LIBYOCTO_DIGITAL_INPUT_GPIO
       status = testDigitalInput();
    #endif //LIBYOCTO_DIGITAL_INPUT_GPIO

    #ifdef LIBYOCTO_DIGITAL_OUTPUT_GPIO
       status = testDigitalOutput();
    #endif //LIBYOCTO_DIGITAL_OUTPUT_GPIO

    #ifdef  LIBYOCTO_COMCONFIG_RBOX630
       status = testCom();
    #endif // LIBYOCTO_COMCONFIG_RBOX630

       status = testLed();

 #ifdef LIBYOCTO_DIGITAL_INPUT_GPIO
    status |= testDigitalInput();
 #endif // LIBYOCTO_DIGITAL_INPUT_GPIO

 #ifdef LIBYOCTO_DIGITAL_OUTPUT_GPIO
    status |= testDigitalOutput();
 #endif // LIBYOCTO_DIGITAL_OUTPUT_GPIO
    */

    status = yoctoInit();

    if (status >= 0)
    {
        printf("Enter x to exit\n");

        int index = 0;
        // 0b00000, 0b10000, 0b10001, 0b10010, 0b10100, 0b11000
        uint8_t dataToWrite[6] = {0x00, 0x10, 0x11, 0x12, 0x14, 0x18};
        while (1)
        {
            uint8_t data;

            printf("---INPUT---\n");
            if (yoctoDigitalInputReadAll(&data) == 0)
            {
                printBinary(data);
            }
            // printf("data=0X%02X", data);
            /*
                        for (int index = 0; index < 3; index++)
                        {
                            if (yoctoDigitalInputRead(index, &data) == 0)
                            {
                                printf("%d:%d\n", index, data);
                            }
                        }
                        */

            printf("---OUTPUT ALL---\n");

            if (yoctoDigitalOutputWriteAll(dataToWrite[index]) == 0)
            {
                printBinary(dataToWrite[index]);
            }
            if (yoctoDigitalOutputReadAll(&data) == 0)
            {
                printBinary(data);
            }

            printf("---OUTPUT SINGLE ON---\n");
            for (int index = 0; index < 5; index++)
            {
                yoctoDigitalOutputWrite(index, 1);
                if (yoctoDigitalOutputReadAll(&data) == 0)
                {
                    printBinary(data);
                }
            }
            printf("---OUTPUT SINGLE OFF---\n");
            for (int index = 0; index < 5; index++)
            {
                yoctoDigitalOutputWrite(index, 0);
                if (yoctoDigitalOutputReadAll(&data) == 0)
                {
                    printBinary(data);
                }
            }

            for (int index = 0; index < 5; index++)
            {
                yoctoDigitalOutputRead(index, &data);
                printf("%d=%d\n", index, data);
            }

            if (++index == sizeof(dataToWrite))
            {
                index = 0;
            }

            if (getchar() == 0x78)
            {
                break;
            }
        }

        yoctoUninit();
    }

    return status;
}
