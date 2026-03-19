/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\07_Proxy_Lab.md
 * Line: 493
 * Language: c
 * Block ID: dbc700e2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 返回错误页面给客户端
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg) {
    char buf[MAXLINE], body[MAXBUF];

    // 构建HTTP响应体
    sprintf(body, "<html><title>Proxy Error</title>");
    sprintf(body + strlen(body), "<body bgcolor="
ffffff\">\n");
    sprintf(body + strlen(body), "%s: %s\n", errnum, shortmsg);
    sprintf(body + strlen(body), "<p>%s: %s\n", longmsg, cause);
    sprintf(body + strlen(body), "<hr><em>The Proxy</em>\n");

    // 发送HTTP响应
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}

// 用户代理头
const char *user_agent_hdr = "Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
