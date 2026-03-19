/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\02_Intermediate_Learning_Path.md
 * Line: 520
 * Language: c
 * Block ID: 97fc7476
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tinyhttpd.c - 功能完整的HTTP服务器

// 功能需求：
// 1. HTTP/1.1协议支持
// 2. 支持GET/POST/HEAD方法
// 3. 静态文件服务
// 4. CGI支持（执行外部程序）
// 5. 虚拟主机支持
// 6. 访问日志
// 7. 配置文件支持

// 核心数据结构
typedef struct {
    char method[16];
    char path[256];
    char version[16];
    char headers[20][256];
    int header_count;
    char *body;
    size_t body_length;
} HttpRequest;

typedef struct {
    int status_code;
    char status_text[64];
    char headers[20][256];
    int header_count;
    char *body;
    size_t body_length;
} HttpResponse;

// 核心函数
int parse_http_request(const char *raw, HttpRequest *req);
int build_http_response(HttpResponse *res, char *buf, size_t buf_size);
int serve_static_file(HttpRequest *req, HttpResponse *res, const char *root);
int serve_cgi(HttpRequest *req, HttpResponse *res, const char *cgi_path);
