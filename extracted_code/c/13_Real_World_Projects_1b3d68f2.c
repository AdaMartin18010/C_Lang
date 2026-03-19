/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 234
 * Language: c
 * Block ID: 1b3d68f2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define HTTP_MAX_METHOD_LEN 16
#define HTTP_MAX_URI_LEN 2048
#define HTTP_MAX_HEADER_NAME_LEN 256
#define HTTP_MAX_HEADER_VALUE_LEN 4096
#define HTTP_MAX_HEADERS 64
#define HTTP_MAX_BODY_LEN 10485760  /* 10MB */

typedef enum {
    HTTP_GET,
    HTTP_HEAD,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_OPTIONS,
    HTTP_PATCH,
    HTTP_UNKNOWN
} http_method_t;

typedef enum {
    HTTP_1_0,
    HTTP_1_1
} http_version_t;

typedef struct http_header {
    char name[HTTP_MAX_HEADER_NAME_LEN];
    char value[HTTP_MAX_HEADER_VALUE_LEN];
} http_header_t;

typedef struct http_request {
    http_method_t method;
    char uri[HTTP_MAX_URI_LEN];
    http_version_t version;
    http_header_t headers[HTTP_MAX_HEADERS];
    size_t header_count;
    char *body;
    size_t body_len;
    bool keep_alive;
    /* Parsing state */
    size_t content_length;
    bool chunked_transfer;
} http_request_t;

typedef enum {
    HTTP_OK = 200,
    HTTP_CREATED = 201,
    HTTP_NO_CONTENT = 204,
    HTTP_MOVED_PERMANENTLY = 301,
    HTTP_FOUND = 302,
    HTTP_NOT_MODIFIED = 304,
    HTTP_BAD_REQUEST = 400,
    HTTP_UNAUTHORIZED = 401,
    HTTP_FORBIDDEN = 403,
    HTTP_NOT_FOUND = 404,
    HTTP_METHOD_NOT_ALLOWED = 405,
    HTTP_REQUEST_TIMEOUT = 408,
    HTTP_PAYLOAD_TOO_LARGE = 413,
    HTTP_URI_TOO_LONG = 414,
    HTTP_INTERNAL_ERROR = 500,
    HTTP_NOT_IMPLEMENTED = 501,
    HTTP_BAD_GATEWAY = 502,
    HTTP_SERVICE_UNAVAILABLE = 503
} http_status_t;

typedef struct http_response {
    http_version_t version;
    http_status_t status;
    http_header_t headers[HTTP_MAX_HEADERS];
    size_t header_count;
    char *body;
    size_t body_len;
    bool keep_alive;
} http_response_t;

/* Request parsing */
int http_parse_request(http_request_t *req, const char *data, size_t len);
void http_request_init(http_request_t *req);
void http_request_free(http_request_t *req);
const char* http_get_header(const http_request_t *req, const char *name);

/* Response building */
void http_response_init(http_response_t *resp);
void http_response_free(http_response_t *resp);
void http_set_status(http_response_t *resp, http_status_t status);
void http_add_header(http_response_t *resp, const char *name, const char *value);
void http_set_body(http_response_t *resp, const char *body, size_t len);
int http_response_serialize(const http_response_t *resp, char *buf, size_t buf_size);
const char* http_status_text(http_status_t status);
const char* http_method_text(http_method_t method);

#endif /* HTTP_PARSER_H */
