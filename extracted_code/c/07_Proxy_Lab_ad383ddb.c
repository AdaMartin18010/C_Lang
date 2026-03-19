/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\07_Proxy_Lab.md
 * Line: 365
 * Language: c
 * Block ID: ad383ddb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 从服务器获取数据并转发给客户端
void fetch_and_forward(int clientfd, char *hostname, char *path,
                       char *port, rio_t *client_rio) {
    int serverfd;
    char buf[MAXLINE], new_request[MAXLINE];
    rio_t server_rio;

    // 连接目标服务器
    serverfd = open_clientfd(hostname, port);
    if (serverfd < 0) {
        clienterror(clientfd, hostname, "404", "Not found",
                    "Cannot connect to server");
        return;
    }

    // 构造新的HTTP请求
    sprintf(new_request, "GET %s HTTP/1.0\r\n", path);
    sprintf(new_request + strlen(new_request), "Host: %s\r\n", hostname);
    sprintf(new_request + strlen(new_request), "User-Agent: %s", user_agent_hdr);
    sprintf(new_request + strlen(new_request), "Connection: close\r\n");
    sprintf(new_request + strlen(new_request), "Proxy-Connection: close\r\n");

    // 转发其他请求头
    char line[MAXLINE];
    while (Rio_readlineb(client_rio, line, MAXLINE) > 0) {
        if (!strcmp(line, "\r\n")) break;
        if (strncasecmp(line, "Host:", 5) == 0) continue;
        if (strncasecmp(line, "User-Agent:", 11) == 0) continue;
        if (strncasecmp(line, "Connection:", 11) == 0) continue;
        if (strncasecmp(line, "Proxy-Connection:", 17) == 0) continue;
        strcat(new_request, line);
    }
    strcat(new_request, "\r\n");

    // 发送请求到服务器
    Rio_writen(serverfd, new_request, strlen(new_request));

    // 接收响应并转发给客户端
    Rio_readinitb(&server_rio, serverfd);
    int n;
    while ((n = Rio_readlineb(&server_rio, buf, MAXLINE)) != 0) {
        Rio_writen(clientfd, buf, n);
    }

    close(serverfd);
}
