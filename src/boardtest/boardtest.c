#include "libyocto.h"
#include "libyocto_config.h"

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

#define WAIT_TIME_SECOND 2

#define SERVER_IP "192.168.1.146"

#define NTP_KEYS "\
1   1234\n\
2   12345\n\
10  123456789\n\
"

#define SSL_KEY "\
-----BEGIN PRIVATE KEY-----\n\
MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDHYerVdg0qcRtw\n\
oiQa/rZxE1V1YLVCQ8tqjOXQH3DjDvHE1quxpCE71ZcSYXWp6qd8MnSdQ158/53G\n\
tTxqosPnpA0rGUspiUyDDKi5uxW4qrxHQKH1zX9EnG5rYi6dK+fsYQ0U6kKlDwVC\n\
SIJgbTIWIIkLda1GPjiIXJRgtmfr2lmJJ0BUFyZB+92Mw76p8q3xt+FiCWOIywuC\n\
t72xltZJsTkdeOV5oijuSHAoJAb1foC66a70vXQXOZyKTlUYa7Vl9w678y0DNF12\n\
J1bV1Aek860fg7nI3uQmvzB3U3STI6BqGVShQV6eu3yM5/2zTBW09mejPUrcVUp5\n\
o5DPANdvAgMBAAECggEAYFASim7uSTQ+Fxib7FZidxjpzTucE5iAKMZLNVAazfQo\n\
Ife5FKS+URMKWRyvJBdwNHSDFHy4phUseBKQTsq1a645ljovrq7eQOfakW2oPn5r\n\
WNgF5KER+2i9hi31aoMNE4iGMzY+X1F0LuKr6DyXV0z8Job/3tCrqmNeYaF7bdg/\n\
wm3DXa7EL1qzKlv5KHlyS56PWUIwwTBW9XGeGnjAfqiQzExE56BfBzyf79CjZtV4\n\
lbqL7jJedl95d6SJWXXXEX+KT0ha64l1AIOgfz7YC2vwMqA0zfilH8Zj1E/PboBs\n\
dmHMl5nimfNaviFsysnNpF1xlNgFAGBC3up9jrdKgQKBgQDqM5e9+6QBHD4ijOv2\n\
x06O+fyiKpmAnjanemLvpZzDxjj3tHXc7v4274FnFof+LtsJXGbpc6W5e+L9wrZF\n\
GNe8FZkPYBaXUdQmawat3jxE0zYuDBliC5hzMnR7fbR5DK+bj42eLPRRQm3sWAmj\n\
SZFOdRhKYpL4GB1uNBl5pelsTwKBgQDZ8K2f6fjUuAiXIrfwu/usroa1ryLSmlp9\n\
osshC4ynuqBzDcnR/cwELTqMb/DqbHvphgtWeL69l7+dmy7uU5Yau32/EKsw8/0e\n\
n2lM3zWhQiyK0BGCiAGZyY0I9HshH+LcH7sn7YderdCCXeGtZOfAZA2+ZYKi6nkT\n\
UIKAZ1V64QKBgGmYe4l4ZVju0AEqAtZQxNXoVwUTKgT40PGwV91vkYAf5t2gqyfD\n\
RjGkyXaejRCJNc9Br1EBCK7Z8d2yphX/fn3aSxnInEb76SitRuSr+AacKKiYYfV3\n\
/vOpwyVO50lQ8/iywAqagEoHJqLSiv3plK+IUjxvt45O6S4gJYBY0yc3AoGAOB3z\n\
5whM7SER/TCQYmlVDYrG3wNgmQ/rmwwO98xMhGfWpql3PR33qV+oAr9ULliOs3uz\n\
camXm6P2T/PEatOVA/8hCtCU8/7vPzJW39ddrM8oPvoCeDTjDKFMMnEZP1dBh+KX\n\
w1kzERr6ZIcd9BLWZcCdlVf4AlETqfiCEeQJq6ECgYEAqfxRKAQq2TBarCkmNaXT\n\
GKvXmlGEqPjyWcors8L13aSRImbucaPFD3ueeswN2ZjYKsw1/bPADgSLlBFMvo+x\n\
N3YQXo3+9p0KJqJb9GxmRwvHmqVhMh+uKlGplo//fFYk9a6YD1myFeKhJB77v1t7\n\
JKWWnp351wpepYz1Ywera8w=\n\
-----END PRIVATE KEY-----\n\
"

#define SSL_CERT "\
-----BEGIN CERTIFICATE-----\n\
MIIC0zCCAbugAwIBAgIUOatu5KaPlBYQnD/3wGlQXr5dr7owDQYJKoZIhvcNAQEL\n\
BQAwEjEQMA4GA1UEAwwHeHVidW50dTAeFw0yMTAxMTExOTExMDVaFw0zMTAxMDkx\n\
OTExMDVaMBIxEDAOBgNVBAMMB3h1YnVudHUwggEiMA0GCSqGSIb3DQEBAQUAA4IB\n\
DwAwggEKAoIBAQDHYerVdg0qcRtwoiQa/rZxE1V1YLVCQ8tqjOXQH3DjDvHE1qux\n\
pCE71ZcSYXWp6qd8MnSdQ158/53GtTxqosPnpA0rGUspiUyDDKi5uxW4qrxHQKH1\n\
zX9EnG5rYi6dK+fsYQ0U6kKlDwVCSIJgbTIWIIkLda1GPjiIXJRgtmfr2lmJJ0BU\n\
FyZB+92Mw76p8q3xt+FiCWOIywuCt72xltZJsTkdeOV5oijuSHAoJAb1foC66a70\n\
vXQXOZyKTlUYa7Vl9w678y0DNF12J1bV1Aek860fg7nI3uQmvzB3U3STI6BqGVSh\n\
QV6eu3yM5/2zTBW09mejPUrcVUp5o5DPANdvAgMBAAGjITAfMAkGA1UdEwQCMAAw\n\
EgYDVR0RBAswCYIHeHVidW50dTANBgkqhkiG9w0BAQsFAAOCAQEArhqNkkH/9rrC\n\
lVWWkTwVWS1LMagWOcQ9knt7LqojZpQilgTewpYQfh/OxIEFb+JpM5lRSfcb6kn0\n\
LhJxtNpIBvDvmkxygOJVb8rwlU1HAcJB9gITqSyRvSKpS9GgiCvHf/P2kWBjSMYS\n\
H9VSUPEThog8ncc+jOVB2dkmz5Tev2xhhGPb5DIxeEktvcg0s+UwVG4tXFU19Ff4\n\
DP/RMS5xavUiPWm7pRO3SggnCrBVQ1ywl3QmqS+ipMKtWc0R6gQQV6HlgoUPQSkJ\n\
aIIcfDrPQHC09bi87CSZhGIZMTjT2LZTiqOR9hsP9L0oNdGfq1Z6UVzthmt+65gn\n\
+oMM3Aujig==\n\
-----END CERTIFICATE-----\n\
"

#define SSL_CA "\
-----BEGIN CERTIFICATE-----\n\
MIIDuDCCAqCgAwIBAgIQDPCOXAgWpa1Cf/DrJxhZ0DANBgkqhkiG9w0BAQUFADBI\n\
MQswCQYDVQQGEwJVUzEgMB4GA1UEChMXU2VjdXJlVHJ1c3QgQ29ycG9yYXRpb24x\n\
FzAVBgNVBAMTDlNlY3VyZVRydXN0IENBMB4XDTA2MTEwNzE5MzExOFoXDTI5MTIz\n\
MTE5NDA1NVowSDELMAkGA1UEBhMCVVMxIDAeBgNVBAoTF1NlY3VyZVRydXN0IENv\n\
cnBvcmF0aW9uMRcwFQYDVQQDEw5TZWN1cmVUcnVzdCBDQTCCASIwDQYJKoZIhvcN\n\
AQEBBQADggEPADCCAQoCggEBAKukgeWVzfX2FI7CT8rU4niVWJxB4Q2ZQCQXOZEz\n\
Zum+4YOvYlyJ0fwkW2Gz4BERQRwdbvC4u/jep4G6pkjGnx29vo6pQT64lO0pGtSO\n\
0gMdA+9tDWccV9cGrcrI9f4Or2YlSASWC12juhbDCE/RRvgUXPLIXgGZbf2IzIao\n\
wW8xQmxSPmjL8xk037uHGFaAJsTQ3MBv396gwpEWoGQRS0S8Hvbn+mPeZqx2pHGj\n\
7DaUaHp3pLHnDi+BeuK1cobvomuL8A/b01k/unK8RCSc43Oz969XL0Imnal0ugBS\n\
8kvNU3xHCzaFDmapCJcWNFfBZveA4+1wVMeT4C4oFVmHursCAwEAAaOBnTCBmjAT\n\
BgkrBgEEAYI3FAIEBh4EAEMAQTALBgNVHQ8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB\n\
/zAdBgNVHQ4EFgQUQjK2FvoE/f5dS3rD/fdMQB1aQ68wNAYDVR0fBC0wKzApoCeg\n\
JYYjaHR0cDovL2NybC5zZWN1cmV0cnVzdC5jb20vU1RDQS5jcmwwEAYJKwYBBAGC\n\
NxUBBAMCAQAwDQYJKoZIhvcNAQEFBQADggEBADDtT0rhWDpSclu1pqNlGKa7UTt3\n\
6Z3q059c4EVlew3KW+JwULKUBRSuSceNQQcSc5R+DCMh/bwQf2AQWnL1mA6s7Ll/\n\
3XpvXdMc9P+IBWlCqQVxyLesJugutIxq/3HcuLHfmbx8IVQr5Fiiu1cprp6poxkm\n\
D5kuCLDv/WnPmRoJjeOnnyvJNjR7JLN4TJUXpAYmHrZkUjZfYGfZnMUFdAvnZyPS\n\
CPyI6a6Lf+Ew9Dd+/cYy2i2eRDAwbO4H3tI0/NL/QPZL9GZGBlSm8jIKYyYwa5vR\n\
3ItHuuG51WLQoqD0ZwV4KWMabwTW+MZMo5qxN7SN5ShLHZ4swrhovO0C7jE=\n\
-----END CERTIFICATE-----\n\
"

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

#ifdef LIBYOCTO_SSL
int testSsl()
{
    messageSection("Ssl");

    int status = EXIT_SUCCESS;
    int rc;

    if ((rc = yoctoConfigSsl(SSL_KEY, SSL_CERT)) < 0)
    {
        status = EXIT_FAILURE;
    }

    message("ConfigSsl", 0, rc, "");

    return status;
}
#endif // LIBYOCTO_SSL

#ifdef LIBYOCTO_SNMP
int testSnmp()
{
    messageSection("Snmp");

    int status = EXIT_SUCCESS;
    int rc;

    if ((rc = yoctoConfigSnmp("raduser", "12345678", "12345678", "radsirius")) < 0)
    {
        status = EXIT_FAILURE;
    }

    message("ConfigSnmp", 0, rc, "");

    return status;
}
#endif //LIBYOCTO_SNMP

#ifdef LIBYOCTO_NTP
int testConfigNtp()
{
    messageSection("Ntp");

    int status = EXIT_SUCCESS;
    int rc;

    if ((rc = yoctoConfigNtpWrite(SERVER_IP, 10)) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigNtpWrite", 0, rc, "");

    if ((rc = yoctoConfigNtpKeysWrite(NTP_KEYS)) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigNtpKeysWrite", 0, rc, "");

    if ((rc = yoctoConfigNtpEnable(1)) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigNtpEnable", 0, rc, "Enable");

    if ((rc = yoctoConfigNtpEnabled()) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigNtpEnabled", 0, rc, "Enabled: %s", rc == 1 ? "yes" : "no");

    if ((rc = yoctoConfigNtpEnable(0)) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigNtpEnable", 0, rc, "Disable");

    if ((rc = yoctoConfigNtpEnabled()) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigNtpEnabled", 0, rc, "Enabled: %s", rc == 1 ? "yes" : "no");

    return status;
}
#endif // LIBYOCTO_NTP

#ifdef LIBYOCTO_REMOTE_JOURNAL
int testConfigRemoteJournal()
{
    messageSection("RemoteJournal");

    int status = EXIT_SUCCESS;
    int rc;

    if ((rc = yoctoConfigRemoteJournalWrite(SERVER_IP, SSL_CA)) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigRemoteJournalWrite", 0, rc, "");

    if ((rc = yoctoConfigRemoteJournalEnable(1)) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigRemoteJournalEnable", 0, rc, "Enable");

    if ((rc = yoctoConfigRemoteJournalEnabled()) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigRemoteJournalEnabled", 0, rc, "Enabled: %s", rc == 1 ? "yes" : "no");

    if ((rc = yoctoConfigRemoteJournalEnable(0)) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigRemoteJournalEnable", 0, rc, "Disable");

    if ((rc = yoctoConfigRemoteJournalEnabled()) < 0)
    {
        status = EXIT_FAILURE;
    }
    message("ConfigRemoteJournalEnabled", 0, rc, "Enabled: %s", rc == 1 ? "yes" : "no");

    return status;
}
#endif // LIBYOCTO_REMOTE_JOURNAL

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
#ifdef LIBYOCTO_SNMP
        status |= testSnmp();
#endif

#ifdef LIBYOCTO_SSL
        status |= testSsl();
#endif // LIBYOCTO_SSL

#ifdef LIBYOCTO_NTP
        status |= testConfigNtp();
#endif // LIBYOCTO_NTP

#ifdef LIBYOCTO_REMOTE_JOURNAL
        status |= testConfigRemoteJournal();
#endif // LIBYOCTO_REMOTE_JOURNAL
    }

    yoctoUninit();
    /*
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
       }
   */

    return status;
}
