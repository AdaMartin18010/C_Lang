/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\01_Network_Server.md
 * Line: 142
 * Language: c
 * Block ID: c567157c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Linux epoll */
int epfd = epoll_create1(EPOLL_CLOEXEC);
struct epoll_event ev = { .events = EPOLLIN, .data.ptr = conn };
epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
epoll_wait(epfd, events, maxevents, timeout);

/* BSD/macOS kqueue */
int kq = kqueue();
struct kevent ev;
EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, conn);
kevent(kq, &ev, 1, NULL, 0, NULL);
kevent(kq, NULL, 0, events, nevents, &timeout);

/* Windows IOCP */
HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
CreateIoCompletionPort((HANDLE)sock, iocp, key, 0);
GetQueuedCompletionStatus(iocp, &bytes, &key, &ov, timeout);
