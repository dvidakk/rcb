#ifndef REDDIT_H
#define REDDIT_H

#include "../http.h"

typedef struct {
    char *access_token;
    char *refresh_token;
    char *token_type;
    double expires_in;
    char *scope;
} RedditToken;

typedef struct {
    char *clientId;
    char *clientSecret;
    char *username;
    char *password;
    char *user_agent;
    RedditToken *token;
    HttpClient *http_client;
} Reddit;

Reddit* Reddit_new(const char *clientId, const char *clientSecret, const char *username, const char *password, const char *user_agent);
void  Reddit_login(Reddit *self);
void Reddit_WhoAmI(Reddit *self);

char* Reddit_getToken(Reddit *self);

void Reddit_free(Reddit *self);

#endif // REDDIT_H