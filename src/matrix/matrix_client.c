#include <cJSON.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../utils/http.h"

#include "matrix_client.h"
#include "models/messages.h"
#include "models/login_response.h"

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

void RedMatrix_login(RedMatrix *self) {
    char data[1024];
    sprintf(data, "{\"token\": \"%s\", \"device_id\": \"DO_NOT_TRACK_THIS_DEVICE\", \"initial_device_display_name\": \"Reddit Web Client\", \"type\": \"com.reddit.token\"}", self->redditToken);

    HttpClientResult response = HttpClient_post(self->http_client, "/_matrix/client/v3/login", data);
    if (!response.success) {
        printf("Error: %s\n", response.error_message);
        return;
    }
    cJSON *root = cJSON_Parse(response.response_body);
    if (!root) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }
    LoginResponse *loginResponse = LoginResponse_new(root);
    self->loginResponse = loginResponse;

    struct curl_slist *headers = HttpClient_get_headers(self->http_client);

    char bearer[1024];
    size_t ret = snprintf(bearer, sizeof(bearer), "Authorization: Bearer %s", loginResponse->access_token);

    // Check if snprintf was successful
    if (ret >= sizeof(bearer)) {
        fprintf(stderr, "Error in function %s: snprintf output was truncated. Required %zu bytes but only %zu were available in the buffer.\n", __func__, ret, sizeof(bearer));
        return;
    }

    headers = curl_slist_append(headers, bearer);
    if (headers == NULL) {
        fprintf(stderr, "Error: curl_slist_append failed\n");
        return;
    }

    HttpClient_set_headers(self->http_client, headers);

    cJSON_Delete(root); 
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

MessageOrJson* RedMatrix_getRoomMessages(RedMatrix *self, const char *room_id, const char *from_token, bool json) {
    char path[256];
    if (from_token != NULL) {
        sprintf(path, "/_matrix/client/v3/rooms/%s/messages?dir=b&limit=100&from=%s&filter=%s", room_id, from_token, curl_easy_escape(self->http_client->curl, "{\"lazy_load_members\":true}", 0));
    } else {
        sprintf(path, "/_matrix/client/v3/rooms/%s/messages?dir=b&limit=100&filter=%s", room_id, curl_easy_escape(self->http_client->curl, "{\"lazy_load_members\":true}", 0));
    }
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
//    if (root) {
//      char *string = cJSON_Print(root);
//      if (string) {
//          printf("%s\n", string);
//          free(string);
//     }
//    }
    MessageOrJson *messageOrJson = malloc(sizeof(MessageOrJson));
    MessageResponse *messageResponse = parseMessageResponse(response.response_body);

    if (json) {
        messageOrJson->json = root;
        messageOrJson->response = NULL;
        messageOrJson->from_token = messageResponse->end;
        return messageOrJson;
    }

    messageOrJson->response = messageResponse;
    messageOrJson->json = NULL;
    messageOrJson->from_token = messageResponse->end;
    
    cJSON_Delete(root);
    return messageOrJson;
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
void RedMatrix_joinRoom(RedMatrix *self, const char *room_id) {
    char path[256];
    sprintf(path, "/_matrix/client/v3/rooms/%s/join", room_id);
    HttpClientResult response = HttpClient_post(self->http_client, path, "{}");
    if (!response.success) {
        printf("Error: %s\n", response.error_message);
        return;
    }
    cJSON *root = cJSON_Parse(response.response_body);
    if (!root) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }
    cJSON_Delete(root);
}

void RedMatrix_leaveRoom(RedMatrix *self, const char *room_id) {
    char path[256];
    sprintf(path, "/_matrix/client/v3/rooms/%s/leave", room_id);
    HttpClientResult response = HttpClient_post(self->http_client, path, "{}");
    if (!response.success) {
        printf("Error: %s\n", response.error_message);
        return;
    }
    printf("Response: %s\n", response.response_body);
    cJSON *root = cJSON_Parse(response.response_body);
    if (!root) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }
    cJSON_Delete(root);
}

void RedMatrix_downloadMxc(RedMatrix *self, const char *mxc_url, const char *filename) {
    char path[256];
    sprintf(path, "/_matrix/media/v3/download/%s", mxc_url);
    printf("Downloading: %s\n", path);
    HttpClientResult response = HttpClient_get(self->http_client, path);
    if (!response.success) {
        printf("Error: %s\n", response.error_message);
        return;
    }
    FILE *file = fopen(filename, "wb");
    if (file) {
        fwrite(response.response_body, 1, response.size, file);
        fclose(file);
    }
}

void RedMatrix_free(RedMatrix *self) {
    HttpClient_free(self->http_client);
    free(self);
}
