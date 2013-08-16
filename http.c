#include "http.h"


char* perform_http_get(const char* url){
    char* buf = NULL;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, allocate_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);
    res = curl_easy_perform(curl);
    if(res != CURLE_OK){
        fputs("curl failed", stderr);
        buf = NULL;
    }
    curl_easy_cleanup(curl);
    return buf;
}

size_t allocate_buffer(void *ptr, size_t size, size_t nmemb, void *data){
    size_t count = (size_t)(size*nmemb);
    char** buf = ((char **) data);
    if (*buf == NULL){
        *buf = strndup(ptr, count); 
    }else {
        *buf = realloc(*buf, (size_t) (strlen(*buf) + count + 1));
        strncat(*buf, ptr, count);
    }
    return count;
}

