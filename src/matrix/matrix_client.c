#include <cJSON.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "matrix_client.h"
#include "../http.h"
#include "models/messages.h"


LoginResponse* LoginResponse_new(cJSON *access_token, cJSON *home_server, cJSON *user_id, cJSON *device_id, cJSON *storage_version) {
    LoginResponse *self = malloc(sizeof(LoginResponse));
    self->access_token = access_token->valuestring;
    self->home_server = home_server->valuestring;
    self->user_id = user_id->valuestring;
    self->device_id = device_id->valuestring;
    self->storage_version = storage_version->valuestring;
    return self;
}

struct curl_slist* create_matrix_headers() {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, CONTENT_TYPE);
    headers = curl_slist_append(headers, USER_AGENT);
    headers = curl_slist_append(headers, ORIGIN);
    headers = curl_slist_append(headers, ACCEPT);
    headers = curl_slist_append(headers, CONNECTION);
    return headers;
}

RedMatrix* RedMatrix_new(const char *redditToken) {
    RedMatrix *self = malloc(sizeof(RedMatrix));
    self->base_url = "https://matrix.redditspace.com";
    self->redditToken = redditToken;
    self->loginResponse = NULL;
    
    struct curl_slist *headers = create_matrix_headers();
    self->http_client = HttpClient_new(self->base_url, headers);
    return self;
}

LoginResponse* RedMatrix_login(RedMatrix *self) {
    char data[1024];
    sprintf(data, "{\"token\": \"%s\", \"device_id\": \"DO_NOT_TRACK_THIS_DEVICE\", \"initial_device_display_name\": \"Reddit Web Client\", \"type\": \"com.reddit.token\"}", self->redditToken);

    HttpClientResult response = HttpClient_post(self->http_client, "/_matrix/client/v3/login", data);
    if (!response.success) {
        printf("Error: %s\n", response.error_message);
        return NULL;
    }
    cJSON *root = cJSON_Parse(response.response_body);
    if (!root) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return NULL;
    }
    LoginResponse *loginResponse = LoginResponse_new(cJSON_GetObjectItem(root, "access_token"), 
                                                     cJSON_GetObjectItem(root, "home_server"), 
                                                     cJSON_GetObjectItem(root, "user_id"), 
                                                     cJSON_GetObjectItem(root, "device_id"), 
                                                     cJSON_GetObjectItem(root, "com.reddit.storage_version"));
    self->loginResponse = loginResponse;
    // appends the access token like a bearer token
    struct curl_slist *headers = NULL;
    char bearer[256];
    sprintf(bearer, "Authorization: Bearer %s", loginResponse->access_token);
    headers = curl_slist_append(headers, bearer);
    HttpClient_set_headers(self->http_client, headers);

    cJSON_Delete(root);
    return loginResponse;
}

void RedMatrix_getJoinedRooms(RedMatrix *self) {
    HttpClientResult response = HttpClient_get(self->http_client, "/_matrix/client/v3/joined_rooms");
    if (!response.success) {
        printf("Error: %s\n", response.error_message);
        return;
    }
    cJSON *root = cJSON_Parse(response.response_body);
    if (!root) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }
    // resopnse is "{'joined_rooms': ['!GS8kU7qKTMydi5gBrShQtg:reddit.com']}
    cJSON *joined_rooms = cJSON_GetObjectItem(root, "joined_rooms");
    cJSON *room = cJSON_GetArrayItem(joined_rooms, 0);
    printf("Room: %s\n", room->valuestring);
    cJSON_Delete(root);
}

MessageResponse* RedMatrix_getRoomMessages(RedMatrix *self, const char *room_id) {
    char path[256];
    sprintf(path, "/_matrix/client/v3/rooms/%s/messages?dir=b&limit=100&filter=%s", room_id, curl_easy_escape(self->http_client->curl, "{\"lazy_load_members\":true}", 0));
    HttpClientResult response = HttpClient_get(self->http_client, path);
    if (!response.success) {
        printf("Error: %s\n", response.error_message);
        return;
    }
    cJSON *root = cJSON_Parse(response.response_body);

    if (!root) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }
    if (root) {
      char *string = cJSON_Print(root);
      if (string) {
          printf("%s\n", string);
          free(string);
      }
    }
    MessageResponse *messageResponse = parseMessageResponse(response.response_body);

    cJSON_Delete(root);
    return messageResponse;
}

// get displayname from user_id
char* RedMatrix_getDisplayName(RedMatrix *self, const char *user_id) {
    char path[256];
    sprintf(path, "/_matrix/client/v3/profile/%s/displayname", user_id);
    HttpClientResult response = HttpClient_get(self->http_client, path);
    if (!response.success) {
        printf("Error: %s\n", response.error_message);
        return NULL;
    }

    cJSON *root = cJSON_Parse(response.response_body);
    if (!root) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return NULL;
    }
    cJSON *displayname = cJSON_GetObjectItem(root, "displayname");
    char *displayname_str = displayname->valuestring;
    cJSON_Delete(root);
    return displayname_str;
}

void RedMatrix_free(RedMatrix *self) {
    HttpClient_free(self->http_client);
    free(self);
}
