/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\03_IO_Methods_Matrix.md
 * Line: 77
 * Language: c
 * Block ID: 2275ab71
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 事件驱动，单线程处理多连接
int epfd = epoll_create1(0);
struct epoll_event ev = {.events = EPOLLIN, .data.fd = sockfd};
epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
// 等待事件，而非等待数据
int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
