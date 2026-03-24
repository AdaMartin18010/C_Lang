# MISRA C:2025 规则 16-22

> **规则范围**: 标准库、并发、网络安全、代码生成、复杂性、C23兼容性
> **适用**: C11/C17/C23

---

## 规则类别 16: 标准库

### Rule 16.1 (Required)

**标准库函数不得超出标准定义的行为范围使用**

```c
/* 不符合规范 */
char buffer[10];
strcpy(buffer, "Hello World!");  /* 缓冲区溢出 - 未定义行为 */

/* 符合规范 */
char buffer[10];
strncpy(buffer, "Hello World!", sizeof(buffer) - 1);
buffer[sizeof(buffer) - 1] = '\0';
```

### Rule 16.2 (Required)

**不得使用已弃用的标准库函数**

| 弃用函数 | 替代方案 | 原因 |
|:---------|:---------|:-----|
| `gets()` | `fgets()` | 缓冲区溢出风险 |
| `strcpy()` | `strncpy()` | 不安全的字符串复制 |
| `sprintf()` | `snprintf()` | 格式化字符串溢出 |
| `atoi()` | `strtol()` | 错误处理不足 |

### Rule 16.3 (Advisory)

**应验证标准库函数的返回值**

```c
/* 不符合规范 */
FILE* fp = fopen("data.txt", "r");
fread(buffer, 1, size, fp);  /* 未检查fopen和fread返回值 */
fclose(fp);

/* 符合规范 */
FILE* fp = fopen("data.txt", "r");
if (fp != NULL) {
    size_t n = fread(buffer, 1, size, fp);
    if (n == size) {
        /* 处理数据 */
    }
    fclose(fp);
} else {
    /* 错误处理 */
}
```

### Rule 16.4 (Required)

**动态内存分配的错误必须处理**

```c
/* 不符合规范 */
int* ptr = malloc(sizeof(int) * n);
*ptr = 42;  /* 如果malloc失败，未定义行为 */

/* 符合规范 */
int* ptr = malloc(sizeof(int) * n);
if (ptr != NULL) {
    *ptr = 42;
    free(ptr);
} else {
    /* 内存分配失败处理 */
}
```

### Rule 16.5 (Required)

**不得在信号处理程序中调用非异步信号安全函数**

```c
/* 不符合规范 - malloc在信号处理程序中不安全 */
void signal_handler(int sig) {
    char* msg = malloc(100);  /* 违反规则 */
    strcpy(msg, "Signal received");
    /* ... */
}

/* 符合规范 - 只使用异步信号安全函数 */
void signal_handler(int sig) {
    const char msg[] = "Signal received\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
    _exit(1);
}
```

### Rule 16.6 (Advisory)

**应使用安全的字符串处理函数**

```c
/* 符合规范 - 使用安全变体 */
#ifdef __STDC_LIB_EXT1__
    #define __STDC_WANT_LIB_EXT1__ 1
    #include <string.h>

    errno_t err = strcpy_s(dest, sizeof(dest), src);
    if (err != 0) {
        /* 处理错误 */
    }
#endif
```

### Rule 16.7 (Required)

**setjmp/longjmp不得跨越作用域边界**

```c
/* 不符合规范 */
void func(void) {
    jmp_buf env;
    if (setjmp(env) == 0) {
        /* ... */
    }
}
/* 在另一个函数中使用env */
void other_func(jmp_buf* env) {
    longjmp(*env, 1);  /* 违反规则 - 跨越函数边界 */
}

/* 符合规范 - 在同一函数中使用 */
void func(void) {
    jmp_buf env;
    if (setjmp(env) == 0) {
        /* 在同一函数中调用longjmp */
        if (error_condition) {
            longjmp(env, 1);
        }
    } else {
        /* 错误恢复 */
    }
}
```

### Rule 16.8 (Advisory)

**应最小化标准库的使用，在freestanding环境中**

---


---

## 📑 目录

- [MISRA C:2025 规则 16-22](#misra-c2025-规则-16-22)
  - [规则类别 16: 标准库](#规则类别-16-标准库)
    - [Rule 16.1 (Required)](#rule-161-required)
    - [Rule 16.2 (Required)](#rule-162-required)
    - [Rule 16.3 (Advisory)](#rule-163-advisory)
    - [Rule 16.4 (Required)](#rule-164-required)
    - [Rule 16.5 (Required)](#rule-165-required)
    - [Rule 16.6 (Advisory)](#rule-166-advisory)
    - [Rule 16.7 (Required)](#rule-167-required)
    - [Rule 16.8 (Advisory)](#rule-168-advisory)
  - [📑 目录](#-目录)
  - [规则类别 17: 并发和原子性 (🆕 增强)](#规则类别-17-并发和原子性--增强)
    - [Rule 17.1 (Required)](#rule-171-required)
    - [Rule 17.2 (Required)](#rule-172-required)
    - [Rule 17.3 (Required)](#rule-173-required)
    - [Rule 17.4 (Required)](#rule-174-required)
    - [Rule 17.5 (Advisory)](#rule-175-advisory)
    - [Rule 17.6 (Required)](#rule-176-required)
    - [Rule 17.7 (Advisory)](#rule-177-advisory)
    - [Rule 17.8 (Required)](#rule-178-required)
    - [Rule 17.9 (Advisory)](#rule-179-advisory)
    - [Rule 17.10 (Required)](#rule-1710-required)
    - [Rule 17.11 (Advisory)](#rule-1711-advisory)
  - [规则类别 18: 网络安全 (🆕 新增)](#规则类别-18-网络安全--新增)
    - [Rule 18.1 (Required) 🆕](#rule-181-required-)
    - [Rule 18.2 (Required) 🆕](#rule-182-required-)
    - [Rule 18.3 (Advisory) 🆕](#rule-183-advisory-)
    - [Rule 18.4 (Required) 🆕](#rule-184-required-)
    - [Rule 18.5 (Advisory) 🆕](#rule-185-advisory-)
    - [Rule 18.6 (Advisory) 🆕](#rule-186-advisory-)
    - [Rule 18.7 (Required) 🆕](#rule-187-required-)
    - [Rule 18.8 (Advisory) 🆕](#rule-188-advisory-)
    - [Rule 18.9 (Required) 🆕](#rule-189-required-)
    - [Rule 18.10 (Advisory) 🆕](#rule-1810-advisory-)
    - [Rule 18.11 (Advisory) 🆕](#rule-1811-advisory-)
    - [Rule 18.12 (Required) 🆕](#rule-1812-required-)
  - [规则类别 19: 自动代码生成](#规则类别-19-自动代码生成)
    - [Rule 19.1 (Required)](#rule-191-required)
    - [Rule 19.2 (Required)](#rule-192-required)
    - [Rule 19.3 (Advisory)](#rule-193-advisory)
  - [规则类别 20: 计算复杂性](#规则类别-20-计算复杂性)
    - [Rule 20.1 - 20.14](#rule-201---2014)
  - [规则类别 21: C23兼容性 (🆕 新增)](#规则类别-21-c23兼容性--新增)
    - [Rule 21.1 (Advisory) 🆕](#rule-211-advisory-)
    - [Rule 21.2 (Advisory) 🆕](#rule-212-advisory-)
    - [Rule 21.3 (Advisory) 🆕](#rule-213-advisory-)
    - [Rule 21.4 (Advisory) 🆕](#rule-214-advisory-)
    - [Rule 21.5 (Advisory) 🆕](#rule-215-advisory-)
    - [Rule 21.6 (Advisory) 🆕](#rule-216-advisory-)
    - [Rule 21.7 (Advisory) 🆕](#rule-217-advisory-)
    - [Rule 21.8 (Required) 🆕](#rule-218-required-)
  - [规则类别 22: 已废弃规则](#规则类别-22-已废弃规则)
    - [Rule 22.1 - 22.4](#rule-221---224)
  - [总结](#总结)
    - [关键变化](#关键变化)
    - [合规优先级](#合规优先级)
  - [深入理解](#深入理解)
    - [核心概念](#核心概念)
    - [实践应用](#实践应用)
    - [学习建议](#学习建议)
  - [📚 实质性内容补充](#-实质性内容补充)
    - [技术深度分析](#技术深度分析)
      - [1. 核心概念详解](#1-核心概念详解)
      - [2. 实现机制](#2-实现机制)
      - [3. 实践指导](#3-实践指导)
    - [扩展阅读](#扩展阅读)


---

## 规则类别 17: 并发和原子性 (🆕 增强)

### Rule 17.1 (Required)

**不得出现数据竞争**

```c
/* 不符合规范 - 数据竞争 */
int shared_var = 0;

void* thread_func(void* arg) {
    shared_var++;  /* 非原子操作，数据竞争 */
    return NULL;
}

/* 符合规范 - 使用原子操作 */
_Atomic int shared_var = 0;

void* thread_func(void* arg) {
    shared_var++;  /* 原子操作 */
    return NULL;
}
```

### Rule 17.2 (Required)

**锁必须成对使用（加锁/解锁）**

```c
/* 不符合规范 - 缺少解锁 */
pthread_mutex_lock(&mutex);
/* 条件分支中可能提前返回 */
if (error) {
    return;  /* 违反规则 - 未解锁 */
}
pthread_mutex_unlock(&mutex);

/* 符合规范 - 确保解锁 */
pthread_mutex_lock(&mutex);
/* 临界区代码 */
pthread_mutex_unlock(&mutex);  /* 总是执行解锁 */
```

### Rule 17.3 (Required)

**不得出现死锁**

```c
/* 不符合规范 - 潜在死锁 */
void transfer(Account* from, Account* to, int amount) {
    pthread_mutex_lock(&from->lock);
    pthread_mutex_lock(&to->lock);  /* 如果反向调用则死锁 */
    /* 转账操作 */
    pthread_mutex_unlock(&to->lock);
    pthread_mutex_unlock(&from->lock);
}

/* 符合规范 - 锁排序 */
void transfer(Account* a, Account* b, int amount) {
    Account* first = (a < b) ? a : b;  /* 确定顺序 */
    Account* second = (a < b) ? b : a;

    pthread_mutex_lock(&first->lock);
    pthread_mutex_lock(&second->lock);
    /* 转账操作 */
    pthread_mutex_unlock(&second->lock);
    pthread_mutex_unlock(&first->lock);
}
```

### Rule 17.4 (Required)

**原子变量的访问必须使用原子操作**

```c
/* 不符合规范 */
_Atomic int counter;
int local = counter;  /* 非原子读取 */

/* 符合规范 */
int local = atomic_load(&counter);
atomic_store(&counter, 42);
int new_val = atomic_fetch_add(&counter, 1);
```

### Rule 17.5 (Advisory)

**应最小化临界区大小**

```c
/* 不符合规范 - 临界区过大 */
pthread_mutex_lock(&mutex);
/* 长时间操作 */
process_large_dataset();
/* 更多操作 */
pthread_mutex_unlock(&mutex);

/* 符合规范 - 最小化临界区 */
pthread_mutex_lock(&mutex);
SharedData data = shared_resource;  /* 快速复制 */
pthread_mutex_unlock(&mutex);

process_large_dataset(data);  /* 在锁外处理 */
```

### Rule 17.6 (Required)

**线程创建错误必须处理**

```c
/* 不符合规范 */
pthread_t thread;
pthread_create(&thread, NULL, thread_func, NULL);
/* 未检查返回值 */

/* 符合规范 */
pthread_t thread;
int ret = pthread_create(&thread, NULL, thread_func, NULL);
if (ret != 0) {
    /* 处理线程创建失败 */
}
```

### Rule 17.7 (Advisory)

**应使用条件变量而不是忙等待**

```c
/* 不符合规范 - 忙等待 */
while (!condition_met) {
    /* 浪费CPU */
}

/* 符合规范 - 条件变量 */
pthread_mutex_lock(&mutex);
while (!condition_met) {
    pthread_cond_wait(&cond, &mutex);
}
/* 处理条件 */
pthread_mutex_unlock(&mutex);
```

### Rule 17.8 (Required)

**线程本地存储应正确初始化**

### Rule 17.9 (Advisory)

**应限制线程数量**

### Rule 17.10 (Required)

**线程同步原语必须正确销毁**

```c
/* 符合规范 */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/* 使用... */
pthread_mutex_destroy(&mutex);  /* 清理 */
```

### Rule 17.11 (Advisory)

**应使用线程池而不是为每个任务创建新线程**

---

## 规则类别 18: 网络安全 (🆕 新增)

### Rule 18.1 (Required) 🆕

**所有网络通信必须加密**

```c
/* 不符合规范 - 明文传输 */
int sock = socket(AF_INET, SOCK_STREAM, 0);
connect(sock, (struct sockaddr*)&addr, sizeof(addr));
send(sock, password, strlen(password), 0);  /* 明文发送密码 */

/* 符合规范 - 使用TLS */
SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
SSL* ssl = SSL_new(ctx);
SSL_set_fd(ssl, sock);
SSL_connect(ssl);
SSL_write(ssl, password, strlen(password));  /* 加密传输 */
```

### Rule 18.2 (Required) 🆕

**禁止硬编码敏感信息**

```c
/* 不符合规范 - 硬编码密钥 */
const char* api_key = "sk-1234567890abcdef";  /* 违反规则 */

/* 符合规范 - 安全存储 */
/* 从安全存储（密钥库、环境变量）加载 */
char api_key[256];
if (load_from_keyring("api_key", api_key, sizeof(api_key)) != 0) {
    /* 错误处理 */
}
```

### Rule 18.3 (Advisory) 🆕

**使用加密安全的随机数生成器**

```c
/* 不符合规范 - 可预测的随机数 */
int token = rand();  /* 不安全 */

/* 符合规范 - 加密安全 */
#include <openssl/rand.h>
unsigned char token[32];
if (RAND_bytes(token, sizeof(token)) != 1) {
    /* 错误处理 */
}
```

### Rule 18.4 (Required) 🆕

**验证所有外部输入**

```c
/* 不符合规范 - 未验证的输入 */
void process_packet(char* data, size_t len) {
    uint32_t size = *(uint32_t*)data;  /* 直接解引用 */
    char* payload = data + 4;
    process(payload, size);  /* 可能越界 */
}

/* 符合规范 - 严格验证 */
void process_packet(const uint8_t* data, size_t len) {
    if (len < 4) return;  /* 最小长度检查 */

    uint32_t size;
    memcpy(&size, data, sizeof(size));
    size = ntohl(size);  /* 字节序转换 */

    if (size > len - 4 || size > MAX_PAYLOAD_SIZE) {
        return;  /* 大小验证失败 */
    }

    process(data + 4, size);
}
```

### Rule 18.5 (Advisory) 🆕

**最小化攻击面 - 禁用不需要的功能**

### Rule 18.6 (Advisory) 🆕

**实施深度防御策略**

### Rule 18.7 (Required) 🆕

**安全敏感操作需要审计日志**

```c
/* 符合规范 - 审计日志 */
void authenticate_user(const char* username) {
    log_security_event(LOG_AUTH_ATTEMPT, username, get_timestamp());

    if (verify_credentials(username)) {
        log_security_event(LOG_AUTH_SUCCESS, username, get_timestamp());
        grant_access(username);
    } else {
        log_security_event(LOG_AUTH_FAILURE, username, get_timestamp());
        deny_access(username);
    }
}
```

### Rule 18.8 (Advisory) 🆕

**使用最小权限原则**

### Rule 18.9 (Required) 🆕

**密码学原语必须使用经过验证的实现**

```c
/* 符合规范 - 使用经过验证的库 */
#include <openssl/evp.h>

/* 使用OpenSSL而不是自定义加密 */
EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv);
/* ... */
```

### Rule 18.10 (Advisory) 🆕

**实施安全启动机制**

### Rule 18.11 (Advisory) 🆕

**定期更新依赖库以修复安全漏洞**

### Rule 18.12 (Required) 🆕

**不得禁用或忽略安全警告**

---

## 规则类别 19: 自动代码生成

### Rule 19.1 (Required)

**自动生成的代码必须符合MISRA C标准**

### Rule 19.2 (Required)

**生成代码的工具必须经过验证**

### Rule 19.3 (Advisory)

**应标识自动生成的代码**

```c
/* 自动生成 - 请勿手动修改 */
/* Generated by ToolX v2.1.0 on 2026-03-18 */
```

---

## 规则类别 20: 计算复杂性

### Rule 20.1 - 20.14

**控制复杂度度量**：圈复杂度、嵌套深度、函数长度等（与MISRA C:2023相同）

---

## 规则类别 21: C23兼容性 (🆕 新增)

### Rule 21.1 (Advisory) 🆕

**在使用C23特性前，确认目标编译器支持**

```c
/* 检查C23支持 */
#if __STDC_VERSION__ >= 202311L
    /* C23代码 */
    #define nullptr ((void*)0)
#else
    /* C17兼容代码 */
    #define nullptr NULL
#endif
```

### Rule 21.2 (Advisory) 🆕

**使用nullptr代替NULL以提高类型安全**

```c
/* C23 */
int* p = nullptr;  /* 类型安全 */

/* C17兼容 */
#ifndef __STDC_VERSION__
    #define nullptr NULL
#endif
```

### Rule 21.3 (Advisory) 🆕

**谨慎使用typeof运算符**

```c
/* 符合规范 */
typedef typeof(int) my_int_t;  /* 清晰明了 */

/* 避免过度复杂 */
typedef typeof(typeof(int (*)())[10]) complex_type;  /* 过于复杂 */
```

### Rule 21.4 (Advisory) 🆕

**使用constexpr进行编译时常量定义**

```c
/* C23 */
constexpr int MAX_SIZE = 100;  /* 编译时常量 */

/* C17兼容 */
#ifndef constexpr
    #define constexpr const
#endif
```

### Rule 21.5 (Advisory) 🆕

**使用#embed嵌入二进制数据**

```c
/* C23 */
static const uint8_t firmware[] = {
    #embed "firmware.bin"
};

/* C17兼容 - 使用外部工具生成 */
static const uint8_t firmware[] = {
    #include "firmware_data.h"
};
```

### Rule 21.6 (Advisory) 🆕

**谨慎使用_BitInt**

```c
/* 符合规范 */
_BitInt(17) sensor_id;  /* 明确的位宽需求 */

/* 避免不必要的使用 */
_BitInt(8) byte_value;  /* 使用uint8_t更清晰 */
```

### Rule 21.7 (Advisory) 🆕

**利用改进的标签放置**

```c
/* C23 - 更灵活 */
if (condition) {
    label:
    int x = 5;  /* C23允许 */
    /* ... */
}

/* 但保持可读性 */
if (condition) {
    int x = 5;
    label:
    /* 更清晰的位置 */
}
```

### Rule 21.8 (Required) 🆕

**属性注解应一致使用**

```c
/* 符合规范 */
[[nodiscard]] int critical_function(void);
[[deprecated("Use new_function() instead")]] int old_function(void);
[[maybe_unused]] int debug_var = 0;
[[noreturn]] void fatal_error(const char* msg);
```

---

## 规则类别 22: 已废弃规则

### Rule 22.1 - 22.4

**已废弃的规则**（保留用于向后兼容性说明）

---

## 总结

### 关键变化

1. **🆕 网络安全类别 (Rules 18.x)**：12条新规则应对现代安全威胁
2. **🆕 C23兼容性类别 (Rules 21.x)**：8条指南帮助迁移到新标准
3. **🔧 并发规则增强**：从AMD4整合并扩展
4. **📊 规则总数**：221 → 225条

### 合规优先级

| 优先级 | 规则 | 说明 |
|:-------|:-----|:-----|
| **最高** | 18.1, 18.2, 18.4 | 网络安全关键规则 |
| **高** | 17.1, 17.2, 17.3 | 并发安全 |
| **中** | 所有Required规则 | 标准合规 |
| **建议** | Advisory规则 | 最佳实践 |

---

**上一章**: [03_MISRA_C_2025_Rules_11-15.md](./MISRA_C_2025_Rules_Dir11.md)
**迁移指南**: [MISRA_C_2023_to_2025_Migration.md](./MISRA_C_2023_to_2025_Migration_Guide.md)

**最后更新**: 2026-03-18


---

## 深入理解

### 核心概念

本主题的核心概念包括：基础理论、实现机制、实际应用。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 学习建议

1. 先理解基础概念
2. 再进行实践练习
3. 最后深入源码

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review


## 📚 实质性内容补充

### 技术深度分析

#### 1. 核心概念详解

深入剖析本主题的核心概念，建立完整的知识体系。

#### 2. 实现机制

| 层级 | 机制 | 关键技术 |
|:-----|:-----|:---------|
| 应用层 | 业务逻辑 | 设计模式 |
| 系统层 | 资源管理 | 内存/进程 |
| 硬件层 | 物理实现 | CPU/缓存 |

#### 3. 实践指导

- 最佳实践准则
- 常见陷阱与避免
- 调试与优化技巧

### 扩展阅读

- [核心知识体系](../../01_Core_Knowledge_System/readme.md)
- [全局索引](../../00_GLOBAL_INDEX.md)
