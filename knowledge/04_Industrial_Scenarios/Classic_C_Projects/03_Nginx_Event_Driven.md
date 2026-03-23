---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
# Nginx 事件驱动架构源码解读

> **源码版本**: Nginx 1.24+
> **核心文件**: src/event/ngx_event.c, src/core/ngx_connection.c
> **学习重点**: 异步非阻塞I/O、事件循环、连接池管理

---

## 1. 整体架构

### 1.1 进程模型

```
Master Process (管理)
    ├── Worker Process 1 (事件循环)
    ├── Worker Process 2 (事件循环)
    └── Worker Process N (事件循环)
        ├── epoll_wait() (Linux)
        ├── kqueue()     (BSD/macOS)
        ├── event_port() (Solaris)
        └── select/poll  (Fallback)
```

### 1.2 核心模块

```c
// 事件模块接口
ngx_module_t ngx_epoll_module;
ngx_module_t ngx_kqueue_module;
ngx_module_t ngx_eventport_module;
ngx_module_t ngx_select_module;
ngx_module_t ngx_poll_module;

// 核心结构
typedef struct {
    ngx_int_t  (*add)(ngx_event_t *ev, ngx_int_t event, ngx_uint_t flags);
    ngx_int_t  (*del)(ngx_event_t *ev, ngx_int_t event, ngx_uint_t flags);
    ngx_int_t  (*enable)(ngx_event_t *ev, ngx_int_t event, ngx_uint_t flags);
    ngx_int_t  (*disable)(ngx_event_t *ev, ngx_int_t event, ngx_uint_t flags);

    ngx_int_t  (*add_conn)(ngx_connection_t *c);
    ngx_int_t  (*del_conn)(ngx_connection_t *c, ngx_uint_t flags);

    ngx_int_t  (*notify)(ngx_event_handler_pt handler);
    ngx_int_t  (*process_events)(ngx_cycle_t *cycle, ngx_msec_t timer,
                                  ngx_uint_t flags);

    ngx_int_t  (*init)(ngx_cycle_t *cycle, ngx_msec_t timer);
    void       (*done)(ngx_cycle_t *cycle);
} ngx_event_actions_t;
```

---

## 2. 事件循环

### 2.1 核心循环

```c
// src/event/ngx_event.c

void ngx_process_events_and_timers(ngx_cycle_t *cycle) {
    ngx_uint_t  flags;
    ngx_msec_t  timer, delta;

    // 获取最短定时器
    timer = ngx_event_find_timer();

    // 优化：如果有过多事件需要处理，不等待定时器
    if (ngx_accept_disabled > 0) {
        // 暂时不接受新连接
    }

    // 处理过期定时器
    ngx_event_expire_timers();

    // 事件收集标志
    flags = NGX_UPDATE_TIME | NGX_UPDATE_EPOLL|RDHUP;

    // 调用具体事件模块 (epoll_wait/kqueue等)
    (void) ngx_process_events(cycle, timer, flags);

    // 更新时间戳
    ngx_time_update();

    // 处理accept事件 (负载均衡)
    ngx_event_process_posted(cycle, &ngx_posted_accept_events);

    // 处理读写事件
    ngx_event_process_posted(cycle, &ngx_posted_events);
}
```

### 2.2 epoll实现

```c
// src/event/modules/ngx_epoll_module.c

typedef struct {
    ngx_uint_t  events;
    ngx_uint_t  aio_requests;
} ngx_epoll_conf_t;

static ngx_int_t ngx_epoll_init(ngx_cycle_t *cycle, ngx_msec_t timer) {
    ngx_epoll_conf_t  *epcf;

    // 创建epoll实例
    ep = epoll_create(cycle->connection_n / 2);

    // 创建用于唤醒的eventfd
    ngx_epoll_eventfd = eventfd(0, 0);

    // 添加到epoll
    ee.events = EPOLLIN|EPOLLET;
    ee.data.ptr = &ngx_eventfd_conn;
    epoll_ctl(ep, EPOLL_CTL_ADD, ngx_epoll_eventfd, &ee);

    return NGX_OK;
}

static ngx_int_t ngx_epoll_add_event(ngx_event_t *ev, ngx_int_t event,
                                      ngx_uint_t flags) {
    int                  op;
    uint32_t             events;
    ngx_epoll_event_t    ee;
    ngx_connection_t    *c;

    c = ev->data;

    // 判断是添加还是修改
    if (event == NGX_READ_EVENT) {
        op = EPOLL_CTL_ADD;
        events = EPOLLIN|EPOLLRDHUP;
    } else {
        op = EPOLL_CTL_MOD;
        events = EPOLLIN|EPOLLOUT|EPOLLRDHUP;
    }

    // 边缘触发模式
    events |= EPOLLET;

    ee.events = events;
    ee.data.ptr = (void *) ((uintptr_t) c | ev->instance);

    return epoll_ctl(ep, op, c->fd, &ee);
}

static ngx_int_t ngx_epoll_process_events(ngx_cycle_t *cycle,
                                          ngx_msec_t timer,
                                          ngx_uint_t flags) {
    int                events;
    uint32_t           revents;
    ngx_int_t          instance, i;
    ngx_uint_t         level;
    ngx_err_t          err;
    ngx_event_t       *rev, *wev;
    ngx_queue_t       *queue;
    ngx_connection_t  *c;

    // 等待事件
    events = epoll_wait(ep, event_list, (int) nevents, timer);

    err = (events == -1) ? ngx_errno : 0;

    if (flags & NGX_UPDATE_TIME || ngx_event_timer_alarm) {
        ngx_time_update();
    }

    // 处理每个事件
    for (i = 0; i < events; i++) {
        c = event_list[i].data.ptr;

        // 检查instance避免过期事件
        instance = (uintptr_t) c & 1;
        c = (ngx_connection_t *) ((uintptr_t) c & (uintptr_t) ~1);

        rev = c->read;

        if (c->fd == -1 || rev->instance != instance) {
            // 连接已关闭或过期
            continue;
        }

        revents = event_list[i].events;

        // 读取事件
        if ((revents & EPOLLIN) && rev->active) {
            rev->ready = 1;
            rev->available = 1;

            if (flags & NGX_POST_EVENTS) {
                // 延后处理
                queue = rev->accept ? &ngx_posted_accept_events
                                    : &ngx_posted_events;
                ngx_post_event(rev, queue);
            } else {
                // 立即处理
                rev->handler(rev);
            }
        }

        // 写入事件
        wev = c->write;
        if ((revents & EPOLLOUT) && wev->active) {
            wev->ready = 1;

            if (flags & NGX_POST_EVENTS) {
                ngx_post_event(wev, &ngx_posted_events);
            } else {
                wev->handler(wev);
            }
        }
    }

    return NGX_OK;
}
```

---

## 3. 连接管理

### 3.1 连接池

```c
// src/core/ngx_connection.c

// 预分配连接池
typedef struct {
    ngx_connection_t   *connections;    // 连接数组
    ngx_event_t        *read_events;    // 读事件数组
    ngx_event_t        *write_events;   // 写事件数组

    ngx_uint_t          connection_n;   // 连接数
} ngx_cycle_t;

ngx_int_t ngx_create_listening_sockets(ngx_cycle_t *cycle) {
    // 预分配所有连接
    cycle->connections = ngx_alloc(sizeof(ngx_connection_t) * cycle->connection_n,
                                   cycle->log);
    cycle->read_events = ngx_alloc(sizeof(ngx_event_t) * cycle->connection_n,
                                   cycle->log);
    cycle->write_events = ngx_alloc(sizeof(ngx_event_t) * cycle->connection_n,
                                    cycle->log);

    // 初始化空闲连接链表
    ngx_connection_t *c = cycle->connections;
    ngx_event_t *rev = cycle->read_events;
    ngx_event_t *wev = cycle->write_events;

    for (i = 0; i < cycle->connection_n; i++) {
        c[i].data = NULL;
        c[i].read = &rev[i];
        c[i].write = &wev[i];
        c[i].fd = (ngx_socket_t) -1;

        // 链入空闲链表
        c[i].next = (i < cycle->connection_n - 1) ? &c[i + 1] : NULL;
    }

    cycle->free_connections = c;
    cycle->free_connection_n = cycle->connection_n;

    return NGX_OK;
}

// 获取空闲连接
ngx_connection_t* ngx_get_connection(ngx_socket_t s, ngx_log_t *log) {
    ngx_connection_t  *c;

    // 从空闲链表获取
    c = ngx_cycle->free_connections;

    if (c == NULL) {
        // 没有空闲连接
        ngx_drain_connections(ngx_cycle);
        c = ngx_cycle->free_connections;
    }

    if (c == NULL) {
        return NULL;
    }

    ngx_cycle->free_connections = c->next;
    ngx_cycle->free_connection_n--;

    // 初始化连接
    c->fd = s;
    c->log = log;

    // 初始化读写事件
    rev = c->read;
    wev = c->write;

    ngx_memzero(rev, sizeof(ngx_event_t));
    ngx_memzero(wev, sizeof(ngx_event_t));

    rev->instance = !rev->instance;  // 翻转instance标记
    wev->instance = !wev->instance;

    rev->data = c;
    wev->data = c;

    return c;
}

// 释放连接
void ngx_free_connection(ngx_connection_t *c) {
    // 从epoll移除
    ngx_del_conn(c, NGX_CLOSE_EVENT);

    c->fd = (ngx_socket_t) -1;

    // 归还到空闲链表
    c->next = ngx_cycle->free_connections;
    ngx_cycle->free_connections = c;
    ngx_cycle->free_connection_n++;
}
```

### 3.2 异步I/O

```c
// 非阻塞读取
ssize_t ngx_recv(ngx_connection_t *c, u_char *buf, size_t size) {
    ssize_t n;
    ngx_err_t err;
    ngx_event_t *rev;

    rev = c->read;

    do {
        n = recv(c->fd, buf, size, 0);
        err = ngx_socket_errno;
    } while (n == -1 && err == NGX_EINTR);

    if (n == -1) {
        if (err == NGX_EAGAIN || err == NGX_EWOULDBLOCK) {
            // 数据未就绪，等待事件
            rev->ready = 0;
            return NGX_AGAIN;
        }

        return NGX_ERROR;
    }

    if (n == 0) {
        rev->ready = 0;
        rev->eof = 1;
        return 0;
    }

    return n;
}

// 非阻塞发送
ssize_t ngx_send(ngx_connection_t *c, u_char *buf, size_t size) {
    ssize_t n;
    ngx_err_t err;
    ngx_event_t *wev;

    wev = c->write;

    do {
        n = send(c->fd, buf, size, 0);
        err = ngx_socket_errno;
    } while (n == -1 && err == NGX_EINTR);

    if (n == -1) {
        if (err == NGX_EAGAIN || err == NGX_EWOULDBLOCK) {
            // 发送缓冲区满，等待可写事件
            wev->ready = 0;
            return NGX_AGAIN;
        }

        return NGX_ERROR;
    }

    return n;
}
```

---

## 4. 定时器

### 4.1 红黑树定时器

```c
// src/event/ngx_event_timer.c

// Nginx使用红黑树管理定时器
static ngx_rbtree_t  ngx_event_timer_rbtree;
static ngx_rbtree_node_t  ngx_event_timer_sentinel;

// 添加定时器
void ngx_event_add_timer(ngx_event_t *ev, ngx_msec_t timer) {
    ngx_msec_t      key;
    ngx_msec_int_t  diff;

    key = ngx_current_msec + timer;

    if (ev->timer_set) {
        // 已存在，先删除
        ngx_event_del_timer(ev);
    }

    ev->timer.key = key;
    ngx_rbtree_insert(&ngx_event_timer_rbtree, &ev->timer);
    ev->timer_set = 1;
}

// 删除定时器
void ngx_event_del_timer(ngx_event_t *ev) {
    ngx_rbtree_delete(&ngx_event_timer_rbtree, &ev->timer);
    ev->timer_set = 0;
}

// 查找最短超时 (用于epoll_wait的timeout参数)
ngx_msec_t ngx_event_find_timer(void) {
    ngx_msec_int_t  timer;
    ngx_rbtree_node_t  *node, *root, *sentinel;

    if (ngx_event_timer_rbtree.root == &ngx_event_timer_sentinel) {
        return NGX_TIMER_INFINITE;
    }

    // 最小key在树的最左节点
    node = ngx_rbtree_min(ngx_event_timer_rbtree.root,
                          &ngx_event_timer_sentinel);

    timer = (ngx_msec_int_t) (node->key - ngx_current_msec);

    return (ngx_msec_t) (timer > 0 ? timer : 0);
}

// 处理过期定时器
void ngx_event_expire_timers(void) {
    ngx_event_t        *ev;
    ngx_rbtree_node_t  *node, *root, *sentinel;

    sentinel = ngx_event_timer_rbtree.sentinel;

    while (1) {
        root = ngx_event_timer_rbtree.root;

        if (root == sentinel) {
            return;
        }

        node = ngx_rbtree_min(root, sentinel);

        if ((ngx_msec_int_t) (node->key - ngx_current_msec) > 0) {
            return;  // 最早的定时器也未过期
        }

        ev = (ngx_event_t *) ((char *) node - offsetof(ngx_event_t, timer));

        ngx_rbtree_delete(&ngx_event_timer_rbtree, &ev->timer);
        ev->timer_set = 0;

        ev->timedout = 1;
        ev->handler(ev);  // 调用超时处理函数
    }
}
```

---

## 5. 负载均衡

### 5.1 惊群避免

```c
// 多worker时避免所有进程同时唤醒

// accept_mutex锁
ngx_shmtx_t  ngx_accept_mutex;

void ngx_trylock_accept_mutex(ngx_cycle_t *cycle) {
    if (ngx_shmtx_trylock(&ngx_accept_mutex)) {
        // 获取锁成功
        if (ngx_accept_mutex_held && ngx_accept_events == 0) {
            return;
        }

        // 将监听socket加入epoll
        if (ngx_enable_accept_events(cycle) == NGX_ERROR) {
            ngx_shmtx_unlock(&ngx_accept_mutex);
            return;
        }

        ngx_accept_events = 0;
        ngx_accept_mutex_held = 1;

    } else {
        // 获取锁失败
        if (ngx_accept_mutex_held) {
            // 移除监听socket
            if (ngx_disable_accept_events(cycle, 0) == NGX_ERROR) {
                return;
            }

            ngx_accept_mutex_held = 0;
        }
    }
}

// 更简单的方法: SO_REUSEPORT (Linux 3.9+)
// 每个worker绑定相同端口，内核负责负载均衡
```

### 5.2 连接数限制

```c
// 基于连接数的负载均衡
ngx_int_t ngx_accept_disabled = 0;

void ngx_process_events_and_timers(ngx_cycle_t *cycle) {
    // 如果当前worker连接过多，暂时不接受新连接
    if (ngx_accept_disabled > 0) {
        ngx_accept_disabled--;

    } else {
        // 尝试获取accept锁
        ngx_trylock_accept_mutex(cycle);
    }

    // 计算accept_disabled
    // 当总连接数的1/8减去剩余空闲连接数 > 0时，开始限制
    ngx_accept_disabled = ngx_cycle->connection_n / 8
                        - ngx_cycle->free_connection_n;

    // ...
}
```

---

## 6. 学习要点

### 6.1 高性能服务器设计

```
关键原则:
1. 非阻塞I/O - 所有操作立即返回
2. 事件驱动  - 单线程处理多连接
3. 零拷贝    - sendfile, splice
4. 内存池    - 减少malloc/free
5. 连接复用  - 预分配连接池
```

### 6.2 代码特点

| 特点 | 说明 |
|:-----|:-----|
| 模块化 | 清晰的模块接口 |
| 跨平台 | 统一抽象，多平台支持 |
| 零拷贝 | sendfile, mmap |
| 配置驱动 | 配置文件决定行为 |

---

**最后更新**: 2026-03-24
