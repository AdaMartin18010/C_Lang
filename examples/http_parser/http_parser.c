/*
 * HTTP 协议解析器实现
 * 状态机驱动，零外部依赖
 * 编译: gcc -Wall -Wextra -O2 -std=c11 -c http_parser.c
 */
#include "http_parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 解析状态 */
typedef enum {
    PS_METHOD,
    PS_URI,
    PS_VERSION,
    PS_HEADER_KEY,
    PS_HEADER_VALUE,
    PS_BODY,
    PS_DONE,
    PS_ERROR
} ParseState;

static int parse_method(HttpRequest *req, const char *start, size_t len) {
    if (len >= 3 && memcmp(start, "GET", 3) == 0) req->method = HTTP_GET;
    else if (len >= 4 && memcmp(start, "POST", 4) == 0) req->method = HTTP_POST;
    else if (len >= 4 && memcmp(start, "HEAD", 4) == 0) req->method = HTTP_HEAD;
    else if (len >= 3 && memcmp(start, "PUT", 3) == 0) req->method = HTTP_PUT;
    else if (len >= 6 && memcmp(start, "DELETE", 6) == 0) req->method = HTTP_DELETE;
    else req->method = HTTP_UNKNOWN;
    return 0;
}

static void trim_crlf(char *s) {
    size_t len = strlen(s);
    while (len > 0 && (s[len-1] == '\r' || s[len-1] == '\n')) {
        s[len-1] = '\0';
        len--;
    }
}

int http_parse_request(HttpRequest *req, const char *data, size_t len) {
    memset(req, 0, sizeof(HttpRequest));
    req->keep_alive = true;

    ParseState state = PS_METHOD;
    const char *p = data;
    const char *end = data + len;
    const char *token_start = p;
    char line_buf[4096];
    size_t line_pos = 0;

    while (p <= end && state != PS_DONE && state != PS_ERROR) {
        char c = (p < end) ? *p : '\0';

        switch (state) {
            case PS_METHOD:
                if (c == ' ') {
                    parse_method(req, token_start, (size_t)(p - token_start));
                    state = PS_URI;
                    token_start = p + 1;
                }
                break;

            case PS_URI:
                if (c == ' ') {
                    size_t uri_len = (size_t)(p - token_start);
                    if (uri_len >= HTTP_MAX_URI) uri_len = HTTP_MAX_URI - 1;
                    memcpy(req->uri, token_start, uri_len);
                    req->uri[uri_len] = '\0';
                    state = PS_VERSION;
                    token_start = p + 1;
                }
                break;

            case PS_VERSION:
                if (c == '\r' || c == '\n' || c == '\0') {
                    size_t ver_len = (size_t)(p - token_start);
                    if (ver_len >= sizeof(req->version)) ver_len = sizeof(req->version) - 1;
                    memcpy(req->version, token_start, ver_len);
                    req->version[ver_len] = '\0';
                    trim_crlf(req->version);
                    state = PS_HEADER_KEY;
                    line_pos = 0;
                    if (c == '\r' && p + 1 < end && *(p+1) == '\n') p++;
                }
                break;

            case PS_HEADER_KEY:
                if (c == '\r' || c == '\n' || c == '\0') {
                    /* 空行 = 头部结束 */
                    if (line_pos == 0) {
                        state = PS_BODY;
                    } else {
                        /* 继续处理下一行 */
                        line_pos = 0;
                    }
                    if (c == '\r' && p + 1 < end && *(p+1) == '\n') p++;
                } else if (c == ':' && line_pos < sizeof(line_buf) - 1) {
                    line_buf[line_pos] = '\0';
                    /* 现在解析 key: value */
                    char *key = line_buf;
                    while (*key == ' ') key++;
                    p++; /* skip ':' */
                    while (p < end && (*p == ' ' || *p == '\t')) p++;
                    const char *val_start = p;
                    while (p < end && *p != '\r' && *p != '\n') p++;
                    size_t val_len = (size_t)(p - val_start);

                    if (req->header_count < HTTP_MAX_HEADERS) {
                        HttpHeader *h = &req->headers[req->header_count++];
                        strncpy(h->key, key, sizeof(h->key) - 1);
                        h->key[sizeof(h->key) - 1] = '\0';
                        if (val_len >= sizeof(h->value)) val_len = sizeof(h->value) - 1;
                        memcpy(h->value, val_start, val_len);
                        h->value[val_len] = '\0';
                        trim_crlf(h->value);

                        if (strcasecmp(h->key, "Content-Length") == 0) {
                            req->content_length = (size_t)strtoul(h->value, NULL, 10);
                        } else if (strcasecmp(h->key, "Connection") == 0) {
                            if (strcasecmp(h->value, "close") == 0) req->keep_alive = false;
                        }
                    }
                    state = PS_HEADER_KEY;
                    line_pos = 0;
                    if (p < end && *p == '\r' && p + 1 < end && *(p+1) == '\n') p++;
                    /* 不 continue，让外层 p++ 正常跳过 \n */
                } else if (line_pos < sizeof(line_buf) - 1) {
                    line_buf[line_pos++] = c;
                }
                break;

            case PS_BODY:
                if (req->content_length > 0) {
                    size_t body_avail = (size_t)(end - p);
                    size_t to_copy = (req->content_length < body_avail) ? req->content_length : body_avail;
                    req->body = malloc(to_copy + 1);
                    if (req->body) {
                        memcpy(req->body, p, to_copy);
                        req->body[to_copy] = '\0';
                        req->body_len = to_copy;
                    }
                    p += (ptrdiff_t)(to_copy - 1); /* -1 because loop increments */
                }
                state = PS_DONE;
                break;

            default:
                break;
        }
        p++;
    }

    return (state == PS_DONE) ? 0 : -1;
}

const char *http_get_header(const HttpRequest *req, const char *key) {
    for (size_t i = 0; i < req->header_count; i++) {
        if (strcasecmp(req->headers[i].key, key) == 0) {
            return req->headers[i].value;
        }
    }
    return NULL;
}

const char *http_method_str(HttpMethod m) {
    switch (m) {
        case HTTP_GET: return "GET";
        case HTTP_POST: return "POST";
        case HTTP_HEAD: return "HEAD";
        case HTTP_PUT: return "PUT";
        case HTTP_DELETE: return "DELETE";
        default: return "UNKNOWN";
    }
}

/* ===== 响应构建 ===== */

void http_response_init(HttpResponse *res, int status) {
    memset(res, 0, sizeof(HttpResponse));
    res->status = status;
    switch (status) {
        case 200: strcpy(res->status_text, "OK"); break;
        case 404: strcpy(res->status_text, "Not Found"); break;
        case 400: strcpy(res->status_text, "Bad Request"); break;
        case 500: strcpy(res->status_text, "Internal Server Error"); break;
        default: strcpy(res->status_text, "Unknown"); break;
    }
}

void http_response_header(HttpResponse *res, const char *key, const char *value) {
    if (res->header_count >= HTTP_MAX_HEADERS) return;
    HttpHeader *h = &res->headers[res->header_count++];
    strncpy(h->key, key, sizeof(h->key) - 1);
    strncpy(h->value, value, sizeof(h->value) - 1);
}

void http_response_body(HttpResponse *res, const char *body, size_t len) {
    res->body = body;
    res->body_len = len;
}

int http_response_format(const HttpResponse *res, char *buf, size_t buf_size) {
    int n = snprintf(buf, buf_size, "HTTP/1.1 %d %s\r\n", res->status, res->status_text);
    if (n < 0 || (size_t)n >= buf_size) return -1;
    size_t pos = (size_t)n;

    for (size_t i = 0; i < res->header_count; i++) {
        n = snprintf(buf + pos, buf_size - pos, "%s: %s\r\n",
                     res->headers[i].key, res->headers[i].value);
        if (n < 0 || (size_t)n >= buf_size - pos) return -1;
        pos += (size_t)n;
    }

    /* Content-Length */
    n = snprintf(buf + pos, buf_size - pos, "Content-Length: %zu\r\n", res->body_len);
    if (n < 0 || (size_t)n >= buf_size - pos) return -1;
    pos += (size_t)n;

    n = snprintf(buf + pos, buf_size - pos, "Connection: close\r\n\r\n");
    if (n < 0 || (size_t)n >= buf_size - pos) return -1;
    pos += (size_t)n;

    if (res->body && res->body_len > 0) {
        if (res->body_len + pos >= buf_size) return -1;
        memcpy(buf + pos, res->body, res->body_len);
        pos += res->body_len;
    }

    buf[pos] = '\0';
    return (int)pos;
}
