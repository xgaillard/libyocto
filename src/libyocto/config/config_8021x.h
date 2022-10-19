#ifndef CONFIG_8021X_H
#define CONFIG_8021X_H

int config8021xWrite(const char *identity, const char *privateKey, const char *privateKeyPassword, const char *certificate, const char *certificateAuthority);

#endif //CONFIG_8021X_H