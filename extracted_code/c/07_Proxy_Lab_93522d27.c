/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\07_Proxy_Lab.md
 * Line: 527
 * Language: c
 * Block ID: 93522d27
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 线程函数声明
void *thread(void *vargp);

// 修改后的主函数
int main(int argc, char **argv) {
    int listenfd, *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    signal(SIGPIPE, SIG_IGN);

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = open_listenfd(argv[1]);
    if (listenfd < 0) {
        error_exit("open_listenfd failed");
    }

    while (1) {
        clientlen = sizeof(clientaddr);
        connfdp = malloc(sizeof(int));
        *connfdp = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);

        if (*connfdp < 0) {
            free(connfdp);
            continue;
        }

        // 创建新线程处理请求
        pthread_create(&tid, NULL, thread, connfdp);
    }

    return 0;
}

// 线程处理函数
void *thread(void *vargp) {
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self());
    free(vargp);

    doit(connfd);
    close(connfd);

    return NULL;
}
