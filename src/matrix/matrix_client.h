#ifndef MATRIX_CLIENT_H
#define MATRIX_CLIENT_H

#define CONTENT_TYPE "Content-Type: application/json"
#define USER_AGENT "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:123.0) Gecko/20100101 Firefox/123.0"
#define ORIGIN "Origin: https://chat.reddit.com"
#define ACCEPT "Accept: application/json"
#define CONNECTION "Connection: keep-alive"

#include <cJSON.h>
#include "../http.h"

typedef struct {
    char *access_token;
    char *home_server;
    char *device_id;
    char *user_id;
    char *storage_version;
} LoginResponse;

typedef struct {
    char *base_url;
    const char *redditToken;
    LoginResponse *loginResponse;
    HttpClient *http_client;
} RedMatrix;


LoginResponse* LoginResponse_new(cJSON *access_token, 
                                 cJSON *home_server, 
                                 cJSON *user_id, 
                                 cJSON *device_id, 
                                 cJSON *storage_version);

RedMatrix* RedMatrix_new(const char *redditToken);

LoginResponse* RedMatrix_login(RedMatrix *self);
void RedMatrix_getJoinedRooms(RedMatrix *self);
void RedMatrix_getRoomMessages(RedMatrix *self, const char *roomId);
void RedMatrix_free(RedMatrix *self);

char* RedMatrix_getDisplayName(RedMatrix *self, const char *userId);

#endif // MATRIX_CLIENT_H