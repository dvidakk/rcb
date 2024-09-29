#ifndef LOGIN_RESPONSE_H
#define LOGIN_RESPONSE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char *access_token;
    char *home_server;
    char *device_id;
    char *user_id;
    char *storage_version;
} LoginResponse;

LoginResponse* LoginResponse_new(cJSON *logResponse);

#ifdef __cplusplus
}
#endif

#endif // LOGIN_RESPONSE_H
