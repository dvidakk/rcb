#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cJSON.h>

#include "reddit/reddit.h"
#include "utils/base64.h"
#include "utils/win_utils.h"

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
    self->http_client = HttpClient_new("https://oauth.reddit.com", NULL);
    return self;
}

struct curl_slist* create_headers(const char *user_agent, const char *clientId, const char *clientSecret) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, user_agent);

    char to_encode[1024];
    snprintf(to_encode, sizeof(to_encode), "%s:%s", clientId, clientSecret);
    char *auth_header = malloc(sizeof(char) * 1024);
    snprintf(auth_header, 1024, "Authorization: Basic %s", base64_encode(to_encode));

    headers = curl_slist_append(headers, auth_header);
    free(auth_header); // Free the dynamically allocated memory

    return headers;
}

struct curl_slist* create_oauth_headers(const char *user_agent, const char *auth_token) {
    struct curl_slist *headers = NULL;
    char auth_header[1024];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", auth_token);
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, user_agent);
    headers = curl_slist_append(headers, auth_header);
    return headers;
}


void Reddit_login(Reddit *self) {

    char data[1024];
    char user_agent[1024];
    snprintf(data, sizeof(data), "grant_type=password&username=%s&password=%s", self->username, self->password);

    snprintf(user_agent, sizeof(user_agent), "User-Agent: %s", self->user_agent);
    struct curl_slist *headers = create_headers(user_agent, self->clientId, self->clientSecret);
    
    HttpClient *http_client = HttpClient_new("https://www.reddit.com", headers);
    HttpClientResult response = HttpClient_post(http_client, "/api/v1/access_token", data);
    if (!response.success) {
        fprintf(stderr, "Error: %s\n", response.error_message);
        HttpClient_free(http_client);
        return;
    }

    cJSON *root = cJSON_Parse(response.response_body);
    if (!root) {
        fprintf(stderr, "Error before: [%s]\n", cJSON_GetErrorPtr());
        HttpClient_free(http_client);
        return;
    }

    self->token = malloc(sizeof(RedditToken));
    if (!self->token) {
        fprintf(stderr, "Failed to allocate memory for RedditToken\n");
        cJSON_Delete(root);
        HttpClient_free(http_client);
        return;
    }

    cJSON *access_token = cJSON_GetObjectItem(root, "access_token");
    cJSON *refresh_token = cJSON_GetObjectItem(root, "refresh_token");
    cJSON *token_type = cJSON_GetObjectItem(root, "token_type");
    cJSON *expires_in = cJSON_GetObjectItem(root, "expires_in");
    cJSON *scope = cJSON_GetObjectItem(root, "scope");

    struct curl_slist *oauth_headers = create_oauth_headers(user_agent, access_token->valuestring);
    HttpClient_set_headers(self->http_client, oauth_headers);
    
    if (access_token) self->token->access_token = strdup(access_token->valuestring);
    if (refresh_token) self->token->refresh_token = strdup(refresh_token->valuestring);
    if (token_type) self->token->token_type = strdup(token_type->valuestring);
    if (expires_in) self->token->expires_in = expires_in->valuedouble;
    if (scope) self->token->scope = strdup(scope->valuestring);
    cJSON_Delete(root);
    HttpClient_free(http_client);
}

void Reddit_WhoAmI(Reddit *self) {
    if (!self->token) {
        fprintf(stderr, "You need to login first\n");
        return;
    }
    HttpClientResult response = HttpClient_get(self->http_client, "/api/v1/me");
    
    if (!response.success) {
        fprintf(stderr, "Error: %s\n", response.error_message);
        HttpClient_free(self->http_client);
        return;
    }
    cJSON *root = cJSON_Parse(response.response_body);
    if (!root) {
        fprintf(stderr, "Error before: [%s]\n", cJSON_GetErrorPtr());
        HttpClient_free(self->http_client);
        return;
    }
    cJSON *name = cJSON_GetObjectItem(root, "name");
    if (name) {
        printf("Logged in as /u/%s\n", name->valuestring);
    }
    cJSON_Delete(root);
}
char* Reddit_getToken(Reddit *self) {
    return self->token->access_token;
}

void safe_free(void *ptr) {
    if (ptr) {
        free(ptr);
    }
}

void Reddit_free(Reddit *self) {
    safe_free(self->clientId);
    safe_free(self->clientSecret);
    safe_free(self->username);
    safe_free(self->password);
    if (self->token) {
        safe_free(self->token->access_token);
        safe_free(self->token->refresh_token);
        safe_free(self->token->token_type);
        safe_free(self->token->scope);
        free(self->token);
    }
	HttpClient_free(self->http_client);
    free(self);
}
