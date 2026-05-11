#include "http_parser.h"
#include <stdio.h>
#include <string.h>
int main(void) {
    const char *req =
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: curl/7.68\r\n"
        "Connection: close\r\n"
        "\r\n";
    HttpRequest parsed;
    int r = http_parse_request(&parsed, req, strlen(req));
    printf("result=%d method=%d uri=%s version=%s headers=%zu\n",
           r, parsed.method, parsed.uri, parsed.version, parsed.header_count);
    for (size_t i = 0; i < parsed.header_count; i++) {
        printf("  [%zu] '%s' = '%s'\n", i, parsed.headers[i].key, parsed.headers[i].value);
    }
    return 0;
}
