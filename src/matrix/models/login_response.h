#ifndef LOGIN_RESPONSE_H
#define LOGIN_RESPONSE_H

typedef struct {
    char *access_token;
    char *home_server;
    char *device_id;
    char *user_id;
    char *storage_version;
} LoginResponse;

LoginResponse* LoginResponse_new(cJSON *logResponse);

#endif // LOGIN_RESPONSE_H
