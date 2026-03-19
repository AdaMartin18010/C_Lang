/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 289
 * Language: c
 * Block ID: 257a2a14
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// http_request.h
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

typedef struct HttpRequest HttpRequest;

// 构建器接口
typedef struct {
    HttpRequest* (*set_method)(HttpRequest* self, const char* method);
    HttpRequest* (*set_url)(HttpRequest* self, const char* url);
    HttpRequest* (*set_header)(HttpRequest* self,
                               const char* key,
                               const char* value);
    HttpRequest* (*set_body)(HttpRequest* self, const char* body);
    HttpRequest* (*build)(HttpRequest* self);
} HttpRequestBuilder;

// 获取构建器
HttpRequestBuilder* http_request_builder(void);

// 请求操作
void http_request_execute(const HttpRequest* req);
void http_request_destroy(HttpRequest* req);

#endif
