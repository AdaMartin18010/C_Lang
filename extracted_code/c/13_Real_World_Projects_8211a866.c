/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 1589
 * Language: c
 * Block ID: 8211a866
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * Static File Handler - Serve static files with proper MIME types
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <errno.h>
#include <time.h>
#include "static_handler.h"

static const mime_type_t mime_types[] = {
    {".html", "text/html"},
    {".htm", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".json", "application/json"},
    {".xml", "application/xml"},
    {".txt", "text/plain"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif", "image/gif"},
    {".svg", "image/svg+xml"},
    {".ico", "image/x-icon"},
    {".pdf", "application/pdf"},
    {".zip", "application/zip"},
    {".gz", "application/gzip"},
    {".mp3", "audio/mpeg"},
    {".mp4", "video/mp4"},
    {".webm", "video/webm"},
    {".woff", "font/woff"},
    {".woff2", "font/woff2"},
    {".ttf", "font/ttf"},
    {".otf", "font/otf"},
    {NULL, NULL}
};

const char* static_get_mime_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";

    for (const mime_type_t *m = mime_types; m->extension; m++) {
        if (strcasecmp(ext, m->extension) == 0) {
            return m->type;
        }
    }
    return "application/octet-stream";
}

bool static_file_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISREG(st.st_mode));
}

off_t static_get_file_size(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return -1;
    return st.st_size;
}

bool static_is_path_safe(const char *document_root, const char *request_path) {
    /* Prevent directory traversal */
    if (strstr(request_path, "..") != NULL) {
        return false;
    }

    /* Ensure path is under document root */
    char resolved_root[512];
    char resolved_path[512];

    if (!realpath(document_root, resolved_root)) {
        strncpy(resolved_root, document_root, sizeof(resolved_root) - 1);
    }

    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", document_root, request_path);

    if (!realpath(full_path, resolved_path)) {
        /* Path doesn't exist, check parent directory */
        char *last_slash = strrchr(full_path, '/');
        if (last_slash) {
            *last_slash = '\0';
            if (!realpath(full_path, resolved_path)) {
                return false;
            }
        }
    }

    size_t root_len = strlen(resolved_root);
    return (strncmp(resolved_path, resolved_root, root_len) == 0);
}

int static_serve_file(const char *path, http_response_t *resp) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    off_t size = static_get_file_size(path);
    if (size < 0) {
        close(fd);
        return -1;
    }

    /* Read small files into memory */
    if (size < 65536) {
        char *content = malloc((size_t)size + 1);
        if (!content) {
            close(fd);
            return -1;
        }

        ssize_t n = read(fd, content, (size_t)size);
        close(fd);

        if (n != size) {
            free(content);
            return -1;
        }

        http_set_body(resp, content, (size_t)size);
        free(content);  /* http_set_body makes a copy */
    } else {
        /* For large files, just set the file descriptor */
        close(fd);
        /* TODO: Implement sendfile support */
    }

    return 0;
}

int static_handle_request(const http_request_t *req, http_response_t *resp,
                          const static_config_t *config) {
    /* Validate path */
    if (!static_is_path_safe(config->document_root, req->uri)) {
        http_set_status(resp, HTTP_FORBIDDEN);
        http_set_body(resp, "<h1>403 Forbidden</h1>", 22);
        return -1;
    }

    /* Build file path */
    char path[1024];
    snprintf(path, sizeof(path), "%s%s", config->document_root, req->uri);

    /* Check if it's a directory */
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
        /* Try index.html */
        strncat(path, "/index.html", sizeof(path) - strlen(path) - 1);
    }

    /* Check file exists */
    if (!static_file_exists(path)) {
        return -1;  /* Let caller handle 404 */
    }

    /* Serve file */
    if (static_serve_file(path, resp) != 0) {
        http_set_status(resp, HTTP_INTERNAL_ERROR);
        http_set_body(resp, "<h1>500 Internal Server Error</h1>", 34);
        return -1;
    }

    /* Set response headers */
    http_set_status(resp, HTTP_OK);
    http_add_header(resp, "Content-Type", static_get_mime_type(path));

    /* Cache control */
    if (config->cache_max_age > 0) {
        char cache_header[64];
        snprintf(cache_header, sizeof(cache_header),
                "max-age=%d", config->cache_max_age);
        http_add_header(resp, "Cache-Control", cache_header);
    }

    /* Last-Modified */
    if (stat(path, &st) == 0) {
        char time_str[64];
        struct tm *gmt = gmtime(&st.st_mtime);
        strftime(time_str, sizeof(time_str),
                "%a, %d %b %Y %H:%M:%S GMT", gmt);
        http_add_header(resp, "Last-Modified", time_str);
    }

    return 0;
}
