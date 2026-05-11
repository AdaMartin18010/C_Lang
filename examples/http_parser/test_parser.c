/*
 * HTTP 解析器单元测试
 * 编译: gcc -Wall -Wextra -O2 -std=c11 -o test_parser http_parser.c test_parser.c
 */
#include "http_parser.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

void test_get(void) {
    const char *req =
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: curl/7.68\r\n"
        "Connection: close\r\n"
        "\r\n";

    HttpRequest parsed;
    assert(http_parse_request(&parsed, req, strlen(req)) == 0);
    assert(parsed.method == HTTP_GET);
    assert(strcmp(parsed.uri, "/index.html") == 0);
    assert(strcmp(parsed.version, "HTTP/1.1") == 0);
    assert(parsed.header_count == 3);
    assert(strcmp(http_get_header(&parsed, "Host"), "localhost:8080") == 0);
    assert(parsed.keep_alive == false);
    printf("test_get: PASS\n");
}

void test_post_with_body(void) {
    const char *req =
        "POST /echo HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, World!";

    HttpRequest parsed;
    assert(http_parse_request(&parsed, req, strlen(req)) == 0);
    assert(parsed.method == HTTP_POST);
    assert(strcmp(parsed.uri, "/echo") == 0);
    assert(parsed.content_length == 13);
    assert(parsed.body_len == 13);
    assert(strncmp(parsed.body, "Hello, World!", 13) == 0);
    printf("test_post_with_body: PASS\n");
}

void test_response_format(void) {
    HttpResponse res;
    http_response_init(&res, 200);
    http_response_header(&res, "Content-Type", "text/plain");
    http_response_body(&res, "OK", 2);

    char buf[1024];
    int len = http_response_format(&res, buf, sizeof(buf));
    assert(len > 0);
    assert(strstr(buf, "HTTP/1.1 200 OK") != NULL);
    assert(strstr(buf, "Content-Length: 2") != NULL);
    assert(strstr(buf, "\r\n\r\nOK") != NULL);
    printf("test_response_format: PASS\n");
}

int main(void) {
    test_get();
    test_post_with_body();
    test_response_format();
    printf("\nAll parser tests passed!\n");
    return 0;
}
