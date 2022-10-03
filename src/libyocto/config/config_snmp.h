#ifndef CONFIG_SNMP_H
#define CONFIG_SNMP_H

int configSnmpWrite(const char* user, const char* auth, const char* priv, const char* view); 

#endif // CONFIG_SNMP_H