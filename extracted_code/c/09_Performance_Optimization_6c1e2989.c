/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 1014
 * Language: c
 * Block ID: 6c1e2989
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化1: epoll代替select/poll
int epoll_fd = epoll_create1(0);
struct epoll_event ev, events[MAX_EVENTS];

ev.events = EPOLLIN;
ev.data.fd = listen_fd;
epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

while (1) {
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    for (int i = 0; i < nfds; i++) {
        if (events[i].data.fd == listen_fd) {
            // 接受新连接
        } else {
            // 处理I/O
        }
    }
}

// 优化2: 零拷贝发送
sendfile(out_fd, in_fd, &offset, count);

// 优化3: 内存池
void *pool_alloc(Pool *p, size_t size);
void pool_free(Pool *p, void *ptr);

// 优化4: 无锁队列
lockfree_queue_push(queue, item);
lockfree_queue_pop(queue, &item);
