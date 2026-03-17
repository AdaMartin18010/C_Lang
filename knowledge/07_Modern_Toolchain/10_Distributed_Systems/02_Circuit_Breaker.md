# 熔断器与限流模式


---

## 📑 目录

- [熔断器与限流模式](#熔断器与限流模式)
  - [📑 目录](#-目录)
  - [熔断器模式 (Circuit Breaker)](#熔断器模式-circuit-breaker)
    - [核心概念](#核心概念)
    - [C实现](#c实现)
  - [限流模式 (Rate Limiting)](#限流模式-rate-limiting)
    - [令牌桶算法](#令牌桶算法)
    - [漏桶算法](#漏桶算法)
    - [滑动窗口限流](#滑动窗口限流)
  - [舱壁模式 (Bulkhead)](#舱壁模式-bulkhead)
  - [参考](#参考)


---

## 熔断器模式 (Circuit Breaker)

### 核心概念

```
熔断器三种状态:
- CLOSED:    正常，请求通过
- OPEN:      熔断，请求直接失败
- HALF_OPEN: 试探，允许少量请求测试

触发条件:
- 失败率超过阈值
- 连续失败次数超过阈值
- 响应时间超过阈值

恢复策略:
- 超时后进入HALF_OPEN
- 成功率达到阈值后关闭
```

### C实现

```c
#include <stdatomic.h>
#include <time.h>

typedef enum {
    CB_CLOSED,
    CB_OPEN,
    CB_HALF_OPEN
} cb_state_t;

typedef struct {
    /* 配置 */
    int failure_threshold;      /* 触发熔断的失败次数 */
    int success_threshold;      /* HALF_OPEN恢复需要的成功次数 */
    int timeout_ms;             /* 超时时间 */
    int half_open_max_calls;    /* HALF_OPEN状态最大请求数 */

    /* 状态 */
    _Atomic(cb_state_t) state;
    _Atomic(int) failure_count;
    _Atomic(int) success_count;
    _Atomic(int) half_open_calls);
    _Atomic(time_t) last_failure_time;
    _Atomic(int64_t) last_test_time;

    mtx_t lock;
} circuit_breaker_t;

circuit_breaker_t* cb_create(int failure_threshold,
                              int success_threshold,
                              int timeout_ms) {
    circuit_breaker_t* cb = calloc(1, sizeof(circuit_breaker_t));
    cb->failure_threshold = failure_threshold;
    cb->success_threshold = success_threshold;
    cb->timeout_ms = timeout_ms;
    cb->half_open_max_calls = 3;

    atomic_init(&cb->state, CB_CLOSED);
    atomic_init(&cb->failure_count, 0);
    atomic_init(&cb->success_count, 0);
    mtx_init(&cb->lock, mtx_plain);

    return cb;
}

/* 检查是否允许请求 */
bool cb_can_execute(circuit_breaker_t* cb) {
    cb_state_t state = atomic_load(&cb->state);

    switch (state) {
        case CB_CLOSED:
            return true;

        case CB_OPEN: {
            /* 检查是否可以进入HALF_OPEN */
            int64_t now = current_time_ms();
            int64_t last = atomic_load(&cb->last_failure_time);

            if (now - last >= cb->timeout_ms) {
                /* 尝试进入HALF_OPEN */
                cb_state_t expected = CB_OPEN;
                if (atomic_compare_exchange_strong(&cb->state, &expected,
                                                    CB_HALF_OPEN)) {
                    atomic_store(&cb->half_open_calls, 0);
                    atomic_store(&cb->success_count, 0);
                    printf("Circuit Breaker: OPEN -> HALF_OPEN\n");
                }
                return true;  /* 即使CAS失败也允许 */
            }
            return false;  /* 仍然熔断 */
        }

        case CB_HALF_OPEN: {
            /* 限制并发请求数 */
            int calls = atomic_fetch_add(&cb->half_open_calls, 1);
            if (calls >= cb->half_open_max_calls) {
                atomic_fetch_sub(&cb->half_open_calls, 1);
                return false;
            }
            return true;
        }
    }
    return false;
}

/* 记录成功 */
void cb_record_success(circuit_breaker_t* cb) {
    cb_state_t state = atomic_load(&cb->state);

    if (state == CB_HALF_OPEN) {
        int successes = atomic_fetch_add(&cb->success_count, 1) + 1;
        atomic_fetch_sub(&cb->half_open_calls, 1);

        if (successes >= cb->success_threshold) {
            /* 恢复CLOSED */
            cb_state_t expected = CB_HALF_OPEN;
            if (atomic_compare_exchange_strong(&cb->state, &expected,
                                                CB_CLOSED)) {
                atomic_store(&cb->failure_count, 0);
                printf("Circuit Breaker: HALF_OPEN -> CLOSED\n");
            }
        }
    } else if (state == CB_CLOSED) {
        /* 定期重置失败计数 */
        atomic_store(&cb->failure_count, 0);
    }
}

/* 记录失败 */
void cb_record_failure(circuit_breaker_t* cb) {
    cb_state_t state = atomic_load(&cb->state);

    if (state == CB_HALF_OPEN) {
        atomic_fetch_sub(&cb->half_open_calls, 1);
        /* 立即熔断 */
        cb_state_t expected = CB_HALF_OPEN;
        if (atomic_compare_exchange_strong(&cb->state, &expected, CB_OPEN)) {
            atomic_store(&cb->last_failure_time, current_time_ms());
            printf("Circuit Breaker: HALF_OPEN -> OPEN\n");
        }
    } else if (state == CB_CLOSED) {
        int failures = atomic_fetch_add(&cb->failure_count, 1) + 1;

        if (failures >= cb->failure_threshold) {
            /* 触发熔断 */
            cb_state_t expected = CB_CLOSED;
            if (atomic_compare_exchange_strong(&cb->state, &expected, CB_OPEN)) {
                atomic_store(&cb->last_failure_time, current_time_ms());
                printf("Circuit Breaker: CLOSED -> OPEN\n");
            }
        }
    }
}

/* 带熔断保护的函数执行 */
typedef int (*protected_func_t)(void* arg);

int cb_execute(circuit_breaker_t* cb, protected_func_t func, void* arg,
               int (*fallback)(void* arg)) {
    if (!cb_can_execute(cb)) {
        /* 熔断，执行降级 */
        if (fallback) {
            return fallback(arg);
        }
        return -EAGAIN;  /* 服务不可用 */
    }

    int result = func(arg);

    if (result == 0) {
        cb_record_success(cb);
    } else {
        cb_record_failure(cb);
    }

    return result;
}

/* 使用示例 */
int call_remote_service(void* arg) {
    /* 实际远程调用 */
    return http_get("http://api.example.com/data");
}

int fallback_function(void* arg) {
    /* 返回缓存数据 */
    printf("Fallback: returning cached data\n");
    return 0;
}

void example(void) {
    circuit_breaker_t* cb = cb_create(
        5,      /* 5次失败触发熔断 */
        3,      /* 3次成功恢复 */
        10000   /* 10秒超时 */
    );

    for (int i = 0; i < 100; i++) {
        int rc = cb_execute(cb, call_remote_service, NULL, fallback_function);
        if (rc != 0) {
            printf("Request failed: %d\n", rc);
        }
    }
}
```

---

## 限流模式 (Rate Limiting)

### 令牌桶算法

```c
typedef struct {
    double capacity;        /* 桶容量 */
    double tokens;          /* 当前令牌数 */
    double fill_rate;       /* 每秒填充速率 */
    int64_t last_update;    /* 最后更新时间 */
    mtx_t lock;
} token_bucket_t;

token_bucket_t* tb_create(double capacity, double fill_rate) {
    token_bucket_t* tb = malloc(sizeof(token_bucket_t));
    tb->capacity = capacity;
    tb->tokens = capacity;  /* 初始满 */
    tb->fill_rate = fill_rate;
    tb->last_update = current_time_ms();
    mtx_init(&tb->lock, mtx_plain);
    return tb;
}

/* 尝试获取令牌 */
bool tb_consume(token_bucket_t* tb, double tokens) {
    mtx_lock(&tb->lock);

    /* 计算新令牌 */
    int64_t now = current_time_ms();
    double elapsed = (now - tb->last_update) / 1000.0;
    tb->tokens = fmin(tb->capacity, tb->tokens + elapsed * tb->fill_rate);
    tb->last_update = now;

    /* 尝试消费 */
    if (tb->tokens >= tokens) {
        tb->tokens -= tokens;
        mtx_unlock(&tb->lock);
        return true;
    }

    mtx_unlock(&tb->lock);
    return false;
}

/* 等待获取令牌 */
bool tb_consume_wait(token_bucket_t* tb, double tokens, int timeout_ms) {
    int waited = 0;
    while (!tb_consume(tb, tokens)) {
        if (waited >= timeout_ms) return false;

        /* 计算等待时间 */
        mtx_lock(&tb->lock);
        double needed = tokens - tb->tokens;
        int wait = (int)(needed / tb->fill_rate * 1000);
        mtx_unlock(&tb->lock);

        wait = fmin(wait, 100);  /* 最多等100ms */
        usleep(wait * 1000);
        waited += wait;
    }
    return true;
}
```

### 漏桶算法

```c
typedef struct {
    double capacity;        /* 桶容量 */
    double water;           /* 当前水量 */
    double leak_rate;       /* 漏出速率 */
    int64_t last_update;
    mtx_t lock;
} leaky_bucket_t;

leaky_bucket_t* lb_create(double capacity, double leak_rate) {
    leaky_bucket_t* lb = malloc(sizeof(leaky_bucket_t));
    lb->capacity = capacity;
    lb->water = 0;
    lb->leak_rate = leak_rate;
    lb->last_update = current_time_ms();
    mtx_init(&lb->lock, mtx_plain);
    return lb;
}

/* 添加水 (请求) */
bool lb_add(leaky_bucket_t* lb, double water) {
    mtx_lock(&lb->lock);

    /* 先漏水 */
    int64_t now = current_time_ms();
    double elapsed = (now - lb->last_update) / 1000.0;
    lb->water = fmax(0, lb->water - elapsed * lb->leak_rate);
    lb->last_update = now;

    /* 检查是否溢出 */
    if (lb->water + water <= lb->capacity) {
        lb->water += water;
        mtx_unlock(&lb->lock);
        return true;
    }

    mtx_unlock(&lb->lock);
    return false;  /* 桶满，拒绝 */
}
```

### 滑动窗口限流

```c
/* 精确滑动窗口 */

typedef struct {
    int64_t timestamp;
    int count;
} window_entry_t;

typedef struct {
    int window_size_ms;     /* 窗口大小 */
    int max_requests;       /* 窗口内最大请求数 */

    window_entry_t* entries;
    int entry_count;
    int head;
    mtx_t lock;
} sliding_window_t;

sliding_window_t* sw_create(int window_ms, int max_requests) {
    sliding_window_t* sw = malloc(sizeof(sliding_window_t));
    sw->window_size_ms = window_ms;
    sw->max_requests = max_requests;
    sw->entries = calloc(max_requests * 2, sizeof(window_entry_t));
    sw->entry_count = 0;
    sw->head = 0;
    mtx_init(&sw->lock, mtx_plain);
    return sw;
}

/* 检查并记录 */
bool sw_allow(sliding_window_t* sw) {
    mtx_lock(&sw->lock);

    int64_t now = current_time_ms();
    int64_t window_start = now - sw->window_size_ms;

    /* 清理过期条目 */
    while (sw->entry_count > 0) {
        int idx = (sw->head - sw->entry_count + max_requests * 2)
                  % (max_requests * 2);
        if (sw->entries[idx].timestamp >= window_start) {
            break;
        }
        sw->entry_count--;
    }

    /* 检查是否超限 */
    if (sw->entry_count >= sw->max_requests) {
        mtx_unlock(&sw->lock);
        return false;
    }

    /* 记录 */
    sw->entries[sw->head] = (window_entry_t){now, 1};
    sw->head = (sw->head + 1) % (sw->max_requests * 2);
    sw->entry_count++;

    mtx_unlock(&sw->lock);
    return true;
}
```

---

## 舱壁模式 (Bulkhead)

```c
/* 舱壁隔离: 限制每个服务的资源使用 */

typedef struct {
    char service_name[64];
    int max_concurrent;     /* 最大并发数 */
    int max_queue_size;     /* 队列大小 */

    atomic_int concurrent;  /* 当前并发 */
    atomic_int queued;      /* 队列中等待 */
} bulkhead_t;

bulkhead_t* bulkhead_create(const char* name, int max_concurrent,
                            int max_queue) {
    bulkhead_t* bh = calloc(1, sizeof(bulkhead_t));
    strncpy(bh->service_name, name, sizeof(bh->service_name));
    bh->max_concurrent = max_concurrent;
    bh->max_queue_size = max_queue;
    atomic_init(&bh->concurrent, 0);
    atomic_init(&bh->queued, 0);
    return bh;
}

/* 尝试进入 */
typedef enum {
    BULKHEAD_ALLOW,     /* 允许立即执行 */
    BULKHEAD_QUEUE,     /* 需要排队 */
    BULKHEAD_REJECT     /* 拒绝 */
} bulkhead_result_t;

bulkhead_result_t bulkhead_enter(bulkhead_t* bh) {
    int current = atomic_load(&bh->concurrent);

    if (current < bh->max_concurrent) {
        /* 有空闲槽 */
        if (atomic_compare_exchange_strong(&bh->concurrent, &current,
                                           current + 1)) {
            return BULKHEAD_ALLOW;
        }
    }

    /* 检查队列 */
    int queued = atomic_fetch_add(&bh->queued, 1);
    if (queued < bh->max_queue_size) {
        return BULKHEAD_QUEUE;
    } else {
        atomic_fetch_sub(&bh->queued, 1);
        return BULKHEAD_REJECT;
    }
}

void bulkhead_exit(bulkhead_t* bh) {
    atomic_fetch_sub(&bh->concurrent, 1);
}

void bulkhead_dequeue(bulkhead_t* bh) {
    atomic_fetch_sub(&bh->queued, 1);
}
```

---

## 参考

- [Circuit Breaker Pattern](https://martinfowler.com/bliki/CircuitBreaker.html)
- [Rate Limiting](https://konghq.com/blog/how-to-design-a-scalable-rate-limiting-algorithm)
- [Bulkhead Pattern](https://docs.microsoft.com/en-us/azure/architecture/patterns/bulkhead)
