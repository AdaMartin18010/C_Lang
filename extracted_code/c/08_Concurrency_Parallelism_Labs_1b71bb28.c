/*
 * Auto-generated from: 12_Practice_Exercises\08_Concurrency_Parallelism_Labs.md
 * Line: 239
 * Language: c
 * Block ID: 1b71bb28
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct EventLoop EventLoop;
typedef void (*IOCallback)(int fd, uint32_t events, void *userdata);
typedef void (*TimerCallback)(void *userdata);

EventLoop* evloop_create(void);
void evloop_destroy(EventLoop *loop);

// 注册I/O事件
void evloop_register(EventLoop *loop, int fd, uint32_t events,
                     IOCallback cb, void *userdata);
void evloop_unregister(EventLoop *loop, int fd);

// 定时器
void evloop_set_timeout(EventLoop *loop, uint64_t ms,
                        TimerCallback cb, void *userdata);

// 运行事件循环
void evloop_run(EventLoop *loop);
void evloop_stop(EventLoop *loop);
