# 安全编码模式与反模式

> **层级定位**: 01 Core Knowledge System / 09 Safety Standards
> **难度级别**: L3-L4
> **预估学习时间**: 6-8 小时

---

## 目录

- [安全编码模式与反模式](#安全编码模式与反模式)
  - [目录](#目录)
  - [1. 输入验证模式](#1-输入验证模式)
    - [1.1 白名单验证](#11-白名单验证)
    - [1.2 整数范围检查](#12-整数范围检查)
  - [2. 内存安全模式](#2-内存安全模式)
    - [2.1 安全字符串操作](#21-安全字符串操作)
    - [2.2 安全内存分配模式](#22-安全内存分配模式)
    - [2.3 防止Use-After-Free](#23-防止use-after-free)
  - [3. 错误处理模式](#3-错误处理模式)
    - [3.1 错误传播](#31-错误传播)
  - [4. 加密安全模式](#4-加密安全模式)
    - [4.1 敏感数据处理](#41-敏感数据处理)
  - [5. 并发安全模式](#5-并发安全模式)
    - [5.1 线程安全单例](#51-线程安全单例)
    - [5.2 无锁读多写少](#52-无锁读多写少)
  - [6. 防御性编程检查清单](#6-防御性编程检查清单)
    - [6.1 函数入口检查](#61-函数入口检查)
    - [6.2 断言使用](#62-断言使用)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 输入验证模式

### 1.1 白名单验证

```c
// 好的做法：白名单
typedef enum { CMD_READ, CMD_WRITE, CMD_DELETE, CMD_MAX } command_t;

int execute_command(int cmd_id, const char *arg) {
    // 白名单验证
    if (cmd_id < 0 || cmd_id >= CMD_MAX) {
        return -EINVAL;
    }

    static const struct {
        int (*handler)(const char *);
        uint32_t min_arg_len;
        uint32_t max_arg_len;
    } cmd_table[CMD_MAX] = {
        [CMD_READ]  = { do_read,  1, 256 },
        [CMD_WRITE] = { do_write, 1, 4096 },
        [CMD_DELETE]= { do_delete, 1, 256 },
    };

    size_t arg_len = strlen(arg);
    if (arg_len < cmd_table[cmd_id].min_arg_len ||
        arg_len > cmd_table[cmd_id].max_arg_len) {
        return -EINVAL;
    }

    return cmd_table[cmd_id].handler(arg);
}

// 不好的做法：黑名单
int execute_command_bad(int cmd_id, const char *arg) {
    // 黑名单容易遗漏
    if (cmd_id == 99) {  // 禁止的系统命令
        return -EPERM;
    }
    // 其他命令都允许？危险！
    return execute(cmd_id, arg);
}
```

### 1.2 整数范围检查

```c
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>

// 安全的整数解析
bool safe_strtol(const char *str, long min, long max, long *out) {
    char *endptr;
    long val;

    errno = 0;
    val = strtol(str, &endptr, 10);

    // 检查转换错误
    if (endptr == str || *endptr != '\0') return false;
    if (errno == ERANGE) return false;

    // 范围检查（防止溢出绕过）
    if (val < min || val > max) return false;

    *out = val;
    return true;
}

// 有符号/无符号转换检查
bool safe_size_t_to_int(size_t val, int *out) {
    if (val > INT_MAX) return false;
    *out = (int)val;
    return true;
}

// 乘法溢出检查
bool safe_mul(size_t a, size_t b, size_t *out) {
    if (a != 0 && b > SIZE_MAX / a) return false;
    *out = a * b;
    return true;
}

// 加法溢出检查
bool safe_add(size_t a, size_t b, size_t *out) {
    if (a > SIZE_MAX - b) return false;
    *out = a + b;
    return true;
}
```

---

## 2. 内存安全模式

### 2.1 安全字符串操作

```c
#include <string.h>

// 模式1：始终检查目标大小
int safe_strcpy(char *dst, size_t dst_size, const char *src) {
    if (!dst || !src || dst_size == 0) return -1;

    size_t src_len = strlen(src);
    if (src_len >= dst_size) {
        // 截断或错误
        return -1;
    }

    memcpy(dst, src, src_len + 1);  // 包含\0
    return 0;
}

// 模式2：使用安全函数（C23标准）
#if __STDC_VERSION__ >= 202311L
    // 使用strlcpy（C23）
    strlcpy(dst, src, dst_size);
#else
    // 手动实现
    size_t i;
    for (i = 0; i < dst_size - 1 && src[i]; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
#endif

// 模式3：动态分配确保空间
char *strdup_safe(const char *src) {
    if (!src) return NULL;

    size_t len = strlen(src);
    char *dst = malloc(len + 1);
    if (dst) {
        memcpy(dst, src, len + 1);
    }
    return dst;
}
```

### 2.2 安全内存分配模式

```c
// 模式：清零分配
void *safe_calloc(size_t nmemb, size_t size) {
    if (nmemb && size > SIZE_MAX / nmemb) {
        errno = ENOMEM;
        return NULL;
    }
    return calloc(nmemb, size);  // calloc检查溢出并清零
}

// 模式：灵活的数组成员
struct packet {
    size_t len;
    char data[];  // 柔性数组
};

struct packet *packet_alloc(size_t data_len) {
    // 检查总大小溢出
    if (data_len > SIZE_MAX - sizeof(struct packet)) {
        return NULL;
    }

    struct packet *p = malloc(sizeof(struct packet) + data_len);
    if (p) {
        p->len = data_len;
        // data无需初始化，由调用者填充
    }
    return p;
}

// 模式：带大小的结构
struct sized_buffer {
    size_t size;
    size_t used;
    char buf[];
};

// 安全访问
char *buf_get(struct sized_buffer *sb, size_t idx) {
    if (!sb || idx >= sb->used) return NULL;
    return &sb->buf[idx];
}
```

### 2.3 防止Use-After-Free

```c
typedef struct handle {
    void *ptr;
    uint32_t generation;  // 代际计数
} handle_t;

static uint32_t global_generation = 0;

void *handle_get(handle_t *h) {
    if (!h || h->generation != global_generation) {
        return NULL;
    }
    return h->ptr;
}

void resource_free(void *ptr) {
    free(ptr);
    global_generation++;  // 使所有handle失效
}

// 另一种方式：指针置空
#define SAFE_FREE(p) do { \
    free(p); \
    (p) = NULL; \
} while(0)

// 复杂对象的RAII模式
typedef struct resource {
    void *data;
    size_t size;
    bool valid;
} resource_t;

void resource_release(resource_t *r) {
    if (r && r->valid) {
        free(r->data);
        r->data = NULL;
        r->size = 0;
        r->valid = false;
    }
}

// 使用__attribute__((cleanup))
static inline void auto_release(resource_t **r) {
    resource_release(*r);
}

#define AUTO_RESOURCE __attribute__((cleanup(auto_release)))

void example(void) {
    AUTO_RESOURCE resource_t *res = resource_alloc(1024);
    // 使用res...
    // 函数退出时自动释放
}
```

---

## 3. 错误处理模式

### 3.1 错误传播

```c
// 模式：错误码链
typedef enum {
    ERR_OK = 0,
    ERR_NOMEM = -1,
    ERR_IO = -2,
    ERR_INVALID = -3,
} error_t;

// 使用goto进行统一清理
int complex_operation(void) {
    error_t err = ERR_OK;
    void *buf1 = NULL;
    void *buf2 = NULL;
    void *buf3 = NULL;

    buf1 = malloc(100);
    if (!buf1) {
        err = ERR_NOMEM;
        goto cleanup;
    }

    buf2 = malloc(200);
    if (!buf2) {
        err = ERR_NOMEM;
        goto cleanup;
    }

    if (do_something(buf1, buf2) < 0) {
        err = ERR_IO;
        goto cleanup;
    }

    buf3 = malloc(300);
    if (!buf3) {
        err = ERR_NOMEM;
        goto cleanup;
    }

cleanup:
    free(buf1);
    free(buf2);
    free(buf3);
    return err;
}

// 模式：结果类型
typedef struct {
    bool success;
    union {
        void *value;
        const char *error_msg;
    };
} result_t;

result_t try_alloc(size_t size) {
    result_t r;
    void *p = malloc(size);
    if (p) {
        r.success = true;
        r.value = p;
    } else {
        r.success = false;
        r.error_msg = "Allocation failed";
    }
    return r;
}
```

---

## 4. 加密安全模式

### 4.1 敏感数据处理

```c
#include <string.h>

// 安全内存清零（防止编译器优化）
static inline void secure_zero(void *ptr, size_t len) {
    volatile unsigned char *p = ptr;
    while (len--) *p++ = 0;
}

// 或者使用C11函数（C23可用）
#if __STDC_VERSION__ >= 202311L
    #include <string.h>
    #define SECURE_ZERO(p, n) memset_explicit(p, 0, n)
#else
    #define SECURE_ZERO(p, n) secure_zero(p, n)
#endif

// 密码结构体
struct password {
    char data[128];
    size_t len;
};

void password_init(struct password *pw) {
    memset(pw, 0, sizeof(*pw));
}

void password_set(struct password *pw, const char *pass) {
    SECURE_ZERO(pw->data, sizeof(pw->data));
    size_t len = strlen(pass);
    if (len >= sizeof(pw->data)) len = sizeof(pw->data) - 1;
    memcpy(pw->data, pass, len);
    pw->len = len;
}

void password_clear(struct password *pw) {
    SECURE_ZERO(pw->data, sizeof(pw->data));
    pw->len = 0;
}

// 使用mlock防止交换
#include <sys/mman.h>

int secure_memory_init(void) {
    // 锁定敏感页面
    unsigned char sensitive_data[4096];
    if (mlock(sensitive_data, sizeof(sensitive_data)) < 0) {
        return -1;
    }
    // ... 使用 ...
    munlock(sensitive_data, sizeof(sensitive_data));
    SECURE_ZERO(sensitive_data, sizeof(sensitive_data));
    return 0;
}
```

---

## 5. 并发安全模式

### 5.1 线程安全单例

```c
#include <pthread.h>
#include <stdatomic.h>

// 模式：双重检查锁定（C11原子）
typedef struct config {
    int value;
} config_t;

static _Atomic(config_t *) g_config = NULL;
static pthread_mutex_t g_config_mutex = PTHREAD_MUTEX_INITIALIZER;

config_t *get_config(void) {
    config_t *cfg = atomic_load_explicit(&g_config, memory_order_acquire);
    if (cfg == NULL) {
        pthread_mutex_lock(&g_config_mutex);
        cfg = atomic_load_explicit(&g_config, memory_order_relaxed);
        if (cfg == NULL) {
            cfg = calloc(1, sizeof(config_t));
            if (cfg) {
                cfg->value = 42;
                atomic_store_explicit(&g_config, cfg, memory_order_release);
            }
        }
        pthread_mutex_unlock(&g_config_mutex);
    }
    return cfg;
}

// 更简单的C11方式：call_once
static config_t *g_config_once = NULL;
static pthread_once_t g_config_once_flag = PTHREAD_ONCE_INIT;

static void init_config(void) {
    g_config_once = calloc(1, sizeof(config_t));
    g_config_once->value = 42;
}

config_t *get_config_once(void) {
    pthread_once(&g_config_once_flag, init_config);
    return g_config_once;
}
```

### 5.2 无锁读多写少

```c
// RCU模式简化版
#include <stdatomic.h>

typedef struct data {
    atomic_int ref_count;
    int value;
} data_t;

data_t *data_new(int val) {
    data_t *d = malloc(sizeof(data_t));
    d->ref_count = ATOMIC_VAR_INIT(1);
    d->value = val;
    return d;
}

void data_get(data_t *d) {
    atomic_fetch_add(&d->ref_count, 1);
}

void data_put(data_t *d) {
    if (atomic_fetch_sub(&d->ref_count, 1) == 1) {
        free(d);
    }
}

// 全局数据指针
_Atomic(data_t *) g_data = NULL;

// 读取（无锁）
int read_data(void) {
    data_t *d = atomic_load_explicit(&g_data, memory_order_acquire);
    if (d) {
        data_get(d);
        int val = d->value;
        data_put(d);
        return val;
    }
    return -1;
}

// 更新（需要锁或其他同步）
void update_data(int new_val) {
    data_t *new_d = data_new(new_val);
    data_t *old_d = atomic_exchange(&g_data, new_d);
    if (old_d) data_put(old_d);  // 延迟释放
}
```

---

## 6. 防御性编程检查清单

### 6.1 函数入口检查

```c
int process_data(const void *input, size_t len, void *output, size_t out_len) {
    // 1. 空指针检查
    if (!input || !output) return -EINVAL;

    // 2. 大小检查
    if (len == 0 || len > MAX_INPUT_SIZE) return -EINVAL;
    if (out_len < MIN_OUTPUT_SIZE) return -EINVAL;

    // 3. 重叠检查（如果适用）
    if (ranges_overlap(input, len, output, out_len)) return -EINVAL;

    // 4. 对齐检查
    if (!IS_ALIGNED(input, ALIGNMENT)) return -EINVAL;

    // 处理...
    return 0;
}
```

### 6.2 断言使用

```c
#include <assert.h>

// 开发时检查（发布时NDEBUG禁用）
void internal_function(int *arr, size_t n) {
    assert(arr != NULL);        // 内部不变式
    assert(n > 0 && n < 10000); // 合理范围

    for (size_t i = 0; i < n; i++) {
        // ...
    }
}

// 运行时检查（始终启用）
#define runtime_assert(cond, msg) do { \
    if (!(cond)) { \
        fprintf(stderr, "Runtime assertion failed: %s\n", msg); \
        abort(); \
    } \
} while(0)
```

---

## 关联导航

### 前置知识

- [安全编码指南](./04_Secure_Coding_Guide.md)
- [MISRA C:2023](../MISRA_C_2023/README.md)
- [CERT C](../CERT_C_2024/README.md)

### 后续延伸

- [并发编程](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md)
- [形式化验证](../../../02_Formal_Semantics_and_Physics/11_CompCert_Verification/README.md)

### 参考

- [SEI CERT C Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c/SEI+CERT+C+Coding+Standard)
- [OWASP C Secure Coding](https://owasp.org/www-project-secure-coding-practices-quick-reference-guide/)
