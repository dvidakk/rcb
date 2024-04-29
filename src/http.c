#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <cJSON.h>

#include "http.h"

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t total_size = size * nmemb;
  char **response_ptr = (char**)userp;

  // Pre-allocate a larger buffer to avoid realloc overhead
  size_t new_size = strlen(*response_ptr) + total_size + 1;
  *response_ptr = (char*)realloc(*response_ptr, new_size);
  if (*response_ptr == NULL)
    return 0; // signal error

  strncat(*response_ptr, (const char*)contents, total_size);
  return total_size;
}

HttpClient* HttpClient_new(const char *base_url, struct curl_slist *headers) {
  HttpClientResult result = {true, NULL};

  if(DEBUG) printf("Creating new HttpClient for URL: %s\n", base_url);

  HttpClient *client = (HttpClient*)malloc(sizeof(HttpClient));
  if (client == NULL) {
    fprintf(stderr, "Failed to allocate memory for HttpClient\n");
    exit(1);
  }
 
  client->curl = curl_easy_init();
    if (client->curl == NULL) {
        fprintf(stderr, "Failed to initialize CURL\n");
        exit(1);
    }

  curl_easy_setopt(client->curl, CURLOPT_URL, base_url);
  client->headers = headers;
  client->base_url = base_url;

  return client;
}
// Set the headers for the HttpClient
void HttpClient_set_headers(HttpClient *client, struct curl_slist *headers) {
    client->headers = headers;
}
HttpClientResult HttpClient_get(HttpClient *client, const char *path) {
  HttpClientResult result = {true, NULL};

  if(DEBUG) printf("Performing GET request to path: %s\n", path);

  char full_request_url[256];
  snprintf(full_request_url, sizeof(full_request_url), "%s%s", client->base_url, path);

  curl_easy_setopt(client->curl, CURLOPT_URL, full_request_url);
  curl_easy_setopt(client->curl, CURLOPT_HTTPGET, 1L);
  curl_easy_setopt(client->curl, CURLOPT_HTTPHEADER, client->headers);
  if(DEBUG) printf("headers: %s\n", client->headers);

  char *response_body = (char*)malloc(1024); // Pre-allocate memory
  if (response_body == NULL) {
    result.success = false;
    result.error_message = "Memory allocation failed";
    return result;
  }
  response_body[0] = '\0'; // Make it an empty string

  curl_easy_setopt(client->curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(client->curl, CURLOPT_WRITEDATA, &response_body);

  CURLcode res = curl_easy_perform(client->curl);
  if (res != CURLE_OK) {
    free(response_body);
    result.success = false;
    result.error_message = curl_easy_strerror(res);
    return result;
  }
  if(DEBUG)  printf("Received response: %s\n", response_body);
  // Parse the response body into a cJSON object
  cJSON *json = cJSON_Parse(response_body);

  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    free(response_body);
    result.success = false;
    result.error_message = "Failed to parse JSON response";
    return result;
  }

  // Convert the cJSON object to a string (optional, depending on use case)
  char *json_string = cJSON_Print(json);
    result.response_body = json_string;
    // set size to the size of the response 
    result.size = strlen(json_string);
  // Clean up
  cJSON_Delete(json);
    free(response_body);

    return result;
}

HttpClientResult HttpClient_post(HttpClient *client, const char *path, const char *data) {
  HttpClientResult result = {true, NULL};

  if (DEBUG)  printf("Performing POST request to path: %s with data: %s\n", path, data);

  char full_request_url[256];
  snprintf(full_request_url, sizeof(full_request_url), "%s%s", client->base_url, path);

  curl_easy_setopt(client->curl, CURLOPT_URL, full_request_url);
  curl_easy_setopt(client->curl, CURLOPT_POST, 1L);
  curl_easy_setopt(client->curl, CURLOPT_POSTFIELDS, data);
  curl_easy_setopt(client->curl, CURLOPT_HTTPHEADER, client->headers);
  if(DEBUG) printf("headers: %s\n", client->headers);

  char *response_body = (char*)malloc(1024); // Pre-allocate memory
  if (response_body == NULL) {
    result.success = false;
    result.error_message = "Memory allocation failed";
    return result;
  }
  response_body[0] = '\0'; // Make it an empty string

  curl_easy_setopt(client->curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(client->curl, CURLOPT_WRITEDATA, &response_body);

  CURLcode res = curl_easy_perform(client->curl);
  if (res != CURLE_OK) {
    free(response_body);
    result.success = false;
    result.error_message = curl_easy_strerror(res);
    return result;
  }
  if (DEBUG) printf("Received response: %s\n", response_body);

    // Parse the response body into a cJSON object
    cJSON *json = cJSON_Parse(response_body);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        free(response_body);
        result.success = false;
        result.error_message = "Failed to parse JSON response";
        return result;
    }

    // Convert the cJSON object to a string (optional, depending on use case)
    char *json_string = cJSON_Print(json);
    result.response_body = json_string;
    // Clean up
    cJSON_Delete(json);
    free(response_body);

    return result;
}

void HttpClient_free(HttpClient *client) {
    curl_easy_cleanup(client->curl);
    free(client);
}
