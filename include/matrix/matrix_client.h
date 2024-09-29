#ifndef MATRIX_CLIENT_H
#define MATRIX_CLIENT_H

#include <cjson/cJSON.h>

#include "utils/http.h"
#include "matrix/models/messages.h"
#include "matrix/models/login_response.h"

#define CONTENT_TYPE "Content-Type: application/json"
#define USER_AGENT   "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:123.0) Gecko/20100101 Firefox/123.0"
#define ORIGIN       "Origin: https://chat.reddit.com"
#define ACCEPT       "Accept: application/json"
#define CONNECTION   "Connection: keep-alive"


#ifdef __cplusplus 
extern "C" {
#endif

typedef struct {
    char *base_url;
    const char *redditToken;
    LoginResponse *loginResponse;
    HttpClient *http_client;
} RedMatrix;

RedMatrix* RedMatrix_new(const char *redditToken);

void RedMatrix_login(RedMatrix *self);
void RedMatrix_joinRoom(RedMatrix *self, const char *roomId);
void RedMatrix_leaveRoom(RedMatrix *self, const char *roomId);
void RedMatrix_getJoinedRooms(RedMatrix *self);
MessageResponse* RedMatrix_getRoomMessages(RedMatrix *self, const char *roomId, const char *from_token);

void RedMatrix_free(RedMatrix *self);

char* RedMatrix_getDisplayName(RedMatrix *self, const char *userId);
void RedMatrix_downloadMxc(RedMatrix *self, const char *mxcUrl, const char *filename);

#ifdef __cplusplus
}
#endif

#endif // MATRIX_CLIENT_H
