#pragma warning(push, 0)
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "curls.h"
#include <curl/curl.h>
#include "msg.h"
#include "hls.h"

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        MSG_ERROR("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

size_t get_data_from_url(const char *url, char **str, uint8_t **bin, int type,std::string user_agent)
{
    CURL *c;
    CURLcode res;
    char * curl_url= strdup(url);
    curl_url[strcspn(curl_url, "\r")] = '\0';

    struct MemoryStruct chunk;

    chunk.memory = (char *) malloc(1);
    chunk.size = 0;
    c = curl_easy_init();
    curl_easy_setopt(c, CURLOPT_URL, curl_url);
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(c, CURLOPT_USERAGENT, user_agent.c_str());
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
    res = curl_easy_perform(c);

    if (res != CURLE_OK) {
        MSG_ERROR("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
        if (type == STRING) {
            *str = strdup(chunk.memory);
        } else if (type == BINKEY) {
            *bin = (uint8_t *)malloc(KEYLEN);
            *bin = (uint8_t *)memcpy(*bin, chunk.memory, KEYLEN);
        } else if (type == BINARY) {
            *bin = (uint8_t *)malloc(chunk.size);
            *bin = (uint8_t *)memcpy(*bin, chunk.memory, chunk.size);
        }
    }
    curl_easy_cleanup(c);
    if (chunk.memory) {
        free(chunk.memory);
    }

    return chunk.size;
}
