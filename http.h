#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

CURL *curl;
CURLcode res;

char* perform_http_get(const char* url);
size_t allocate_buffer(void *ptr, size_t size, size_t nmemb, void *data);
