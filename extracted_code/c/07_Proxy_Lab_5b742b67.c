/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\07_Proxy_Lab.md
 * Line: 582
 * Language: c
 * Block ID: 5b742b67
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define NTHREADS  16
#define SBUFSIZE  1024

// 同步缓冲区
typedef struct {
    int *buf;
    int n;
    int front;
    int rear;
    sem_t mutex;
    sem_t slots;
    sem_t items;
} sbuf_t;

sbuf_t sbuf;

void sbuf_init(sbuf_t *sp, int n) {
    sp->buf = calloc(n, sizeof(int));
    sp->n = n;
    sp->front = sp->rear = 0;
    sem_init(&sp->mutex, 0, 1);
    sem_init(&sp->slots, 0, n);
    sem_init(&sp->items, 0, 0);
}

void sbuf_deinit(sbuf_t *sp) {
    free(sp->buf);
}

void sbuf_insert(sbuf_t *sp, int item) {
    sem_wait(&sp->slots);
    sem_wait(&sp->mutex);
    sp->buf[(++sp->rear) % (sp->n)] = item;
    sem_post(&sp->mutex);
    sem_post(&sp->items);
}

int sbuf_remove(sbuf_t *sp) {
    int item;
    sem_wait(&sp->items);
    sem_wait(&sp->mutex);
    item = sp->buf[(++sp->front) % (sp->n)];
    sem_post(&sp->mutex);
    sem_post(&sp->slots);
    return item;
}

// 工作者线程
void *worker_thread(void *vargp) {
    pthread_detach(pthread_self());

    while (1) {
        int connfd = sbuf_remove(&sbuf);
        doit(connfd);
        close(connfd);
    }
    return NULL;
}

// 预线程主函数
int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    signal(SIGPIPE, SIG_IGN);

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = open_listenfd(argv[1]);
    sbuf_init(&sbuf, SBUFSIZE);

    // 创建工作者线程池
    for (int i = 0; i < NTHREADS; i++)
        pthread_create(&tid, NULL, worker_thread, NULL);

    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
        if (connfd >= 0) {
            sbuf_insert(&sbuf, connfd);
        }
    }

    sbuf_deinit(&sbuf);
    return 0;
}
