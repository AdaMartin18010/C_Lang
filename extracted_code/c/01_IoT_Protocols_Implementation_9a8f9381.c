/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 4558
 * Language: c
 * Block ID: 9a8f9381
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file http_client.c
 * @brief HTTP客户端实现
 */

#include "http_client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* 方法字符串 */
static const char* method_strings[] = {
    "GET", "POST", "PUT", "DELETE", "HEAD", "PATCH"
};

/* 初始化客户端 */
int http_client_init(http_client_t *client, void *transport_ctx,
                      http_connect_t connect, http_send_t send,
                      http_recv_t recv, http_close_t close) {
    if (client == NULL || connect == NULL || send == NULL ||
        recv == NULL || close == NULL) {
        return HTTP_ERR_INVALID_PARAM;
    }

    memset(client, 0, sizeof(http_client_t));
    client->transport_ctx = transport_ctx;
    client->connect = connect;
    client->send = send;
    client->recv = recv;
    client->close = close;
    client->default_version = HTTP_1_1;
    client->timeout_ms = 10000;
    client->keep_alive = false;
    client->connected = false;

    return HTTP_ERR_OK;
}

/* 初始化请求 */
void http_request_init(http_request_t *req, http_method_t method, const char *url) {
    if (req == NULL || url == NULL) return;

    memset(req, 0, sizeof(http_request_t));
    req->method = method;
    req->version = HTTP_1_1;
    req->port = 80;

    /* 解析URL */
    http_parse_url(url, req->host, sizeof(req->host), &req->port,
                   req->path, sizeof(req->path));
}

/* 添加请求头 */
int http_request_add_header(http_request_t *req, const char *name, const char *value) {
    if (req == NULL || name == NULL || value == NULL) {
        return HTTP_ERR_INVALID_PARAM;
    }

    if (req->header_count >= HTTP_MAX_HEADERS) {
        return HTTP_ERR_NO_MEMORY;
    }

    http_header_t *hdr = &req->headers[req->header_count++];
    strncpy(hdr->name, name, sizeof(hdr->name) - 1);
    strncpy(hdr->value, value, sizeof(hdr->value) - 1);

    return HTTP_ERR_OK;
}

/* 设置请求体 */
int http_request_set_body(http_request_t *req, const uint8_t *data, size_t len) {
    if (req == NULL) return HTTP_ERR_INVALID_PARAM;
    if (len > HTTP_MAX_BODY_LEN) return HTTP_ERR_NO_MEMORY;

    if (data != NULL && len > 0) {
        memcpy(req->body, data, len);
        req->body_len = len;
    } else {
        req->body_len = 0;
    }

    return HTTP_ERR_OK;
}

/* 设置JSON请求体 */
int http_request_set_json_body(http_request_t *req, const char *json) {
    if (req == NULL || json == NULL) return HTTP_ERR_INVALID_PARAM;

    int err = http_request_set_body(req, (const uint8_t *)json, strlen(json));
    if (err == HTTP_ERR_OK) {
        strcpy(req->content_type, "application/json");
    }
    return err;
}

/* 构建HTTP请求 */
static int build_request(const http_request_t *req, uint8_t *buffer, size_t max_len) {
    int pos = 0;
    int n;

    /* 请求行 */
    n = snprintf((char *)buffer + pos, max_len - pos, "%s %s HTTP/1.1\r\n",
                 method_strings[req->method], req->path);
    if (n < 0 || (size_t)n >= max_len - pos) return HTTP_ERR_NO_MEMORY;
    pos += n;

    /* Host头 */
    if (req->port != 80) {
        n = snprintf((char *)buffer + pos, max_len - pos, "Host: %s:%d\r\n",
                     req->host, req->port);
    } else {
        n = snprintf((char *)buffer + pos, max_len - pos, "Host: %s\r\n", req->host);
    }
    pos += n;

    /* Content-Type */
    if (req->content_type[0] != '\0') {
        n = snprintf((char *)buffer + pos, max_len - pos, "Content-Type: %s\r\n",
                     req->content_type);
        pos += n;
    }

    /* Content-Length */
    if (req->body_len > 0) {
        n = snprintf((char *)buffer + pos, max_len - pos, "Content-Length: %zu\r\n",
                     req->body_len);
        pos += n;
    }

    /* 自定义头 */
    for (uint8_t i = 0; i < req->header_count; i++) {
        n = snprintf((char *)buffer + pos, max_len - pos, "%s: %s\r\n",
                     req->headers[i].name, req->headers[i].value);
        pos += n;
    }

    /* 空行 */
    n = snprintf((char *)buffer + pos, max_len - pos, "\r\n");
    pos += n;

    /* 请求体 */
    if (req->body_len > 0) {
        if (pos + req->body_len > max_len) return HTTP_ERR_NO_MEMORY;
        memcpy(buffer + pos, req->body, req->body_len);
        pos += (int)req->body_len;
    }

    return pos;
}

/* 执行请求 */
int http_client_request(http_client_t *client, const http_request_t *req,
                         http_response_t *resp) {
    if (client == NULL || req == NULL || resp == NULL) {
        return HTTP_ERR_INVALID_PARAM;
    }

    /* 连接服务器 */
    if (!client->connected) {
        if (client->connect(client->transport_ctx, req->host, req->port) != 0) {
            return HTTP_ERR_CONNECT_FAIL;
        }
        client->connected = true;
    }

    /* 构建请求 */
    int req_len = build_request(req, client->tx_buffer, sizeof(client->tx_buffer));
    if (req_len < 0) return req_len;

    /* 发送请求 */
    if (client->send(client->transport_ctx, client->tx_buffer, req_len) != req_len) {
        client->close(client->transport_ctx);
        client->connected = false;
        return HTTP_ERR_SEND_FAIL;
    }

    /* 接收响应 */
    int rx_len = 0;
    int total_len = 0;
    uint32_t start_time = 0; /* 应使用系统时间 */

    while (total_len < sizeof(client->rx_buffer) - 1) {
        rx_len = client->recv(client->transport_ctx,
                               client->rx_buffer + total_len,
                               sizeof(client->rx_buffer) - total_len - 1,
                               1000);
        if (rx_len > 0) {
            total_len += rx_len;
        } else {
            break;
        }
    }

    if (total_len == 0) {
        return HTTP_ERR_RECV_FAIL;
    }

    client->rx_buffer[total_len] = '\0';

    /* 解析响应 (简化实现) */
    char *p = (char *)client->rx_buffer;

    /* 解析状态行 */
    if (strncmp(p, "HTTP/1.1 ", 9) == 0 || strncmp(p, "HTTP/1.0 ", 9) == 0) {
        resp->version = (p[5] == '1') ? HTTP_1_1 : HTTP_1_0;
        resp->status_code = (uint16_t)atoi(p + 9);
    }

    /* 查找响应体 */
    char *body = strstr(p, "\r\n\r\n");
    if (body != NULL) {
        body += 4;
        resp->body_len = total_len - (body - (char *)client->rx_buffer);
        if (resp->body_len > HTTP_MAX_BODY_LEN) resp->body_len = HTTP_MAX_BODY_LEN;
        memcpy(resp->body, body, resp->body_len);
    }

    /* 如果不是keep-alive，关闭连接 */
    if (!client->keep_alive) {
        client->close(client->transport_ctx);
        client->connected = false;
    }

    return HTTP_ERR_OK;
}

/* GET请求 */
int http_get(http_client_t *client, const char *url, http_response_t *resp) {
    http_request_t req;
    http_request_init(&req, HTTP_GET, url);
    return http_client_request(client, &req, resp);
}

/* POST请求 */
int http_post(http_client_t *client, const char *url, const uint8_t *data,
               size_t len, const char *content_type, http_response_t *resp) {
    http_request_t req;
    http_request_init(&req, HTTP_POST, url);
    if (content_type) {
        strncpy(req.content_type, content_type, sizeof(req.content_type) - 1);
    }
    http_request_set_body(&req, data, len);
    return http_client_request(client, &req, resp);
}

/* POST JSON */
int http_post_json(http_client_t *client, const char *url, const char *json,
                    http_response_t *resp) {
    return http_post(client, url, (const uint8_t *)json, strlen(json),
                      "application/json", resp);
}

/* PUT请求 */
int http_put(http_client_t *client, const char *url, const uint8_t *data,
              size_t len, http_response_t *resp) {
    http_request_t req;
    http_request_init(&req, HTTP_PUT, url);
    http_request_set_body(&req, data, len);
    return http_client_request(client, &req, resp);
}

/* DELETE请求 */
int http_delete(http_client_t *client, const char *url, http_response_t *resp) {
    http_request_t req;
    http_request_init(&req, HTTP_DELETE, url);
    return http_client_request(client, &req, resp);
}

/* 解析URL */
int http_parse_url(const char *url, char *host, size_t host_len,
                    uint16_t *port, char *path, size_t path_len) {
    if (url == NULL || host == NULL || port == NULL || path == NULL) {
        return HTTP_ERR_INVALID_PARAM;
    }

    *port = 80;

    const char *p = url;

    /* 跳过协议部分 */
    if (strncmp(p, "http://", 7) == 0) {
        p += 7;
    } else if (strncmp(p, "https://", 8) == 0) {
        p += 8;
        *port = 443;
    }

    /* 提取host和port */
    const char *slash = strchr(p, '/');
    const char *colon = strchr(p, ':');

    size_t host_end;
    if (colon != NULL && (slash == NULL || colon < slash)) {
        /* 有端口号 */
        host_end = (size_t)(colon - p);
        if (host_end >= host_len) host_end = host_len - 1;
        strncpy(host, p, host_end);
        host[host_end] = '\0';

        *port = (uint16_t)atoi(colon + 1);

        if (slash != NULL) {
            p = slash;
        } else {
            p = "/";
        }
    } else {
        /* 无端口号 */
        if (slash != NULL) {
            host_end = (size_t)(slash - p);
        } else {
            host_end = strlen(p);
        }

        if (host_end >= host_len) host_end = host_len - 1;
        strncpy(host, p, host_end);
        host[host_end] = '\0';

        if (slash != NULL) {
            p = slash;
        } else {
            p = "/";
        }
    }

    /* 提取路径 */
    strncpy(path, p, path_len - 1);
    path[path_len - 1] = '\0';

    return HTTP_ERR_OK;
}

/* 错误码转字符串 */
const char* http_err_str(http_error_t err) {
    switch (err) {
        case HTTP_ERR_OK: return "OK";
        case HTTP_ERR_INVALID_PARAM: return "Invalid parameter";
        case HTTP_ERR_NO_MEMORY: return "No memory";
        case HTTP_ERR_CONNECT_FAIL: return "Connection failed";
        case HTTP_ERR_SEND_FAIL: return "Send failed";
        case HTTP_ERR_RECV_FAIL: return "Receive failed";
        case HTTP_ERR_TIMEOUT: return "Timeout";
        case HTTP_ERR_PARSE_FAIL: return "Parse failed";
        default: return "Unknown error";
    }
}
