/*
 * Auto-generated from: 15_Memory_Safe_Languages\02_Migration_Strategies\01_Incremental_Migration.md
 * Line: 512
 * Language: c
 * Block ID: f1be1be1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// http_parser.c - 原始C实现，存在缓冲区溢出风险
#include <string.h>
#include <stdlib.h>

typedef struct {
    char method[16];
    char path[256];
    char version[16];
    int complete;
} http_request_t;

int parse_http_request(const char* data, size_t len, http_request_t* req) {
    // 危险: 没有边界检查!
    const char* space1 = strchr(data, ' ');
    if (!space1) return -1;

    strncpy(req->method, data, space1 - data);  // 可能溢出!
    req->method[space1 - data] = '\0';

    const char* space2 = strchr(space1 + 1, ' ');
    if (!space2) return -1;

    strncpy(req->path, space1 + 1, space2 - space1 - 1);  // 可能溢出!
    req->path[space2 - space1 - 1] = '\0';

    // ...
    return 0;
}
