/*
 * HTTP 协议解析器 - 工业实战项目
 * 头文件
 */
#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define HTTP_MAX_HEADERS 32
#define HTTP_MAX_URI 2048
#define HTTP_MAX_METHOD 16

typedef enum {
    HTTP_GET,
    HTTP_POST,
    HTTP_HEAD,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_UNKNOWN
} HttpMethod;

typedef struct {
    char key[64];
    char value[256];
} HttpHeader;

typedef struct {
    HttpMethod method;
    char uri[HTTP_MAX_URI];
    char version[16];
    HttpHeader headers[HTTP_MAX_HEADERS];
    size_t header_count;
    char *body;
    size_t body_len;
    size_t content_length;
    bool keep_alive;
} HttpRequest;

typedef struct {
    int status;
    char status_text[64];
    HttpHeader headers[HTTP_MAX_HEADERS];
    size_t header_count;
    const char *body;
    size_t body_len;
} HttpResponse;

/* 解析 */
int http_parse_request(HttpRequest *req, const char *data, size_t len);

/* 响应构建 */
void http_response_init(HttpResponse *res, int status);
void http_response_header(HttpResponse *res, const char *key, const char *value);
void http_response_body(HttpResponse *res, const char *body, size_t len);
int http_response_format(const HttpResponse *res, char *buf, size_t buf_size);

/* 工具 */
const char *http_get_header(const HttpRequest *req, const char *key);
const char *http_method_str(HttpMethod m);

#endif /* HTTP_PARSER_H */
