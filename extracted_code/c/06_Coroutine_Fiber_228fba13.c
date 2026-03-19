/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\06_Coroutine_Fiber.md
 * Line: 495
 * Language: c
 * Block ID: 228fba13
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 使用协程实现异步IO */

typedef struct {
    int fd;
    coroutine* co;
} aio_request_t;

static aio_request_t* pending_aio[1024];
static int epoll_fd;

/* 异步读取 */
void async_read(int fd, void* buf, size_t len) {
    /* 注册到epoll */
    struct epoll_event ev = {
        .events = EPOLLIN,
        .data.ptr = current_co
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);

    /* 挂起协程 */
    co_yield();

    /* 恢复后执行读取 */
    read(fd, buf, len);
}

/* IO调度循环 */
void io_scheduler(void) {
    struct epoll_event events[64];

    while (1) {
        int n = epoll_wait(epoll_fd, events, 64, 0);

        /* 恢复就绪的协程 */
        for (int i = 0; i < n; i++) {
            coroutine* co = events[i].data.ptr;
            co_resume(co);
        }

        /* 运行其他就绪协程 */
        schedule();
    }
}

/* 示例 */
void http_handler(void* arg) {
    int client_fd = *(int*)arg;
    char buf[4096];

    /* 同步写法，实际异步执行 */
    async_read(client_fd, buf, sizeof(buf));
    process_request(buf);
    async_write(client_fd, response, response_len);

    close(client_fd);
}
