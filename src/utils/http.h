#ifndef HTTP_H
#define HTTP_H

#define DEBUG 0

#include <curl/curl.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  bool success;
  const char *error_message;
    char *response_body;
  // response size
  size_t size;
} HttpClientResult;

typedef struct {
    CURL *curl;
    struct curl_slist *headers;
    const char *base_url;
} HttpClient;

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
HttpClient* HttpClient_new(const char *base_url, struct curl_slist *headers);
HttpClientResult HttpClient_get(HttpClient *client, const char *path);
HttpClientResult HttpClient_post(HttpClient *client, const char *path, const char *data);
void HttpClient_free(HttpClient *client);
void HttpClient_set_headers(HttpClient *client, struct curl_slist *headers);
struct curl_slist* HttpClient_get_headers(HttpClient *client);

#ifdef __cplusplus
}
#endif

#endif // HTTP_H