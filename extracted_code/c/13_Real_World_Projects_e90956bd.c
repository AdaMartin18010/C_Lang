/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 406
 * Language: c
 * Block ID: e90956bd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef STATIC_HANDLER_H
#define STATIC_HANDLER_H

#include "http_parser.h"
#include <stdbool.h>

typedef struct mime_type {
    const char *extension;
    const char *type;
} mime_type_t;

typedef struct static_config {
    char document_root[512];
    bool directory_listing;
    bool gzip_enabled;
    int cache_max_age;
} static_config_t;

/* Static file handling */
int static_handle_request(const http_request_t *req, http_response_t *resp,
                          const static_config_t *config);
bool static_file_exists(const char *path);
const char* static_get_mime_type(const char *path);
off_t static_get_file_size(const char *path);
int static_serve_file(const char *path, http_response_t *resp);
bool static_is_path_safe(const char *document_root, const char *request_path);

#endif /* STATIC_HANDLER_H */
