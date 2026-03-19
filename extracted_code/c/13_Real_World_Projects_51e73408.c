/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 1124
 * Language: c
 * Block ID: 51e73408
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * HTTP Parser - RFC 7230 Compliant HTTP/1.1 Parser
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "http_parser.h"

static http_method_t parse_method(const char *str) {
    if (strcasecmp(str, "GET") == 0) return HTTP_GET;
    if (strcasecmp(str, "HEAD") == 0) return HTTP_HEAD;
    if (strcasecmp(str, "POST") == 0) return HTTP_POST;
    if (strcasecmp(str, "PUT") == 0) return HTTP_PUT;
    if (strcasecmp(str, "DELETE") == 0) return HTTP_DELETE;
    if (strcasecmp(str, "OPTIONS") == 0) return HTTP_OPTIONS;
    if (strcasecmp(str, "PATCH") == 0) return HTTP_PATCH;
    return HTTP_UNKNOWN;
}

static http_version_t parse_version(const char *str) {
    if (strcmp(str, "HTTP/1.0") == 0) return HTTP_1_0;
    if (strcmp(str, "HTTP/1.1") == 0) return HTTP_1_1;
    return HTTP_1_1;  /* Default to 1.1 */
}

const char* http_method_text(http_method_t method) {
    switch (method) {
        case HTTP_GET:     return "GET";
        case HTTP_HEAD:    return "HEAD";
        case HTTP_POST:    return "POST";
        case HTTP_PUT:     return "PUT";
        case HTTP_DELETE:  return "DELETE";
        case HTTP_OPTIONS: return "OPTIONS";
        case HTTP_PATCH:   return "PATCH";
        default:           return "UNKNOWN";
    }
}

const char* http_status_text(http_status_t status) {
    switch (status) {
        case HTTP_OK:                    return "OK";
        case HTTP_CREATED:               return "Created";
        case HTTP_NO_CONTENT:            return "No Content";
        case HTTP_MOVED_PERMANENTLY:     return "Moved Permanently";
        case HTTP_FOUND:                 return "Found";
        case HTTP_NOT_MODIFIED:          return "Not Modified";
        case HTTP_BAD_REQUEST:           return "Bad Request";
        case HTTP_UNAUTHORIZED:          return "Unauthorized";
        case HTTP_FORBIDDEN:             return "Forbidden";
        case HTTP_NOT_FOUND:             return "Not Found";
        case HTTP_METHOD_NOT_ALLOWED:    return "Method Not Allowed";
        case HTTP_REQUEST_TIMEOUT:       return "Request Timeout";
        case HTTP_PAYLOAD_TOO_LARGE:     return "Payload Too Large";
        case HTTP_URI_TOO_LONG:          return "URI Too Long";
        case HTTP_INTERNAL_ERROR:        return "Internal Server Error";
        case HTTP_NOT_IMPLEMENTED:       return "Not Implemented";
        case HTTP_BAD_GATEWAY:           return "Bad Gateway";
        case HTTP_SERVICE_UNAVAILABLE:   return "Service Unavailable";
        default:                         return "Unknown";
    }
}

void http_request_init(http_request_t *req) {
    memset(req, 0, sizeof(http_request_t));
    req->method = HTTP_UNKNOWN;
    req->version = HTTP_1_1;
    req->keep_alive = true;
}

void http_request_free(http_request_t *req) {
    if (req->body) {
        free(req->body);
        req->body = NULL;
    }
    req->body_len = 0;
}

const char* http_get_header(const http_request_t *req, const char *name) {
    for (size_t i = 0; i < req->header_count; i++) {
        if (strcasecmp(req->headers[i].name, name) == 0) {
            return req->headers[i].value;
        }
    }
    return NULL;
}

int http_parse_request(http_request_t *req, const char *data, size_t len) {
    const char *p = data;
    const char *end = data + len;

    /* Find end of request line (first \r\n) */
    const char *line_end = memmem(p, end - p, "\r\n", 2);
    if (!line_end) return -1;  /* Incomplete */

    /* Parse request line: METHOD URI HTTP/VERSION */
    char method_str[HTTP_MAX_METHOD_LEN];
    char version_str[16];

    int scanned = sscanf(p, "%15s %2047s %15s", method_str, req->uri, version_str);
    if (scanned != 3) return -1;

    req->method = parse_method(method_str);
    req->version = parse_version(version_str);

    p = line_end + 2;

    /* Parse headers */
    while (p < end) {
        /* Check for end of headers (empty line) */
        if (p + 2 <= end && memcmp(p, "\r\n", 2) == 0) {
            p += 2;
            break;
        }

        line_end = memmem(p, end - p, "\r\n", 2);
        if (!line_end) return -1;  /* Incomplete headers */

        /* Parse header line */
        const char *colon = memchr(p, ':', line_end - p);
        if (!colon) return -1;  /* Invalid header */

        if (req->header_count >= HTTP_MAX_HEADERS) {
            return -1;  /* Too many headers */
        }

        http_header_t *hdr = &req->headers[req->header_count++];
        size_t name_len = colon - p;
        size_t value_len = line_end - (colon + 1);

        /* Trim whitespace from value */
        const char *value_start = colon + 1;
        while (value_start < line_end && isspace((unsigned char)*value_start)) {
            value_start++;
        }
        while (line_end > value_start && isspace((unsigned char)*(line_end - 1))) {
            line_end--;
        }
        value_len = line_end - value_start;

        if (name_len >= HTTP_MAX_HEADER_NAME_LEN ||
            value_len >= HTTP_MAX_HEADER_VALUE_LEN) {
            req->header_count--;
            return -1;
        }

        memcpy(hdr->name, p, name_len);
        hdr->name[name_len] = '\0';

        memcpy(hdr->value, value_start, value_len);
        hdr->value[value_len] = '\0';

        /* Convert header name to lowercase for case-insensitive comparison */
        for (char *c = hdr->name; *c; c++) {
            *c = (char)tolower((unsigned char)*c);
        }

        p = line_end + 2;
    }

    /* Check for body */
    const char *content_len = http_get_header(req, "content-length");
    if (content_len) {
        req->content_length = (size_t)atoll(content_len);
        if (req->content_length > 0) {
            size_t body_received = end - p;
            if (body_received < req->content_length) {
                return -1;  /* Incomplete body */
            }

            req->body = malloc(req->content_length + 1);
            if (!req->body) return -1;

            memcpy(req->body, p, req->content_length);
            req->body[req->content_length] = '\0';
            req->body_len = req->content_length;
        }
    }

    /* Determine keep-alive */
    const char *conn = http_get_header(req, "connection");
    if (conn) {
        if (strcasecmp(conn, "close") == 0) {
            req->keep_alive = false;
        } else if (strcasecmp(conn, "keep-alive") == 0) {
            req->keep_alive = true;
        }
    } else {
        req->keep_alive = (req->version == HTTP_1_1);
    }

    return 0;
}

void http_response_init(http_response_t *resp) {
    memset(resp, 0, sizeof(http_response_t));
    resp->version = HTTP_1_1;
    resp->status = HTTP_OK;
    resp->keep_alive = true;
}

void http_response_free(http_response_t *resp) {
    if (resp->body) {
        free(resp->body);
        resp->body = NULL;
    }
    resp->body_len = 0;
}

void http_set_status(http_response_t *resp, http_status_t status) {
    resp->status = status;
}

void http_add_header(http_response_t *resp, const char *name, const char *value) {
    if (resp->header_count >= HTTP_MAX_HEADERS) return;

    http_header_t *hdr = &resp->headers[resp->header_count++];
    strncpy(hdr->name, name, HTTP_MAX_HEADER_NAME_LEN - 1);
    strncpy(hdr->value, value, HTTP_MAX_HEADER_VALUE_LEN - 1);
}

void http_set_body(http_response_t *resp, const char *body, size_t len) {
    if (resp->body) {
        free(resp->body);
    }

    resp->body = malloc(len + 1);
    if (resp->body) {
        memcpy(resp->body, body, len);
        resp->body[len] = '\0';
        resp->body_len = len;
    }
}

int http_response_serialize(const http_response_t *resp, char *buf, size_t buf_size) {
    int pos = 0;

    /* Status line */
    pos += snprintf(buf + pos, buf_size - pos, "HTTP/1.1 %d %s\r\n",
                   resp->status, http_status_text(resp->status));

    /* Headers */
    for (size_t i = 0; i < resp->header_count; i++) {
        pos += snprintf(buf + pos, buf_size - pos, "%s: %s\r\n",
                       resp->headers[i].name, resp->headers[i].value);
    }

    /* Content-Length if body present */
    if (resp->body_len > 0) {
        pos += snprintf(buf + pos, buf_size - pos, "Content-Length: %zu\r\n",
                       resp->body_len);
    }

    /* Connection header */
    if (resp->keep_alive) {
        pos += snprintf(buf + pos, buf_size - pos, "Connection: keep-alive\r\n");
    } else {
        pos += snprintf(buf + pos, buf_size - pos, "Connection: close\r\n");
    }

    /* Server header */
    pos += snprintf(buf + pos, buf_size - pos, "Server: CHTTPServer/1.0\r\n");

    /* Empty line */
    pos += snprintf(buf + pos, buf_size - pos, "\r\n");

    return pos;
}
