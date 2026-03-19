/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\01_Creational_Patterns.md
 * Line: 225
 * Language: c
 * Block ID: 3ed043f0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* HTTP请求建造者 */

typedef struct {
    char method[16];
    char url[256];
    char headers[1024];
    char body[4096];
} http_request_t;

typedef struct {
    http_request_t* req;
} request_builder_t;

request_builder_t* builder_new(void) {
    request_builder_t* b = malloc(sizeof(request_builder_t));
    b->req = calloc(1, sizeof(http_request_t));
    strcpy(b->req->method, "GET");
    return b;
}

request_builder_t* builder_method(request_builder_t* b, const char* method) {
    strncpy(b->req->method, method, sizeof(b->req->method) - 1);
    return b;  /* 返回自身支持链式调用 */
}

request_builder_t* builder_url(request_builder_t* b, const char* url) {
    strncpy(b->req->url, url, sizeof(b->req->url) - 1);
    return b;
}

request_builder_t* builder_header(request_builder_t* b,
                                   const char* key,
                                   const char* value) {
    snprintf(b->req->headers + strlen(b->req->headers),
             sizeof(b->req->headers) - strlen(b->req->headers),
             "%s: %s\r\n", key, value);
    return b;
}

request_builder_t* builder_body(request_builder_t* b, const char* body) {
    strncpy(b->req->body, body, sizeof(b->req->body) - 1);
    return b;
}

http_request_t* builder_build(request_builder_t* b) {
    http_request_t* req = b->req;
    free(b);
    return req;
}

/* 使用 */
void example_builder(void) {
    http_request_t* req = builder_new()
        ->method("POST")
        ->url("https://api.example.com/users")
        ->header("Content-Type", "application/json")
        ->header("Authorization", "Bearer token")
        ->body("{\"name\":\"John\"}")
        ->build();
}
