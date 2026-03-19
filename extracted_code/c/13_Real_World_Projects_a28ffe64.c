/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 439
 * Language: c
 * Block ID: a28ffe64
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef CGI_HANDLER_H
#define CGI_HANDLER_H

#include "http_parser.h"
#include <stdbool.h>

#define CGI_TIMEOUT 30  /* seconds */
#define CGI_MAX_OUTPUT 10485760  /* 10MB */

typedef struct cgi_config {
    char cgi_path[512];
    char script_alias[128];
    int timeout;
    size_t max_output;
    char *env_vars[64];
    int env_count;
} cgi_config_t;

/* CGI handling */
int cgi_handle_request(const http_request_t *req, http_response_t *resp,
                       const cgi_config_t *config);
bool cgi_is_script_request(const char *uri, const cgi_config_t *config);
int cgi_execute(const char *script_path, const http_request_t *req,
                char **output, size_t *output_len, const cgi_config_t *config);

#endif /* CGI_HANDLER_H */
