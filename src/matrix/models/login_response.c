#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

#include "matrix/models/login_response.h"


LoginResponse* LoginResponse_new(cJSON *logResponse) {
    LoginResponse *self = malloc(sizeof(LoginResponse));
    cJSON *access_token = cJSON_GetObjectItem(logResponse, "access_token");
    self->access_token = access_token ? access_token->valuestring : NULL;
    
    cJSON *home_server = cJSON_GetObjectItem(logResponse, "home_server");
    self->home_server = home_server ? home_server->valuestring : NULL;

    cJSON *user_id = cJSON_GetObjectItem(logResponse, "user_id");
    self->user_id = user_id ? user_id->valuestring : NULL;

    cJSON *device_id = cJSON_GetObjectItem(logResponse, "device_id");
    self->device_id = device_id ? device_id->valuestring : NULL;

    cJSON *storage_version = cJSON_GetObjectItem(logResponse, "com.reddit.storage_version");
    self->storage_version = storage_version ? storage_version->valuestring : NULL;
    return self;
}
