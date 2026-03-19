/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 317
 * Language: c
 * Block ID: 0af044bb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// http_request.c
#include "http_request.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_HEADERS 16

typedef struct {
    char key[32];
    char value[128];
} Header;

struct HttpRequest {
    char method[16];
    char url[256];
    Header headers[MAX_HEADERS];
    int header_count;
    char* body;

    // 构建器方法
    HttpRequestBuilder builder;
};

static HttpRequest* builder_set_method(HttpRequest* self, const char* method) {
    strncpy(self->method, method, sizeof(self->method) - 1);
    return self;
}

static HttpRequest* builder_set_url(HttpRequest* self, const char* url) {
    strncpy(self->url, url, sizeof(self->url) - 1);
    return self;
}

static HttpRequest* builder_set_header(HttpRequest* self,
                                       const char* key,
                                       const char* value) {
    if (self->header_count < MAX_HEADERS) {
        Header* h = &self->headers[self->header_count++];
        strncpy(h->key, key, sizeof(h->key) - 1);
        strncpy(h->value, value, sizeof(h->value) - 1);
    }
    return self;
}

static HttpRequest* builder_set_body(HttpRequest* self, const char* body) {
    if (self->body) free(self->body);
    self->body = strdup(body);
    return self;
}

static HttpRequest* builder_build(HttpRequest* self) {
    return self;
}

HttpRequestBuilder* http_request_builder(void) {
    HttpRequest* req = calloc(1, sizeof(HttpRequest));
    if (!req) return NULL;

    strcpy(req->method, "GET");

    req->builder.set_method = builder_set_method;
    req->builder.set_url = builder_set_url;
    req->builder.set_header = builder_set_header;
    req->builder.set_body = builder_set_body;
    req->builder.build = builder_build;

    return &req->builder;
}

// 宏简化链式调用
#define BUILDER_CALL(builder, method, ...) \
    (builder)->method((HttpRequest*)(builder), ##__VA_ARGS__)

// 使用示例
void builder_example(void) {
    HttpRequestBuilder* b = http_request_builder();
    HttpRequest* req = BUILDER_CALL(
        b, set_method, "POST"
    )->set_url(
        (HttpRequest*)b, "https://api.example.com/data"
    )->set_header(
        (HttpRequest*)b, "Content-Type", "application/json"
    )->set_header(
        (HttpRequest*)b, "Authorization", "Bearer token123"
    )->set_body(
        (HttpRequest*)b, "{\"key\":\"value\"}"
    )->build((HttpRequest*)b);

    http_request_execute(req);
    http_request_destroy(req);
}
