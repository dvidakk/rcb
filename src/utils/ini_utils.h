#ifndef INI_UTILS_H
#define INI_UTILS_H

typedef struct{
    char* client_id;
    char* client_secret;
    char* user;
    char* password;
    char* user_agent;
} Creds;

Creds* parseIniFile(char* filename);

#endif
