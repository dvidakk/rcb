#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cJSON.h>

#include "reddit.h"
#include "base64.h"

Reddit* Reddit_new(const char *clientId, const char *clientSecret, const char *username, const char *password, const char *user_agent){
    Reddit *self = malloc(sizeof(Reddit));
    if (!self) {
        fprintf(stderr, "Failed to allocate memory for Reddit\n");
        return NULL;
    }
    self->clientId = strdup(clientId);
    self->clientSecret = strdup(clientSecret);
    self->username = strdup(username);
    self->password = strdup(password);
    self->user_agent = strdup(user_agent);
    self->token = NULL;
    return self;
}
struct curl_slist* create_headers(const char *user_agent, const char *auth_header) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, user_agent);
    headers = curl_slist_append(headers, auth_header);
    return headers;
}

char* create_auth_header(const char *clientId, const char *clientSecret) {
    char to_encode[1024];
    snprintf(to_encode, sizeof(to_encode), "%s:%s", clientId, clientSecret);
    return base64_encode(to_encode);
}

void Reddit_login(Reddit *self) {

    char data[1024];
    char user_agent[1024];
    sprintf(data, "grant_type=password&username=%s&password=%s", self->username, self->password);

    char *auth_header = create_auth_header(self->clientId, self->clientSecret);
    snprintf(user_agent, sizeof(user_agent), "User-Agent: %s", self->user_agent);
    struct curl_slist *headers = create_headers(user_agent, auth_header);
    free(auth_header);
    
    HttpClient *http_client = HttpClient_new("https://www.reddit.com", headers);
    HttpClientResult response = HttpClient_post(http_client, "/api/v1/access_token", data);
    if (!response.success) {
        fprintf(stderr, "Error: %s\n", response.error_message);
        HttpClient_free(http_client);
        return NULL;
    }
    cJSON *root = cJSON_Parse(response.response_body);
    if (!root) {
        fprintf(stderr, "Error before: [%s]\n", cJSON_GetErrorPtr());
        HttpClient_free(http_client);
        return NULL;
    }
    self->token = malloc(sizeof(RedditToken));
    if (!self->token) {
        fprintf(stderr, "Failed to allocate memory for RedditToken\n");
        cJSON_Delete(root);
        HttpClient_free(http_client);
        return NULL;
    }
    cJSON *access_token = cJSON_GetObjectItem(root, "access_token");
    cJSON *refresh_token = cJSON_GetObjectItem(root, "refresh_token");
    cJSON *token_type = cJSON_GetObjectItem(root, "token_type");
    cJSON *expires_in = cJSON_GetObjectItem(root, "expires_in");
    cJSON *scope = cJSON_GetObjectItem(root, "scope");
    if (access_token) self->token->access_token = strdup(access_token->valuestring);
    if (refresh_token) self->token->refresh_token = strdup(refresh_token->valuestring);
    if (token_type) self->token->token_type = strdup(token_type->valuestring);
    if (expires_in) self->token->expires_in = expires_in->valuedouble;
    if (scope) self->token->scope = strdup(scope->valuestring);
    cJSON_Delete(root);
    HttpClient_free(http_client);
}

char* Reddit_getToken(Reddit *self) {
    return self->token->access_token;
}

void Reddit_free(Reddit *self) {
    if (self->clientId) free(self->clientId);
    if (self->clientSecret) free(self->clientSecret);
    if (self->username) free(self->username);
    if (self->password) free(self->password);
    if (self->token) {
        if (self->token->access_token) free(self->token->access_token);
        if (self->token->refresh_token) free(self->token->refresh_token);
        if (self->token->token_type) free(self->token->token_type);
        if (self->token->scope) free(self->token->scope);
        free(self->token);
    }
    free(self);
}