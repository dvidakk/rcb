#ifndef INI_UTILS_H
#define INI_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    char* client_id;
    char* client_secret;
    char* user;
    char* password;
    char* user_agent;
} Creds;

Creds* parseIniFile(char* filename);

#ifdef __cplusplus
}
#endif

#endif
