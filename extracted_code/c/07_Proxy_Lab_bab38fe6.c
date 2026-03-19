/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\07_Proxy_Lab.md
 * Line: 301
 * Language: c
 * Block ID: bab38fe6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 处理单个HTTP事务
void doit(int fd) {
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE], path[MAXLINE], port[MAXLINE];
    rio_t rio;

    // 读取请求行
    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE))
        return;

    printf("Request: %s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);

    // 只支持GET方法
    if (strcasecmp(method, "GET")) {
        clienterror(fd, method, "501", "Not Implemented",
                    "Proxy does not implement this method");
        return;
    }

    // 解析URI
    parse_uri(uri, hostname, path, port);
    printf("Parsed: host=%s, path=%s, port=%s\n", hostname, path, port);

    // 转发请求到目标服务器
    fetch_and_forward(fd, hostname, path, port, &rio);
}

// 解析URI
void parse_uri(char *uri, char *hostname, char *path, char *port) {
    char *ptr;

    // 跳过 http:// 或 https://
    if ((ptr = strstr(uri, "://")) != NULL) {
        uri = ptr + 3;
    }

    // 提取主机名
    ptr = strchr(uri, '/');
    if (ptr) {
        strcpy(path, ptr);
        *ptr = '\0';
        strcpy(hostname, uri);
    } else {
        strcpy(hostname, uri);
        strcpy(path, "/");
    }

    // 检查端口号
    ptr = strchr(hostname, ':');
    if (ptr) {
        *ptr = '\0';
        strcpy(port, ptr + 1);
    } else {
        strcpy(port, "80");
    }
}
