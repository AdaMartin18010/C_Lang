/*
 * Auto-generated from: 12_Practice_Exercises\12_Mini_Projects.md
 * Line: 26
 * Language: c
 * Block ID: e859e70e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct HttpServer HttpServer;
typedef struct {
    const char *document_root;
    int port;
    int max_connections;
    bool enable_gzip;
} ServerConfig;

HttpServer* http_server_create(const ServerConfig *config);
void http_server_destroy(HttpServer *server);
int http_server_run(HttpServer *server);  // 阻塞
void http_server_stop(HttpServer *server);
