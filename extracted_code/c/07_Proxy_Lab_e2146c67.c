/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\07_Proxy_Lab.md
 * Line: 792
 * Language: c
 * Block ID: e2146c67
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void fetch_and_forward_cached(int clientfd, char *hostname, char *path,
                               char *port, rio_t *client_rio, char *url) {
    char cache_data[MAX_OBJECT_SIZE];
    int cache_size;

    // 先检查缓存
    if ((cache_size = cache_find(url, cache_data)) > 0) {
        printf("Cache hit for %s\n", url);
        Rio_writen(clientfd, cache_data, cache_size);
        return;
    }

    // 缓存未命中，从服务器获取
    int serverfd = open_clientfd(hostname, port);
    if (serverfd < 0) {
        clienterror(clientfd, hostname, "404", "Not found",
                    "Cannot connect to server");
        return;
    }

    // 构造并发送请求
    char new_request[MAXLINE];
    sprintf(new_request, "GET %s HTTP/1.0\r\n", path);
    sprintf(new_request + strlen(new_request), "Host: %s\r\n", hostname);
    sprintf(new_request + strlen(new_request), "User-Agent: %s", user_agent_hdr);
    sprintf(new_request + strlen(new_request), "Connection: close\r\n");
    sprintf(new_request + strlen(new_request), "Proxy-Connection: close\r\n\r\n");

    Rio_writen(serverfd, new_request, strlen(new_request));

    // 接收响应并缓存
    rio_t server_rio;
    Rio_readinitb(&server_rio, serverfd);

    char response_buf[MAX_OBJECT_SIZE];
    int total_size = 0;
    int n;

    while ((n = Rio_readlineb(&server_rio, response_buf + total_size,
                               MAX_OBJECT_SIZE - total_size)) > 0) {
        Rio_writen(clientfd, response_buf + total_size, n);
        total_size += n;
    }

    close(serverfd);

    // 添加到缓存
    if (total_size <= MAX_OBJECT_SIZE) {
        cache_insert(url, response_buf, total_size);
    }
}
