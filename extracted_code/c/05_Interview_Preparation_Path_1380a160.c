/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\05_Interview_Preparation_Path.md
 * Line: 387
 * Language: c
 * Block ID: 1380a160
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// select/poll的问题：
// 1. 每次调用都要复制fd集合到内核
// 2. 遍历所有fd检查就绪状态 - O(n)
// 3. fd数量有限（select默认1024）

// epoll的改进：
// 1. epoll_ctl注册fd，只需一次拷贝
// 2. 使用红黑树存储fd，O(log n)查找
// 3. 使用就绪链表，直接返回就绪fd
// 4. 支持水平触发(LT)和边缘触发(ET)

// 代码示例：
int epoll_fd = epoll_create1(0);
struct epoll_event ev, events[MAX_EVENTS];

// 注册fd
ev.events = EPOLLIN | EPOLLET;  // ET模式
ev.data.fd = listen_fd;
epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

// 等待事件
int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
for (int i = 0; i < nfds; i++) {
    if (events[i].data.fd == listen_fd) {
        // 接受新连接
    } else {
        // 处理IO
    }
}

// 适用场景：
// - 大量连接，少量活跃（C10K问题）
// - 需要高性能网络服务器
