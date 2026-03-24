# C语言安全编程完全指南


> **版本**: v1.0.0
> **最后更新**: 2026-03-25
> **作者**: C_Lang Team
> **难度**: L2 基础

---
> **版本**: v1.0
> **更新日期**: 2026-03-16
> **适用范围**: C99/C11/C17/C23
> **安全等级**: 生产环境必备

---

## 目录

- [C语言安全编程完全指南](#c语言安全编程完全指南)
  - [目录](#目录)
  - [1. 安全威胁模型](#1-安全威胁模型)
    - [1.1 STRIDE威胁分类](#11-stride威胁分类)
    - [1.2 C语言特有风险](#12-c语言特有风险)
    - [1.3 攻击面分析](#13-攻击面分析)
    - [1.4 威胁建模检查清单](#14-威胁建模检查清单)
  - [2. 输入验证](#2-输入验证)
    - [2.1 缓冲区溢出防护](#21-缓冲区溢出防护)
    - [2.2 整数溢出检测](#22-整数溢出检测)
    - [2.3 格式化字符串安全](#23-格式化字符串安全)
    - [2.4 命令注入防护](#24-命令注入防护)
  - [3. 内存安全](#3-内存安全)
    - [3.1 安全内存管理模式](#31-安全内存管理模式)
    - [3.2 Use After Free 防护](#32-use-after-free-防护)
    - [3.3 双重释放防护](#33-双重释放防护)
    - [3.4 未初始化内存防护](#34-未初始化内存防护)
    - [3.5 安全内存清零](#35-安全内存清零)
  - [4. 字符串安全](#4-字符串安全)
    - [4.1 安全字符串函数](#41-安全字符串函数)
    - [4.2 多字节字符安全](#42-多字节字符安全)
    - [4.3 UTF-8安全处理](#43-utf-8安全处理)
  - [5. 并发安全](#5-并发安全)
    - [5.1 数据竞争防护](#51-数据竞争防护)
    - [5.2 死锁避免](#52-死锁避免)
    - [5.3 原子操作使用](#53-原子操作使用)
    - [5.4 线程局部存储](#54-线程局部存储)
  - [6. 密码学安全](#6-密码学安全)
    - [6.1 随机数生成](#61-随机数生成)
    - [6.2 哈希函数使用](#62-哈希函数使用)
    - [6.3 密钥管理](#63-密钥管理)
    - [6.4 敏感数据擦除](#64-敏感数据擦除)
  - [7. 防御性编程](#7-防御性编程)
    - [7.1 断言和不变式](#71-断言和不变式)
    - [7.2 错误处理策略](#72-错误处理策略)
    - [7.3 日志安全](#73-日志安全)
    - [7.4 最小权限原则](#74-最小权限原则)
  - [8. 静态分析工具](#8-静态分析工具)
    - [8.1 GCC/Clang警告选项](#81-gccclang警告选项)
    - [8.2 Clang Static Analyzer](#82-clang-static-analyzer)
    - [8.3 Coverity Scan](#83-coverity-scan)
    - [8.4 CodeQL](#84-codeql)
  - [9. 运行时检测](#9-运行时检测)
    - [9.1 AddressSanitizer (ASan)](#91-addresssanitizer-asan)
    - [9.2 MemorySanitizer (MSan)](#92-memorysanitizer-msan)
    - [9.3 UndefinedBehaviorSanitizer (UBSan)](#93-undefinedbehaviorsanitizer-ubsan)
    - [9.4 Control-Flow Integrity (CFI)](#94-control-flow-integrity-cfi)
    - [9.5 综合运行时检测脚本](#95-综合运行时检测脚本)
  - [10. 安全开发生命周期](#10-安全开发生命周期)
    - [10.1 安全设计审查](#101-安全设计审查)
    - [10.2 代码审计检查清单](#102-代码审计检查清单)
    - [10.3 渗透测试指南](#103-渗透测试指南)
    - [10.4 漏洞响应流程](#104-漏洞响应流程)
  - [附录A: 安全编码速查表](#附录a-安全编码速查表)
    - [危险函数与替代方案](#危险函数与替代方案)
    - [编译安全选项速查](#编译安全选项速查)
  - [附录B: 参考资源](#附录b-参考资源)
    - [标准与规范](#标准与规范)
    - [工具资源](#工具资源)
    - [安全库](#安全库)
  - [深入理解](#深入理解)
    - [核心概念](#核心概念)
    - [实践应用](#实践应用)
    - [学习建议](#学习建议)

---

## 1. 安全威胁模型

### 1.1 STRIDE威胁分类

| 威胁类型 | 描述 | C语言典型风险 |
|---------|------|--------------|
| **S**poofing (欺骗) | 冒充身份 | 弱认证、证书验证缺失 |
| **T**ampering (篡改) | 修改数据 | 缓冲区溢出、整数溢出 |
| **R**epudiation (否认) | 否认操作 | 日志不足、审计缺失 |
| **I**nformation Disclosure (信息泄露) | 敏感信息暴露 | 未初始化内存、越界读取 |
| **D**enial of Service (拒绝服务) | 服务中断 | 资源耗尽、死循环、崩溃 |
| **E**levation of Privilege (权限提升) | 获取更高权限 | 缓冲区溢出执行代码 |

### 1.2 C语言特有风险

```c
/**
 * C语言固有风险分析
 */

// 风险1: 无边界检查
void unsafe_buffer(void) {
    char buf[10];
    gets(buf);  // ⚠️ 极度危险，已移除C11标准
}

// 风险2: 手动内存管理
void unsafe_memory(void) {
    char *p = malloc(100);
    free(p);
    free(p);  // ⚠️ 双重释放
}

// 风险3: 未定义行为
void undefined_behavior(void) {
    int a[10];
    int x = a[20];  // ⚠️ 未定义行为，可能泄露信息
}

// 风险4: 整数溢出
void integer_overflow(void) {
    size_t len = 0xFFFFFFFF;
    char *buf = malloc(len + 1);  // ⚠️ 溢出后分配过小缓冲区
}
```

### 1.3 攻击面分析

```
┌─────────────────────────────────────────────────────────────┐
│                    C程序攻击面                               │
├─────────────────────────────────────────────────────────────┤
│  输入层          处理层           输出层        环境层        │
│  ┌──────┐      ┌──────┐        ┌──────┐     ┌──────┐       │
│  │文件  │─────▶│解析  │───────▶│网络  │     │环境变量│       │
│  ├──────┤      ├──────┤        ├──────┤     ├──────┤       │
│  │网络  │─────▶│计算  │───────▶│文件  │────▶│配置  │       │
│  ├──────┤      ├──────┤        ├──────┤     ├──────┤       │
│  │用户  │─────▶│内存  │───────▶│日志  │     │权限  │       │
│  ├──────┤      ├──────┤        ├──────┤     ├──────┤       │
│  │环境  │─────▶│字符串│───────▶│屏幕  │     │资源  │       │
│  └──────┘      └──────┘        └──────┘     └──────┘       │
└─────────────────────────────────────────────────────────────┘
```

### 1.4 威胁建模检查清单

- [ ] 识别所有信任边界
- [ ] 列出所有数据输入点
- [ ] 分析数据流和存储
- [ ] 评估STRIDE各威胁
- [ ] 定义安全假设和约束

---

## 2. 输入验证

### 2.1 缓冲区溢出防护

**威胁描述**: 向缓冲区写入超出其容量的数据，覆盖相邻内存。

**攻击示例**:

```c
// 漏洞代码：栈缓冲区溢出
void vulnerable_function(char *input) {
    char buffer[64];
    strcpy(buffer, input);  // ⚠️ 无长度检查
    // 攻击者输入65+字节可覆盖返回地址
}
```

**防护代码**:

```c
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// 安全版本1: 使用带长度限制的函数
bool safe_copy_v1(const char *input, char *output, size_t out_len) {
    if (!input || !output || out_len == 0) {
        return false;
    }

    size_t in_len = strnlen(input, out_len);
    if (in_len >= out_len) {
        // 输入过长，截断或拒绝
        return false;
    }

    memcpy(output, input, in_len);
    output[in_len] = '\0';
    return true;
}

// 安全版本2: 使用strlcpy (BSD/Linux)
#ifdef __linux__
#include <bsd/string.h>  // 需安装libbsd-dev
#else
#include <string.h>
#endif

bool safe_copy_v2(const char *input, char *output, size_t out_len) {
    if (!input || !output || out_len == 0) {
        return false;
    }

    size_t result = strlcpy(output, input, out_len);
    return result < out_len;  // 返回true表示未截断
}

// 安全版本3: C11边界检查接口
#if defined(__STDC_LIB_EXT1__) && (__STDC_WANT_LIB_EXT1__ == 1)
bool safe_copy_v3(const char *input, char *output, size_t out_len) {
    if (!input || !output || out_len == 0) {
        return false;
    }

    errno_t err = strncpy_s(output, out_len, input, _TRUNCATE);
    return err == 0;
}
#endif
```

**检测方法**:

```bash
# GCC/Clang栈保护
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
gcc -fstack-protector-strong -Wstack-protector -fstack-check

# 运行时检测
valgrind --tool=memcheck --leak-check=full ./program
```

### 2.2 整数溢出检测

**威胁描述**: 算术运算结果超出整数类型表示范围，导致未定义行为。

**攻击示例**:

```c
// 漏洞代码：整数溢出导致缓冲区过小
void *vulnerable_alloc(size_t count, size_t size) {
    return malloc(count * size);  // ⚠️ 乘法溢出
}

// 攻击：count=0x40000000, size=4 → 溢出为0，分配0字节但后续写入16GB
```

**防护代码**:

```c
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

// 安全乘法 - 方式1: 手动检查
bool safe_multiply(size_t a, size_t b, size_t *result) {
    if (a == 0 || b == 0) {
        *result = 0;
        return true;
    }

    // 检查溢出: a * b > SIZE_MAX
    if (a > SIZE_MAX / b) {
        return false;  // 溢出
    }

    *result = a * b;
    return true;
}

// 安全乘法 - 方式2: 使用内置函数(GCC/Clang)
bool safe_multiply_builtin(size_t a, size_t b, size_t *result) {
    return __builtin_mul_overflow(a, b, result) == false;
}

// 安全加法
bool safe_add(size_t a, size_t b, size_t *result) {
    if (a > SIZE_MAX - b) {
        return false;
    }
    *result = a + b;
    return true;
}

// 安全分配器
void *safe_calloc(size_t count, size_t size) {
    size_t total;
    if (!safe_multiply(count, size, &total)) {
        return NULL;  // 溢出
    }
    return calloc(count, size);
}

// 完整安全的内存分配包装
void *safe_malloc_array(size_t count, size_t elem_size) {
    size_t total_size;

    // 检查参数有效性
    if (count == 0 || elem_size == 0) {
        return NULL;
    }

    // 检查乘法溢出
    if (!safe_multiply(count, elem_size, &total_size)) {
        return NULL;
    }

    // 检查加上头部信息后的溢出（如有）
    const size_t header_size = sizeof(size_t) * 2;
    size_t alloc_size;
    if (!safe_add(total_size, header_size, &alloc_size)) {
        return NULL;
    }

    void *ptr = malloc(alloc_size);
    if (ptr) {
        // 记录元数据用于调试
        size_t *metadata = ptr;
        metadata[0] = count;
        metadata[1] = elem_size;
        return metadata + 2;
    }
    return NULL;
}
```

### 2.3 格式化字符串安全

**威胁描述**: 用户输入直接作为格式化字符串，可导致内存读取/写入。

**攻击示例**:

```c
// 漏洞代码
void vulnerable_log(const char *user_input) {
    printf(user_input);  // ⚠️ 用户可控格式化字符串
    // 攻击者输入: "%p %p %p %s" 可泄露栈内存
    // 攻击者输入: "%n" 可写入内存
}
```

**防护代码**:

```c
#include <stdio.h>
#include <stdarg.h>

// 安全方式1: 使用格式化字符串占位符
void safe_log_v1(const char *user_input) {
    printf("%s", user_input);  // ✓ 用户输入作为参数，非格式字符串
}

// 安全方式2: 包装函数强制检查
void safe_printf(const char *fmt, ...) {
    if (!fmt) return;

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

// 安全的错误日志宏
#define SAFE_LOG_ERROR(msg) do { \
    fprintf(stderr, "[ERROR] %s:%d: %s\n", __FILE__, __LINE__, (msg)); \
} while(0)

// 禁止直接使用用户输入作为格式字符串的静态分析注解
#ifdef __GNUC__
__attribute__((format(printf, 1, 2)))
#endif
void safe_format_log(const char *fmt, ...) {
    // 必须提供格式字符串
}

// 用户输入验证函数
bool validate_format_input(const char *input) {
    if (!input) return false;

    // 检查危险格式化字符
    const char *dangerous = "%*";
    for (const char *p = input; *p; p++) {
        if (*p == '%' || *p == '*') {
            return false;  // 拒绝包含格式化指令的输入
        }
    }
    return true;
}
```

### 2.4 命令注入防护

**威胁描述**: 用户输入被拼接到系统命令，导致任意命令执行。

**攻击示例**:

```c
// 漏洞代码
void vulnerable_exec(const char *filename) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ls -la %s", filename);
    system(cmd);  // ⚠️ filename="; rm -rf /"
}
```

**防护代码**:

```c
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

// 方式1: 使用exec系列函数（避免shell）
int safe_exec_ls(const char *filename) {
    // 验证文件名 - 只允许安全字符
    for (const char *p = filename; *p; p++) {
        if (!isalnum(*p) && *p != '.' && *p != '_' && *p != '-') {
            errno = EINVAL;
            return -1;
        }
    }

    pid_t pid = fork();
    if (pid == -1) {
        return -1;
    }

    if (pid == 0) {
        // 子进程
        execlp("ls", "ls", "-la", filename, (char *)NULL);
        _exit(127);  // exec失败
    }

    // 父进程等待
    int status;
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
}

// 方式2: 参数化执行（更安全）
int safe_execvp(const char *file, char *const argv[]) {
    // 不使用system，直接使用execvp
    pid_t pid = fork();
    if (pid == 0) {
        execvp(file, argv);
        _exit(127);
    }

    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

// 方式3: 使用libselinux或seccomp限制
#ifdef USE_SECCOMP
#include <seccomp.h>

void setup_seccomp(void) {
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL);
    if (!ctx) return;

    // 只允许白名单系统调用
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);

    seccomp_load(ctx);
    seccomp_release(ctx);
}
#endif
```

---

## 3. 内存安全

### 3.1 安全内存管理模式

**威胁描述**: C语言手动内存管理容易出错，导致UAF、双重释放等漏洞。

**防护代码**:

```c
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

// 内存分配状态跟踪
#define MAX_TRACKED_PTRS 1024

typedef struct {
    void *ptr;
    size_t size;
    bool freed;
    const char *file;
    int line;
} mem_record_t;

static mem_record_t mem_records[MAX_TRACKED_PTRS];
static int mem_count = 0;

// 安全分配宏
#define SAFE_MALLOC(size) safe_malloc_impl((size), __FILE__, __LINE__)
#define SAFE_FREE(ptr) safe_free_impl(&(ptr))
#define SAFE_CALLOC(n, size) safe_calloc_impl((n), (size), __FILE__, __LINE__)
#define SAFE_REALLOC(ptr, size) safe_realloc_impl((ptr), (size), __FILE__, __LINE__)

void *safe_malloc_impl(size_t size, const char *file, int line) {
    if (size == 0) return NULL;
    if (size > PTRDIFF_MAX) return NULL;  // 防止溢出

    void *ptr = malloc(size);
    if (ptr) {
        if (mem_count < MAX_TRACKED_PTRS) {
            mem_records[mem_count++] = (mem_record_t){
                .ptr = ptr,
                .size = size,
                .freed = false,
                .file = file,
                .line = line
            };
        }
        // 初始化内存（可选，但更安全）
        memset(ptr, 0, size);
    }
    return ptr;
}

void safe_free_impl(void **ptr) {
    if (!ptr || !*ptr) return;

    // 查找记录
    for (int i = 0; i < mem_count; i++) {
        if (mem_records[i].ptr == *ptr) {
            if (mem_records[i].freed) {
                // 双重释放检测
                fprintf(stderr, "[FATAL] Double free at %p\n", *ptr);
                abort();
            }
            mem_records[i].freed = true;
            break;
        }
    }

    free(*ptr);
    *ptr = NULL;  // 防止悬空指针
}

void *safe_calloc_impl(size_t nmemb, size_t size, const char *file, int line) {
    size_t total;
    if (__builtin_mul_overflow(nmemb, size, &total)) {
        return NULL;
    }
    return safe_malloc_impl(total, file, line);
}
```

### 3.2 Use After Free 防护

**威胁描述**: 释放内存后继续使用，可能导致数据泄露或代码执行。

**防护代码**:

```c
// UAF检测模式
#define UAF_SENTINEL 0xDEADBEEFCAFEBABEULL

// 带哨兵的内存结构
typedef struct {
    uint64_t sentinel;
    size_t size;
    char data[];  // 柔性数组
} guarded_block_t;

void *guarded_alloc(size_t size) {
    guarded_block_t *block = malloc(sizeof(guarded_block_t) + size);
    if (!block) return NULL;

    block->sentinel = UAF_SENTINEL;
    block->size = size;

    return block->data;
}

void guarded_free(void *ptr) {
    if (!ptr) return;

    guarded_block_t *block = (void *)((char *)ptr - sizeof(guarded_block_t));

    // 验证哨兵
    if (block->sentinel != UAF_SENTINEL) {
        fprintf(stderr, "[FATAL] Invalid free or heap corruption\n");
        abort();
    }

    // 毒化内存
    memset(block->data, 0xDD, block->size);  // 0xDD = dead
    block->sentinel = 0xBADC0DE;

    free(block);
}

// 智能指针模拟（简化版）
typedef struct {
    void *ptr;
    bool owned;
} smart_ptr_t;

smart_ptr_t make_smart(void *p) {
    return (smart_ptr_t){.ptr = p, .owned = true};
}

void smart_free(smart_ptr_t *sp) {
    if (sp && sp->owned) {
        free(sp->ptr);
        sp->ptr = NULL;
        sp->owned = false;
    }
}

// 使用RAII模式
#define SCOPE_PTR(type, name, init) \
    type name __attribute__((cleanup(smart_cleanup))) = (init)
```

### 3.3 双重释放防护

**防护代码**:

```c
#include <pthread.h>

static pthread_mutex_t free_lock = PTHREAD_MUTEX_INITIALIZER;

// 线程安全的双重释放防护
void thread_safe_free(void **ptr) {
    if (!ptr || !*ptr) return;

    pthread_mutex_lock(&free_lock);

    // 原子操作：检查并置空
    void *to_free = *ptr;
    *ptr = NULL;  // 立即置空

    pthread_mutex_unlock(&free_lock);

    free(to_free);
}

// 版本2: 引用计数（适用于共享内存）
typedef struct refcounted {
    size_t count;
    void (*destructor)(void *);
    char data[];
} refcounted_t;

void *refcounted_alloc(size_t size, void (*dtor)(void *)) {
    refcounted_t *rc = malloc(sizeof(refcounted_t) + size);
    if (!rc) return NULL;

    rc->count = 1;
    rc->destructor = dtor;
    return rc->data;
}

void refcounted_acquire(void *ptr) {
    if (!ptr) return;
    refcounted_t *rc = (void *)((char *)ptr - sizeof(refcounted_t));
    __sync_fetch_and_add(&rc->count, 1);
}

void refcounted_release(void **ptr) {
    if (!ptr || !*ptr) return;

    refcounted_t *rc = (void *)((char *)*ptr - sizeof(refcounted_t));
    if (__sync_sub_and_fetch(&rc->count, 1) == 0) {
        if (rc->destructor) {
            rc->destructor(rc->data);
        }
        free(rc);
    }
    *ptr = NULL;
}
```

### 3.4 未初始化内存防护

**防护代码**:

```c
// 强制初始化宏
#define DECLARE_ARRAY(type, name, size) \
    type name[size] = {0}

#define DECLARE_STRUCT(type, name) \
    type name = {0}

// 安全结构体初始化
typedef struct {
    char name[64];
    int age;
    char *data;
    size_t data_len;
} person_t;

person_t create_person(void) {
    // C23前: 全部初始化为0
    person_t p = {0};
    return p;
}

// C23 designated initializers with zero fill
#if __STDC_VERSION__ >= 202311L
person_t create_person_c23(void) {
    return (person_t){.name = ""};  // 其余自动为0
}
#endif

// 内存清零函数 - C11标准
#ifdef __STDC_LIB_EXT1__
#define SAFE_MEMZERO(ptr, size) \
    memset_s((ptr), (size), 0, (size))
#else
// 自定义安全清零（防止编译器优化）
static volatile void *(*volatile memset_ptr)(void *, int, size_t) = memset;

void secure_zero(void *ptr, size_t len) {
    memset_ptr(ptr, 0, len);
    // 内存屏障防止重排序
    __asm__ __volatile__("" ::: "memory");
}
#define SAFE_MEMZERO(ptr, size) secure_zero((ptr), (size))
#endif
```

### 3.5 安全内存清零

```c
/**
 * 安全内存清零 - 防止编译器优化删除
 */

#if defined(__STDC_LIB_EXT1__) && (__STDC_WANT_LIB_EXT1__ == 1)
// 使用C11 memset_s
#define SECURE_ZERO(ptr, len) memset_s((ptr), (len), 0, (len))

#elif defined(__GNUC__) || defined(__clang__)
// GCC/Clang: 使用显式内存屏障
static inline void secure_zero_memory(void *ptr, size_t len) {
    volatile unsigned char *p = ptr;
    while (len--) {
        *p++ = 0;
    }
    __asm__ __volatile__("" ::: "memory");
}
#define SECURE_ZERO(ptr, len) secure_zero_memory((ptr), (len))

#elif defined(_WIN32)
// Windows: 使用SecureZeroMemory
#include <windows.h>
#define SECURE_ZERO(ptr, len) SecureZeroMemory((ptr), (len))

#elif defined(__APPLE__)
// macOS: 使用memset_s
#include <string.h>
#define SECURE_ZERO(ptr, len) memset_s((ptr), (len), 0, (len))

#else
// 通用方案：volatile函数指针
static void *(*volatile secure_memset)(void *, int, size_t) = memset;
#define SECURE_ZERO(ptr, len) secure_memset((ptr), 0, (len))
#endif

// 敏感数据清理示例
void process_password(const char *input) {
    char password[128];

    if (strlen(input) >= sizeof(password)) {
        return;
    }

    strncpy(password, input, sizeof(password) - 1);
    password[sizeof(password) - 1] = '\0';

    // 使用密码...
    verify_password(password);

    // 立即安全清零
    SECURE_ZERO(password, sizeof(password));
}

// 密钥结构自动清理
typedef struct {
    unsigned char key[32];
    unsigned char iv[16];
} crypto_key_t;

void crypto_key_cleanup(crypto_key_t *key) {
    if (key) {
        SECURE_ZERO(key->key, sizeof(key->key));
        SECURE_ZERO(key->iv, sizeof(key->iv));
    }
}

// GCC/Clang cleanup属性
#ifdef __GNUC__
#define AUTO_ZERO(type, name) \
    type name __attribute__((cleanup(cleanup_##type)))

static inline void cleanup_crypto_key(crypto_key_t **k) {
    crypto_key_cleanup(*k);
}
#endif
```

---

## 4. 字符串安全

### 4.1 安全字符串函数

**威胁描述**: C标准字符串函数缺乏边界检查，易导致缓冲区溢出。

**防护代码**:

```c
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// strlcpy实现（如果系统不提供）
#ifndef HAVE_STRLCPY
size_t strlcpy(char *dst, const char *src, size_t size) {
    size_t srclen = strlen(src);

    if (size > 0) {
        size_t copylen = (srclen >= size) ? size - 1 : srclen;
        memcpy(dst, src, copylen);
        dst[copylen] = '\0';
    }

    return srclen;  // 返回源字符串长度
}

size_t strlcat(char *dst, const char *src, size_t size) {
    size_t dstlen = strnlen(dst, size);

    if (dstlen >= size) {
        return dstlen + strlen(src);  // dst未正确终止
    }

    size_t srclen = strlen(src);
    size_t copylen = (srclen >= size - dstlen) ? size - dstlen - 1 : srclen;

    memcpy(dst + dstlen, src, copylen);
    dst[dstlen + copylen] = '\0';

    return dstlen + srclen;
}
#endif

// 安全字符串类型
typedef struct {
    char data[256];
    size_t len;
    size_t capacity;
} safe_string_t;

void safe_string_init(safe_string_t *str) {
    if (!str) return;
    str->data[0] = '\0';
    str->len = 0;
    str->capacity = sizeof(str->data) - 1;
}

bool safe_string_set(safe_string_t *str, const char *src) {
    if (!str || !src) return false;

    size_t srclen = strlen(src);
    if (srclen > str->capacity) {
        return false;  // 过长
    }

    memcpy(str->data, src, srclen + 1);
    str->len = srclen;
    return true;
}

bool safe_string_append(safe_string_t *str, const char *src) {
    if (!str || !src) return false;

    size_t srclen = strlen(src);
    if (str->len + srclen > str->capacity) {
        return false;
    }

    memcpy(str->data + str->len, src, srclen + 1);
    str->len += srclen;
    return true;
}

// 安全sprintf替代
#include <stdarg.h>

int safe_snprintf(char *str, size_t size, const char *format, ...) {
    if (!str || size == 0 || !format) {
        return -1;
    }

    va_list args;
    va_start(args, format);
    int ret = vsnprintf(str, size, format, args);
    va_end(args);

    if (ret < 0 || (size_t)ret >= size) {
        str[size - 1] = '\0';  // 确保终止
        return -1;  // 截断或错误
    }

    return ret;
}

// 安全字符串比较
bool safe_string_equal(const char *a, const char *b) {
    if (!a || !b) return a == b;  // 都NULL才相等
    return strcmp(a, b) == 0;
}

// 时间安全比较（防侧信道攻击）
int timing_safe_compare(const void *a, const void *b, size_t len) {
    const volatile unsigned char *pa = a;
    const volatile unsigned char *pb = b;
    unsigned char result = 0;

    for (size_t i = 0; i < len; i++) {
        result |= pa[i] ^ pb[i];
    }

    return result;  // 0表示相等，非0表示不等
}
```

### 4.2 多字节字符安全

**防护代码**:

```c
#include <wchar.h>
#include <locale.h>
#include <uchar.h>

// 设置安全的locale
void setup_safe_locale(void) {
    // 使用C.UTF-8而非系统locale
    setlocale(LC_ALL, "C.UTF-8");
}

// 安全的UTF-8字符边界检查
bool is_utf8_continuation(char c) {
    return (c & 0xC0) == 0x80;
}

// 获取UTF-8字符长度（首字节）
int utf8_char_len(char first_byte) {
    unsigned char c = (unsigned char)first_byte;

    if (c < 0x80) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;

    return -1;  // 非法首字节
}

// 验证UTF-8字符串有效性
bool validate_utf8(const char *str) {
    if (!str) return false;

    for (const char *p = str; *p; ) {
        int len = utf8_char_len(*p);
        if (len < 0) return false;

        // 检查后续字节
        for (int i = 1; i < len; i++) {
            if (!p[i] || !is_utf8_continuation(p[i])) {
                return false;
            }
        }

        p += len;
    }

    return true;
}

// 安全的多字节字符串截断
bool safe_utf8_truncate(char *str, size_t max_bytes, size_t max_chars) {
    if (!str || max_bytes == 0) return false;

    size_t bytes = 0;
    size_t chars = 0;

    for (const char *p = str; *p && bytes < max_bytes - 1 && chars < max_chars; ) {
        int char_len = utf8_char_len(*p);
        if (char_len < 0) char_len = 1;  // 跳过非法字节

        // 检查是否超出边界
        if (bytes + char_len > max_bytes - 1) break;

        bytes += char_len;
        chars++;
        p += char_len;
    }

    str[bytes] = '\0';
    return true;
}

// 安全的宽字符转换
size_t safe_mbstowcs(wchar_t *dest, const char *src, size_t max) {
    if (!src || max == 0) return (size_t)-1;

    // 先验证UTF-8
    if (!validate_utf8(src)) {
        return (size_t)-1;
    }

    size_t result = mbstowcs(dest, src, max);
    if (result == (size_t)-1) {
        return (size_t)-1;
    }

    // 确保终止
    if (dest && result >= max) {
        dest[max - 1] = L'\0';
    }

    return result;
}
```

### 4.3 UTF-8安全处理

```c
/**
 * UTF-8 安全处理函数
 */

// UTF-8序列有效性验证
bool utf8_is_valid_sequence(const uint8_t *bytes, size_t len) {
    if (len == 0) return false;

    uint8_t c0 = bytes[0];

    // 1字节: 0xxxxxxx
    if (c0 < 0x80) return len == 1;

    // 2字节: 110xxxxx 10xxxxxx
    if ((c0 & 0xE0) == 0xC0) {
        if (len != 2) return false;
        if ((bytes[1] & 0xC0) != 0x80) return false;
        // 检查过度编码 (C0-C1)
        if (c0 < 0xC2) return false;
        return true;
    }

    // 3字节: 1110xxxx 10xxxxxx 10xxxxxx
    if ((c0 & 0xF0) == 0xE0) {
        if (len != 3) return false;
        if ((bytes[1] & 0xC0) != 0x80 || (bytes[2] & 0xC0) != 0x80) return false;
        // 检查过度编码 (E0 80-9F)
        if (c0 == 0xE0 && bytes[1] < 0xA0) return false;
        // 检查代理对 (ED A0-BF)
        if (c0 == 0xED && bytes[1] >= 0xA0) return false;
        return true;
    }

    // 4字节: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    if ((c0 & 0xF8) == 0xF0) {
        if (len != 4) return false;
        if ((bytes[1] & 0xC0) != 0x80 ||
            (bytes[2] & 0xC0) != 0x80 ||
            (bytes[3] & 0xC0) != 0x80) return false;
        // 检查范围 (F0 90-BF, F4 80-8F)
        if (c0 == 0xF0 && bytes[1] < 0x90) return false;
        if (c0 == 0xF4 && bytes[1] >= 0x90) return false;
        if (c0 > 0xF4) return false;
        return true;
    }

    return false;
}

// 安全的字符串长度计算（字符数）
size_t utf8_strlen_safe(const char *str, size_t max_bytes) {
    if (!str) return 0;

    size_t char_count = 0;
    size_t bytes = 0;

    while (str[bytes] && bytes < max_bytes) {
        int len = utf8_char_len(str[bytes]);
        if (len < 0) len = 1;  // 跳过非法字节

        bytes += len;
        char_count++;
    }

    return char_count;
}

// 规范化路径（防止UTF-8规范化攻击）
bool normalize_utf8_path(const char *input, char *output, size_t out_size) {
    if (!input || !output || out_size == 0) return false;

    size_t j = 0;
    for (size_t i = 0; input[i] && j < out_size - 1; ) {
        // 跳过控制字符
        if ((unsigned char)input[i] < 0x20) {
            i++;
            continue;
        }

        // 检查多字节序列有效性
        int char_len = utf8_char_len(input[i]);
        if (char_len > 0 && i + char_len <= strlen(input)) {
            if (utf8_is_valid_sequence((const uint8_t *)&input[i], char_len)) {
                // 复制有效UTF-8字符
                for (int k = 0; k < char_len && j < out_size - 1; k++) {
                    output[j++] = input[i++];
                }
                continue;
            }
        }

        // 非法字节：用替换字符或跳过
        i++;
    }

    output[j] = '\0';
    return true;
}
```

---

## 5. 并发安全

### 5.1 数据竞争防护

**威胁描述**: 多个线程无同步访问共享数据，导致未定义行为。

**防护代码**:

```c
#include <pthread.h>
#include <stdatomic.h>

// 线程安全的计数器
typedef struct {
    atomic_long count;
} atomic_counter_t;

void counter_init(atomic_counter_t *c) {
    atomic_init(&c->count, 0);
}

long counter_increment(atomic_counter_t *c) {
    return atomic_fetch_add(&c->count, 1) + 1;
}

long counter_get(atomic_counter_t *c) {
    return atomic_load(&c->count);
}

// 带读写锁的数据结构
typedef struct {
    pthread_rwlock_t lock;
    int data[1000];
    size_t size;
} thread_safe_array_t;

void ts_array_init(thread_safe_array_t *arr) {
    pthread_rwlock_init(&arr->lock, NULL);
    arr->size = 0;
}

void ts_array_destroy(thread_safe_array_t *arr) {
    pthread_rwlock_destroy(&arr->lock);
}

bool ts_array_get(thread_safe_array_t *arr, size_t index, int *value) {
    pthread_rwlock_rdlock(&arr->lock);

    bool success = false;
    if (index < arr->size) {
        *value = arr->data[index];
        success = true;
    }

    pthread_rwlock_unlock(&arr->lock);
    return success;
}

bool ts_array_set(thread_safe_array_t *arr, size_t index, int value) {
    pthread_rwlock_wrlock(&arr->lock);

    bool success = false;
    if (index < sizeof(arr->data) / sizeof(arr->data[0])) {
        arr->data[index] = value;
        if (index >= arr->size) {
            arr->size = index + 1;
        }
        success = true;
    }

    pthread_rwlock_unlock(&arr->lock);
    return success;
}
```

### 5.2 死锁避免

**防护代码**:

```c
#include <pthread.h>
#include <time.h>

// 死锁避免：统一的锁顺序
#define LOCK_ORDER_A 1
#define LOCK_ORDER_B 2
#define LOCK_ORDER_C 3

typedef struct {
    pthread_mutex_t mutex;
    int order;  // 全局顺序标识
} ordered_mutex_t;

void ordered_mutex_init(ordered_mutex_t *m, int order) {
    pthread_mutex_init(&m->mutex, NULL);
    m->order = order;
}

// 尝试获取锁，带超时避免永久阻塞
bool mutex_lock_timeout(pthread_mutex_t *mutex, int timeout_ms) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;

    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    return pthread_mutex_timedlock(mutex, &ts) == 0;
}

// 多锁获取（按顺序）
bool lock_multiple(ordered_mutex_t *locks[], size_t count, int timeout_ms) {
    // 按order排序（简单冒泡排序）
    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            if (locks[j]->order < locks[i]->order) {
                ordered_mutex_t *tmp = locks[i];
                locks[i] = locks[j];
                locks[j] = tmp;
            }
        }
    }

    // 按顺序获取
    size_t acquired = 0;
    for (size_t i = 0; i < count; i++) {
        if (!mutex_lock_timeout(&locks[i]->mutex, timeout_ms)) {
            // 获取失败，释放已获取的锁
            for (size_t j = 0; j < acquired; j++) {
                pthread_mutex_unlock(&locks[j]->mutex);
            }
            return false;
        }
        acquired++;
    }

    return true;
}

void unlock_multiple(ordered_mutex_t *locks[], size_t count) {
    // 逆序释放
    for (int i = (int)count - 1; i >= 0; i--) {
        pthread_mutex_unlock(&locks[i]->mutex);
    }
}

// 使用示例
void safe_transfer(thread_safe_array_t *from, thread_safe_array_t *to,
                   size_t index, int value) {
    ordered_mutex_t lock1 = {from->lock, LOCK_ORDER_A};
    ordered_mutex_t lock2 = {to->lock, LOCK_ORDER_B};
    ordered_mutex_t *locks[] = {&lock1, &lock2};

    if (lock_multiple(locks, 2, 1000)) {
        // 执行操作
        int val;
        if (ts_array_get(from, index, &val)) {
            // 转移数据...
        }
        unlock_multiple(locks, 2);
    }
}
```

### 5.3 原子操作使用

```c
#include <stdatomic.h>
#include <stdbool.h>

// 无锁队列节点
typedef struct node {
    void *data;
    _Atomic(struct node *) next;
} node_t;

typedef struct {
    _Atomic(node_t *) head;
    _Atomic(node_t *) tail;
} lockfree_queue_t;

void lf_queue_init(lockfree_queue_t *q) {
    node_t *dummy = malloc(sizeof(node_t));
    dummy->next = ATOMIC_VAR_INIT(NULL);
    atomic_init(&q->head, dummy);
    atomic_init(&q->tail, dummy);
}

void lf_queue_enqueue(lockfree_queue_t *q, void *data) {
    node_t *new_node = malloc(sizeof(node_t));
    new_node->data = data;
    atomic_init(&new_node->next, NULL);

    node_t *tail;
    node_t *next;

    while (1) {
        tail = atomic_load(&q->tail);
        next = atomic_load(&tail->next);

        if (tail == atomic_load(&q->tail)) {
            if (next == NULL) {
                // 尝试链接新节点
                if (atomic_compare_exchange_weak(&tail->next, &next, new_node)) {
                    break;
                }
            } else {
                // 尝试推进tail
                atomic_compare_exchange_weak(&q->tail, &tail, next);
            }
        }
    }

    // 尝试更新tail
    atomic_compare_exchange_weak(&q->tail, &tail, new_node);
}

bool lf_queue_dequeue(lockfree_queue_t *q, void **data) {
    node_t *head;
    node_t *tail;
    node_t *next;

    while (1) {
        head = atomic_load(&q->head);
        tail = atomic_load(&q->tail);
        next = atomic_load(&head->next);

        if (head == atomic_load(&q->head)) {
            if (head == tail) {
                if (next == NULL) {
                    return false;  // 空队列
                }
                atomic_compare_exchange_weak(&q->tail, &tail, next);
            } else {
                *data = next->data;
                if (atomic_compare_exchange_weak(&q->head, &head, next)) {
                    break;
                }
            }
        }
    }

    free(head);
    return true;
}

// 内存顺序选择指南
void memory_order_examples(void) {
    atomic_int x = ATOMIC_VAR_INIT(0);
    atomic_int y = ATOMIC_VAR_INIT(0);

    // memory_order_relaxed: 无同步，仅原子性
    atomic_fetch_add_explicit(&x, 1, memory_order_relaxed);

    // memory_order_acquire/release: 生产者-消费者同步
    atomic_store_explicit(&x, 1, memory_order_release);
    int val = atomic_load_explicit(&x, memory_order_acquire);

    // memory_order_seq_cst: 全局顺序（默认，最保守）
    atomic_store(&x, 1);  // 等价于 seq_cst
}
```

### 5.4 线程局部存储

```c
#include <pthread.h>

// TLS方式1: __thread (GCC/Clang)
#ifdef __GNUC__
__thread int thread_local_counter = 0;
__thread char thread_local_buffer[1024];
#endif

// TLS方式2: pthread_key（可移植）
pthread_key_t tls_key;

typedef struct {
    int request_count;
    char session_id[64];
    void *connection;
} thread_context_t;

void tls_init(void) {
    pthread_key_create(&tls_key, free);  // 析构函数自动释放
}

thread_context_t *get_thread_context(void) {
    thread_context_t *ctx = pthread_getspecific(tls_key);
    if (!ctx) {
        ctx = calloc(1, sizeof(thread_context_t));
        pthread_setspecific(tls_key, ctx);
    }
    return ctx;
}

void cleanup_thread_context(void) {
    thread_context_t *ctx = pthread_getspecific(tls_key);
    if (ctx) {
        // 清理资源
        if (ctx->connection) {
            close_connection(ctx->connection);
        }
        free(ctx);
        pthread_setspecific(tls_key, NULL);
    }
}

// 线程池任务隔离示例
typedef struct {
    void (*task_fn)(void *);
    void *arg;
} task_t;

void worker_thread(void *unused) {
    thread_context_t *ctx = get_thread_context();
    ctx->request_count = 0;

    while (1) {
        task_t *task = get_next_task();
        if (!task) break;

        // 每个任务前重置TLS
        memset(ctx->session_id, 0, sizeof(ctx->session_id));

        task->task_fn(task->arg);

        ctx->request_count++;

        // 任务后清理
        cleanup_task_data();
    }

    cleanup_thread_context();
}

// C11 _Thread_local
#if __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
#include <threads.h>

_Thread_local int c11_thread_counter = 0;
_Thread_local thread_context_t c11_context;
#endif
```

---

## 6. 密码学安全

### 6.1 随机数生成

**威胁描述**: 使用可预测的随机数会导致安全令牌、密钥被猜测。

**防护代码**:

```c
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

// 安全的随机数生成器
typedef struct {
    int fd;
    bool initialized;
} secure_rng_t;

secure_rng_t g_rng = {-1, false};

bool secure_rng_init(void) {
    if (g_rng.initialized) return true;

    // Linux/Unix: /dev/urandom（非阻塞）或 /dev/random（阻塞，更安全但慢）
    g_rng.fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
    if (g_rng.fd < 0) {
        // 回退到getrandom (Linux 3.17+)
        #ifdef SYS_getrandom
        g_rng.fd = -2;  // 标记使用getrandom
        #else
        return false;
        #endif
    }

    g_rng.initialized = true;
    return true;
}

void secure_rng_cleanup(void) {
    if (g_rng.fd >= 0) {
        close(g_rng.fd);
        g_rng.fd = -1;
    }
    g_rng.initialized = false;
}

bool secure_random_bytes(void *buf, size_t len) {
    if (!g_rng.initialized && !secure_rng_init()) {
        return false;
    }

    unsigned char *p = buf;
    size_t total = 0;

    while (total < len) {
        ssize_t n;

        #ifdef SYS_getrandom
        if (g_rng.fd == -2) {
            n = syscall(SYS_getrandom, p + total, len - total, 0);
        } else
        #endif
        {
            n = read(g_rng.fd, p + total, len - total);
        }

        if (n < 0) {
            if (errno == EINTR) continue;
            return false;
        }
        total += n;
    }

    return true;
}

// 生成安全随机整数
bool secure_random_int(int min, int max, int *result) {
    if (min >= max) return false;

    uint32_t range = (uint32_t)(max - min);
    uint32_t mask = range;
    mask |= mask >> 1;
    mask |= mask >> 2;
    mask |= mask >> 4;
    mask |= mask >> 8;
    mask |= mask >> 16;

    uint32_t value;
    do {
        if (!secure_random_bytes(&value, sizeof(value))) {
            return false;
        }
        value &= mask;
    } while (value >= range);

    *result = min + (int)value;
    return true;
}

// OpenSSL集成（生产环境推荐）
#ifdef USE_OPENSSL
#include <openssl/rand.h>
#include <openssl/err.h>

bool openssl_random_bytes(void *buf, size_t len) {
    int rc = RAND_bytes(buf, (int)len);
    if (rc != 1) {
        ERR_print_errors_fp(stderr);
        return false;
    }
    return true;
}
#endif
```

### 6.2 哈希函数使用

```c
#ifdef USE_OPENSSL
#include <openssl/evp.h>
#include <openssl/sha.h>

// 安全哈希计算
typedef enum {
    HASH_SHA256,
    HASH_SHA384,
    HASH_SHA512
} hash_type_t;

bool compute_hash(const void *data, size_t len,
                  hash_type_t type,
                  unsigned char *out, size_t *out_len) {
    const EVP_MD *md;
    switch (type) {
        case HASH_SHA256: md = EVP_sha256(); *out_len = 32; break;
        case HASH_SHA384: md = EVP_sha384(); *out_len = 48; break;
        case HASH_SHA512: md = EVP_sha512(); *out_len = 64; break;
        default: return false;
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) return false;

    bool success = EVP_DigestInit_ex(ctx, md, NULL) &&
                   EVP_DigestUpdate(ctx, data, len) &&
                   EVP_DigestFinal_ex(ctx, out, NULL);

    EVP_MD_CTX_free(ctx);
    return success;
}

// HMAC计算
bool compute_hmac(const void *key, size_t key_len,
                  const void *data, size_t data_len,
                  hash_type_t type,
                  unsigned char *out, size_t *out_len) {
    const EVP_MD *md;
    switch (type) {
        case HASH_SHA256: md = EVP_sha256(); *out_len = 32; break;
        case HASH_SHA384: md = EVP_sha384(); *out_len = 48; break;
        case HASH_SHA512: md = EVP_sha512(); *out_len = 64; break;
        default: return false;
    }

    unsigned int len;
    HMAC(md, key, (int)key_len, data, data_len, out, &len);
    *out_len = len;
    return true;
}

// PBKDF2密钥派生
bool derive_key(const char *password, const void *salt, size_t salt_len,
                int iterations, hash_type_t type,
                unsigned char *out, size_t out_len) {
    const EVP_MD *md;
    switch (type) {
        case HASH_SHA256: md = EVP_sha256(); break;
        case HASH_SHA384: md = EVP_sha384(); break;
        case HASH_SHA512: md = EVP_sha512(); break;
        default: return false;
    }

    return PKCS5_PBKDF2_HMAC(password, -1, salt, (int)salt_len,
                             iterations, md, (int)out_len, out) == 1;
}
#endif

// 简单哈希（非密码学安全，仅用于数据结构）
#include <stdint.h>

// FNV-1a哈希（快速，非加密）
uint32_t fnv1a_hash(const void *data, size_t len) {
    const uint32_t FNV_PRIME = 0x01000193;
    const uint32_t FNV_OFFSET = 0x811c9dc5;

    const unsigned char *p = data;
    uint32_t hash = FNV_OFFSET;

    for (size_t i = 0; i < len; i++) {
        hash ^= p[i];
        hash *= FNV_PRIME;
    }

    return hash;
}
```

### 6.3 密钥管理

```c
#include <pthread.h>

// 受保护的密钥存储
typedef struct {
    unsigned char key[32];
    atomic_bool locked;
    pthread_mutex_t mutex;
} protected_key_t;

// 密钥封装
bool import_key(protected_key_t *pk, const unsigned char *key_data, size_t len) {
    if (len != sizeof(pk->key)) return false;

    pthread_mutex_lock(&pk->mutex);
    memcpy(pk->key, key_data, len);
    atomic_store(&pk->locked, false);
    pthread_mutex_unlock(&pk->mutex);

    return true;
}

// 使用密钥时临时解密/解锁
bool use_key(protected_key_t *pk, bool (*callback)(const unsigned char *key, void *ctx), void *ctx) {
    pthread_mutex_lock(&pk->mutex);

    if (atomic_load(&pk->locked)) {
        pthread_mutex_unlock(&pk->mutex);
        return false;
    }

    // 执行回调
    bool result = callback(pk->key, ctx);

    pthread_mutex_unlock(&pk->mutex);
    return result;
}

// 锁定密钥（禁止使用）
void lock_key(protected_key_t *pk) {
    pthread_mutex_lock(&pk->mutex);
    SECURE_ZERO(pk->key, sizeof(pk->key));
    atomic_store(&pk->locked, true);
    pthread_mutex_unlock(&pk->mutex);
}

// 密钥派生最佳实践
#define MIN_PBKDF2_ITERATIONS 100000  // NIST推荐最少10000，建议100000+

bool secure_derive_key(const char *password,
                       const unsigned char *salt, size_t salt_len,
                       unsigned char *out, size_t out_len) {
    // 生成随机盐
    if (!salt || salt_len == 0) {
        static unsigned char random_salt[32];
        if (!secure_random_bytes(random_salt, sizeof(random_salt))) {
            return false;
        }
        salt = random_salt;
        salt_len = sizeof(random_salt);
    }

    return derive_key(password, salt, salt_len,
                      MIN_PBKDF2_ITERATIONS,
                      HASH_SHA256, out, out_len);
}
```

### 6.4 敏感数据擦除

```c
/**
 * 敏感数据安全处理
 */

// 敏感数据存储类型
typedef struct {
    unsigned char data[1024];
    size_t len;
} sensitive_buffer_t;

// 分配时清零
sensitive_buffer_t *sensitive_alloc(size_t size) {
    sensitive_buffer_t *buf = calloc(1, sizeof(sensitive_buffer_t));
    if (!buf) return NULL;

    if (size > sizeof(buf->data)) {
        free(buf);
        return NULL;
    }

    buf->len = size;
    return buf;
}

// 释放时清零
void sensitive_free(sensitive_buffer_t **buf) {
    if (!buf || !*buf) return;

    SECURE_ZERO((*buf)->data, (*buf)->len);
    (*buf)->len = 0;

    free(*buf);
    *buf = NULL;
}

// 防止核心转储
void prevent_core_dump(void) {
    #include <sys/resource.h>
    struct rlimit limit = {0, 0};
    setrlimit(RLIMIT_CORE, &limit);
}

// 锁定内存（防止交换到磁盘）
bool lock_sensitive_memory(void *addr, size_t len) {
    #ifdef __linux__
    #include <sys/mman.h>
    return mlock(addr, len) == 0;
    #else
    (void)addr; (void)len;
    return false;
    #endif
}

// 内存访问保护切换
#ifdef __linux__
#include <sys/mman.h>

typedef struct {
    void *addr;
    size_t len;
} protected_memory_t;

bool protect_memory_readonly(protected_memory_t *pm) {
    return mprotect(pm->addr, pm->len, PROT_READ) == 0;
}

bool protect_memory_readwrite(protected_memory_t *pm) {
    return mprotect(pm->addr, pm->len, PROT_READ | PROT_WRITE) == 0;
}

bool protect_memory_noaccess(protected_memory_t *pm) {
    return mprotect(pm->addr, pm->len, PROT_NONE) == 0;
}
#endif

// 完整的密码处理流程示例
bool process_password_securely(const char *user_input) {
    sensitive_buffer_t *password = sensitive_alloc(128);
    if (!password) return false;

    // 复制输入
    size_t input_len = strlen(user_input);
    if (input_len >= password->len) {
        sensitive_free(&password);
        return false;
    }
    memcpy(password->data, user_input, input_len);

    // 锁定内存
    lock_sensitive_memory(password->data, password->len);

    // 使用密码...
    // hash_password(password->data, ...);

    // 清理并释放
    sensitive_free(&password);
    return true;
}
```

---

## 7. 防御性编程

### 7.1 断言和不变式

```c
#include <assert.h>
#include <stdbool.h>

// 静态断言（编译时检查）
#define STATIC_ASSERT(expr, msg) _Static_assert((expr), msg)

// 编译时检查类型大小
STATIC_ASSERT(sizeof(size_t) >= sizeof(uint32_t), "size_t too small");
STATIC_ASSERT(CHAR_BIT == 8, "char must be 8 bits");

// 运行时断言（仅在调试构建启用）
#ifdef NDEBUG
    #define DEBUG_ASSERT(expr) ((void)0)
    #define INVARIANT(expr) ((void)0)
#else
    #define DEBUG_ASSERT(expr) assert(expr)
    #define INVARIANT(expr) do { \
        if (!(expr)) { \
            fprintf(stderr, "Invariant violated: %s at %s:%d\n", \
                    #expr, __FILE__, __LINE__); \
            abort(); \
        } \
    } while(0)
#endif

// 契约式编程
typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} vector_t;

// 前置条件
#define PRECONDITION(expr) DEBUG_ASSERT(expr)

// 后置条件
#define POSTCONDITION(expr) DEBUG_ASSERT(expr)

bool vector_push(vector_t *v, int value) {
    // 前置条件
    PRECONDITION(v != NULL);
    PRECONDITION(v->size <= v->capacity);
    PRECONDITION(v->capacity == 0 || v->data != NULL);

    size_t old_size = v->size;

    if (v->size >= v->capacity) {
        // 扩展容量...
        return false;
    }

    v->data[v->size++] = value;

    // 后置条件
    POSTCONDITION(v->size == old_size + 1);
    POSTCONDITION(v->data[v->size - 1] == value);

    return true;
}

// 类不变式
void check_vector_invariant(const vector_t *v) {
    if (v->capacity > 0) {
        INVARIANT(v->data != NULL);
        INVARIANT(v->size <= v->capacity);
    } else {
        INVARIANT(v->size == 0);
        INVARIANT(v->data == NULL);
    }
}

// 边界检查宏
#define ARRAY_BOUNDS_CHECK(arr, index, size) do { \
    DEBUG_ASSERT((index) < (size)); \
    if ((index) >= (size)) { \
        return false; /* 或处理错误 */ \
    } \
} while(0)

#define POINTER_BOUNDS_CHECK(ptr, base, size) do { \
    DEBUG_ASSERT((ptr) >= (base) && (ptr) < (base) + (size)); \
} while(0)
```

### 7.2 错误处理策略

```c
#include <stdbool.h>
#include <errno.h>

// 错误码定义
typedef enum {
    ERR_OK = 0,
    ERR_NOMEM,
    ERR_INVALID,
    ERR_OVERFLOW,
    ERR_IO,
    ERR_PERMISSION,
    ERR_NOT_FOUND,
    ERR_TIMEOUT,
    ERR_UNKNOWN
} error_code_t;

// 结果类型（类似Rust的Result）
typedef struct {
    bool ok;
    union {
        void *value;
        error_code_t error;
    };
} result_t;

#define OK(val) ((result_t){.ok = true, {.value = (val)}})
#define ERR(code) ((result_t){.ok = false, {.error = (code)}})

// 错误处理宏
#define TRY(expr) do { \
    result_t _r = (expr); \
    if (!_r.ok) return _r; \
} while(0)

#define TRY_OR(expr, fallback) do { \
    result_t _r = (expr); \
    if (!_r.ok) { fallback; } \
} while(0)

// 具体使用示例
result_t allocate_buffer(size_t size) {
    if (size == 0 || size > MAX_BUFFER_SIZE) {
        return ERR(ERR_INVALID);
    }

    void *ptr = malloc(size);
    if (!ptr) {
        return ERR(ERR_NOMEM);
    }

    return OK(ptr);
}

result_t process_data(const void *input, size_t len) {
    result_t buf_result = allocate_buffer(len);
    if (!buf_result.ok) {
        return buf_result;  // 传播错误
    }

    void *buffer = buf_result.value;

    // 处理数据...

    free(buffer);
    return OK(NULL);
}

// 链式错误处理
result_t complex_operation(void) {
    void *a = NULL, *b = NULL, *c = NULL;

    result_t ra = allocate_buffer(100);
    if (!ra.ok) goto cleanup;
    a = ra.value;

    result_t rb = allocate_buffer(200);
    if (!rb.ok) goto cleanup;
    b = rb.value;

    result_t rc = allocate_buffer(300);
    if (!rc.ok) goto cleanup;
    c = rc.value;

    // 使用a, b, c...

cleanup:
    free(c);
    free(b);
    free(a);
    return ra.ok && rb.ok && rc.ok ? OK(NULL) : ERR(ERR_NOMEM);
}

// 错误信息转换
const char *error_string(error_code_t code) {
    static const char *messages[] = {
        [ERR_OK] = "Success",
        [ERR_NOMEM] = "Out of memory",
        [ERR_INVALID] = "Invalid argument",
        [ERR_OVERFLOW] = "Integer overflow",
        [ERR_IO] = "I/O error",
        [ERR_PERMISSION] = "Permission denied",
        [ERR_NOT_FOUND] = "Not found",
        [ERR_TIMEOUT] = "Operation timeout",
        [ERR_UNKNOWN] = "Unknown error"
    };

    if (code < 0 || (size_t)code >= sizeof(messages)/sizeof(messages[0])) {
        return "Invalid error code";
    }
    return messages[code];
}
```

### 7.3 日志安全

```c
#include <time.h>
#include <stdarg.h>
#include <syslog.h>

// 日志级别
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} log_level_t;

// 敏感数据掩码
static const char *mask_sensitive(const char *input) {
    static __thread char masked[1024];

    // 简单掩码：如果包含"password", "token", "key"等，替换为***
    if (strcasestr(input, "password") ||
        strcasestr(input, "token") ||
        strcasestr(input, "secret") ||
        strcasestr(input, "key=")) {
        return "***REDACTED***";
    }

    // 复制并截断
    strncpy(masked, input, sizeof(masked) - 1);
    masked[sizeof(masked) - 1] = '\0';
    return masked;
}

// 安全日志函数
void secure_log(log_level_t level, const char *format, ...) {
    if (level < MIN_LOG_LEVEL) return;

    // 时间戳
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

    // 级别字符串
    const char *level_str[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

    // 格式化消息
    char message[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    // 输出到stderr（生产环境应使用syslog或专用日志库）
    fprintf(stderr, "[%s] [%s] %s\n", time_str, level_str[level], message);

    // 关键错误写入syslog
    if (level >= LOG_LEVEL_ERROR) {
        syslog(LOG_ERR, "%s", message);
    }
}

// 安全的错误日志（不泄露敏感信息）
#define SECURE_LOG_ERROR(msg, ...) do { \
    char sanitized[1024]; \
    snprintf(sanitized, sizeof(sanitized), __VA_ARGS__); \
    secure_log(LOG_LEVEL_ERROR, "%s: %s", (msg), mask_sensitive(sanitized)); \
} while(0)

// 审计日志（记录安全相关操作）
void audit_log(const char *action, const char *subject, bool success) {
    time_t now = time(NULL);
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", gmtime(&now));

    syslog(LOG_NOTICE, "AUDIT: time=%s action=%s subject=%s result=%s",
           time_str, action, subject, success ? "SUCCESS" : "FAILURE");
}

// 日志注入防护
bool validate_log_content(const char *content) {
    // 拒绝包含控制字符的输入
    for (const char *p = content; *p; p++) {
        if (*p < 0x20 && *p != '\t' && *p != '\n' && *p != '\r') {
            return false;
        }
        // 检查潜在的日志注入
        if (strncmp(p, "\n[AUDIT]", 8) == 0 ||
            strncmp(p, "\n[ERROR]", 8) == 0) {
            return false;
        }
    }
    return true;
}
```

### 7.4 最小权限原则

```c
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

// 降权运行
bool drop_privileges(const char *username) {
    struct passwd *pw = getpwnam(username);
    if (!pw) {
        perror("getpwnam");
        return false;
    }

    // 先初始化组
    if (initgroups(username, pw->pw_gid) != 0) {
        perror("initgroups");
        return false;
    }

    // 设置gid
    if (setgid(pw->pw_gid) != 0) {
        perror("setgid");
        return false;
    }

    // 设置uid（不可逆）
    if (setuid(pw->pw_uid) != 0) {
        perror("setuid");
        return false;
    }

    // 验证降权成功
    if (getuid() == 0 || geteuid() == 0) {
        fprintf(stderr, "Failed to drop root privileges\n");
        return false;
    }

    return true;
}

// 限制进程能力（Linux）
#ifdef __linux__
#include <sys/capability.h>

bool restrict_capabilities(void) {
    cap_t caps = cap_init();  // 空能力集
    if (!caps) return false;

    // 仅保留必要能力（示例：仅保留NET_BIND_SERVICE）
    cap_value_t keep[] = {CAP_NET_BIND_SERVICE};
    cap_set_flag(caps, CAP_PERMITTED, 1, keep, CAP_SET);
    cap_set_flag(caps, CAP_EFFECTIVE, 1, keep, CAP_SET);

    bool ok = cap_set_proc(caps) == 0;
    cap_free(caps);

    return ok;
}
#endif

// 进入chroot jail
bool enter_chroot(const char *path) {
    if (chroot(path) != 0) {
        perror("chroot");
        return false;
    }

    if (chdir("/") != 0) {
        perror("chdir");
        return false;
    }

    return true;
}

// 资源限制
bool set_resource_limits(void) {
    struct rlimit limit;

    // 限制CPU时间
    limit.rlim_cur = 60;  // 60秒
    limit.rlim_max = 120;
    setrlimit(RLIMIT_CPU, &limit);

    // 限制内存
    limit.rlim_cur = 256 * 1024 * 1024;  // 256MB
    limit.rlim_max = 512 * 1024 * 1024;
    setrlimit(RLIMIT_AS, &limit);

    // 限制打开文件数
    limit.rlim_cur = 1024;
    limit.rlim_max = 2048;
    setrlimit(RLIMIT_NOFILE, &limit);

    // 限制核心文件大小（防止敏感信息泄露）
    limit.rlim_cur = 0;
    limit.rlim_max = 0;
    setrlimit(RLIMIT_CORE, &limit);

    return true;
}

// 安全初始化流程
bool security_init(const char *run_as_user) {
    // 1. 设置资源限制
    if (!set_resource_limits()) {
        return false;
    }

    // 2. 限制能力
    #ifdef __linux__
    restrict_capabilities();
    #endif

    // 3. 降权
    if (run_as_user && !drop_privileges(run_as_user)) {
        return false;
    }

    // 4. 阻止核心转储
    prevent_core_dump();

    return true;
}
```

---

## 8. 静态分析工具

### 8.1 GCC/Clang警告选项

```makefile
# Makefile 安全编译选项

# 基础警告
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wpedantic

# 安全相关警告
CFLAGS += -Wformat=2              # 格式化字符串检查
CFLAGS += -Wformat-security       # 格式化安全
CFLAGS += -Wstack-protector       # 栈保护警告
CFLAGS += -Wstack-usage=1000000   # 栈使用警告(1MB)
CFLAGS += -Wcast-align            # 对齐警告
CFLAGS += -Wstrict-overflow=4     # 严格溢出检查
CFLAGS += -Wstrict-aliasing=2     # 严格别名
CFLAGS += -Wpointer-arith         # 指针运算警告
CFLAGS += -Wwrite-strings         # 字符串常量写入警告
CFLAGS += -Werror=format-security # 将格式化安全问题视为错误
CFLAGS += -Werror=implicit-function-declaration
CFLAGS += -Werror=int-conversion

# 安全编译选项
CFLAGS += -fstack-protector-strong    # 栈保护
CFLAGS += -fstack-clash-protection    # 栈冲突保护
CFLAGS += -D_FORTIFY_SOURCE=2         # 缓冲区检查
CFLAGS += -fPIE                       # 位置无关代码
LDFLAGS += -pie                       # PIE链接

# 更严格的选项（可能产生误报）
# CFLAGS += -Wconversion              # 类型转换警告
# CFLAGS += -Wsign-conversion         # 符号转换警告

# 调试构建
CFLAGS_DEBUG = -g3 -O0 -fsanitize=address,undefined

# 发布构建
CFLAGS_RELEASE = -O2 -DNDEBUG -fomit-frame-pointer
```

### 8.2 Clang Static Analyzer

```bash
# 安装
# macOS: 自带
# Linux: sudo apt-get install clang-tools

# 基本扫描
scan-build gcc -c source.c

# 完整项目扫描
scan-build make

# 生成HTML报告
scan-build -o report --html-title "My Project Analysis" make

# 启用所有检查器
scan-build --use-analyzer=/usr/bin/clang \
    -enable-checker alpha \
    -enable-checker security \
    -enable-checker unix \
    -enable-checker cplusplus \
    make

# 常用检查器类别
# - alpha.core          : 核心检查（实验性）
# - alpha.security      : 安全检查
# - alpha.unix          : Unix API检查
# - security            : 安全漏洞检查
# - unix                : Unix系统调用检查
```

### 8.3 Coverity Scan

```bash
# Coverity Scan 集成指南

# 1. 注册项目：https://scan.coverity.com/
# 2. 下载 cov-analysis 工具
# 3. 配置环境
export PATH=$PATH:/path/to/cov-analysis/bin

# 4. 编译项目
cov-build --dir cov-int make

# 5. 分析
cov-analyze --dir cov-int --coding-standard-config misra.cfg

# 6. 生成报告
cov-format-errors --dir cov-int --html-output html-report

# 7. 提交到Coverity Scan（开源项目免费）
# 创建tarball
tar czvf project.tgz cov-int

# 使用curl提交（需COVERITY_TOKEN）
curl --form token=$COVERITY_TOKEN \
     --form email=your@email.com \
     --form file=@project.tgz \
     --form version="1.0" \
     --form description="Description" \
     https://scan.coverity.com/builds?project=YourProject
```

### 8.4 CodeQL

```yaml
# .github/workflows/codeql-analysis.yml
name: "CodeQL"

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]
  schedule:
    - cron: '0 0 * * 0'  # 每周日

jobs:
  analyze:
    name: Analyze
    runs-on: ubuntu-latest

    strategy:
      fail-fast: false
      matrix:
        language: [ 'cpp' ]

    steps:
    - name: Checkout repository
      uses: actions/checkout@v3

    - name: Initialize CodeQL
      uses: github/codeql-action/init@v2
      with:
        languages: ${{ matrix.language }}
        queries: security-extended,security-and-quality

    - name: Autobuild
      uses: github/codeql-action/autobuild@v2

    - name: Perform CodeQL Analysis
      uses: github/codeql-action/analyze@v2
```

```bash
# 本地CodeQL CLI使用

# 1. 安装CodeQL CLI
# https://github.com/github/codeql-cli-binaries/releases

# 2. 创建数据库
codeql database create cpp-database \
    --language=cpp \
    --source-root=. \
    --command="make"

# 3. 运行分析
codeql database analyze cpp-database \
    /path/to/codeql-repo/cpp/ql/src/Security \
    --format=sarifv2.1.0 \
    --output=security-results.sarif

# 4. 查看结果
codeql bqrs decode security-results.bqrs
```

---

## 9. 运行时检测

### 9.1 AddressSanitizer (ASan)

```makefile
# ASan 编译选项

# 基本ASan
CFLAGS_ASAN = -fsanitize=address -g

# 完整ASan（含泄漏检测）
CFLAGS_ASAN_FULL = -fsanitize=address,leak \
                   -fno-omit-frame-pointer \
                   -g

# 运行时选项
# ASAN_OPTIONS=detect_leaks=1:halt_on_error=0
```

```bash
# 使用示例

# 编译
gcc -fsanitize=address -g -o myprogram myprogram.c

# 运行（自动检测）
./myprogram

# 常用ASAN_OPTIONS
export ASAN_OPTIONS="
    detect_leaks=1              # 启用泄漏检测
    halt_on_error=0             # 继续运行发现多个错误
    print_stats=1               # 打印统计信息
    log_path=asan.log           # 日志文件
    detect_stack_use_after_return=1  # 检测返回后使用
"

# 抑制已知问题
# 创建 asan.supp 文件
# leak:some_known_leak
# memory:some_known_issue
export ASAN_OPTIONS="$ASAN_OPTIONS:suppressions=asan.supp"
```

### 9.2 MemorySanitizer (MSan)

```makefile
# MSan 编译选项（仅Clang）
CFLAGS_MSAN = -fsanitize=memory \
              -fno-omit-frame-pointer \
              -g \
              -O2

# 注意：MSan需要所有库也用MSan编译，通常用于测试
```

```bash
# 使用示例（需要干净环境）

# 使用Docker容器运行MSan
# 或使用Clang的libc++ MSan版本

# 编译
clang -fsanitize=memory -fno-omit-frame-pointer -g -o myprogram myprogram.c

# 运行
./myprogram

# MSan检测：
# - 使用未初始化内存
# - 未初始化值影响控制流
# - 传递给系统调用
```

### 9.3 UndefinedBehaviorSanitizer (UBSan)

```makefile
# UBSan 编译选项

# 基本UBSan
CFLAGS_UBSAN = -fsanitize=undefined \
               -fno-omit-frame-pointer \
               -g

# 特定检查
CFLAGS_UBSAN += -fsanitize=signed-integer-overflow
CFLAGS_UBSAN += -fsanitize=unsigned-integer-overflow
CFLAGS_UBSAN += -fsanitize=shift
CFLAGS_UBSAN += -fsanitize=alignment
CFLAGS_UBSAN += -fsanitize=bool
CFLAGS_UBSAN += -fsanitize=enum
CFLAGS_UBSAN += -fsanitize=pointer-overflow
CFLAGS_UBSAN += -fsanitize=null
CFLAGS_UBSAN += -fsanitize=return
CFLAGS_UBSAN += -fsanitize=bounds

# 继续执行模式
CFLAGS_UBSAN += -fsanitize-recover=all
```

```bash
# 使用示例

# 编译
gcc -fsanitize=undefined -g -o myprogram myprogram.c

# 运行时选项
export UBSAN_OPTIONS="
    print_stacktrace=1
    halt_on_error=0
    report_error_type=1
    log_path=ubsan.log
"

# 常用检测项：
# - 整数溢出
# - 数组越界
# - 空指针解引用
# - 类型对齐错误
# - 无效位移
# - 返回值检查失败
```

### 9.4 Control-Flow Integrity (CFI)

```makefile
# CFI 编译选项（Clang）

CFLAGS_CFI = -fsanitize=cfi \
             -fvisibility=hidden \
             -flto \
             -g

# 仅检查虚函数调用
CFLAGS_CFI_VCALL = -fsanitize=cfi-vcall \
                   -fvisibility=hidden \
                   -flto

# 诊断模式（不终止）
CFLAGS_CFI += -fsanitize-recover=cfi
```

```bash
# 使用示例

# 注意：CFI需要链接时优化(LTO)
clang -fsanitize=cfi -fvisibility=hidden -flto -c source.c
clang -fsanitize=cfi -flto source.o -o program

# 检测的攻击：
# - 虚表劫持
# - 返回导向编程(ROP)
# - 间接跳转劫持
```

### 9.5 综合运行时检测脚本

```bash
#!/bin/bash
# run-security-tests.sh

set -e

SOURCE=$1
PROGRAM="test_program"

echo "=== 构建不同检测版本 ==="

# ASan版本
echo "Building ASan version..."
gcc -fsanitize=address -g -O1 -o ${PROGRAM}_asan ${SOURCE}

# UBSan版本
echo "Building UBSan version..."
gcc -fsanitize=undefined -g -O2 -o ${PROGRAM}_ubsan ${SOURCE}

# 普通版本（用于valgrind）
echo "Building normal version..."
gcc -g -O0 -o ${PROGRAM}_normal ${SOURCE}

echo "=== 运行测试 ==="

# ASan测试
echo "--- AddressSanitizer ---"
ASAN_OPTIONS="detect_leaks=1:halt_on_error=0" \
    ./${PROGRAM}_asan || true

# UBSan测试
echo "--- UndefinedBehaviorSanitizer ---"
UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=0" \
    ./${PROGRAM}_ubsan || true

# Valgrind测试（如果可用）
if command -v valgrind &> /dev/null; then
    echo "--- Valgrind Memcheck ---"
    valgrind --leak-check=full --show-leak-kinds=all \
        --track-origins=yes \
        ./${PROGRAM}_normal || true
fi

# 清理
rm -f ${PROGRAM}_asan ${PROGRAM}_ubsan ${PROGRAM}_normal

echo "=== 测试完成 ==="
```

---

## 10. 安全开发生命周期

### 10.1 安全设计审查

```
┌─────────────────────────────────────────────────────────────────────┐
│                     安全设计审查检查清单                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│ □ 威胁建模                                                          │
│   - 已完成STRIDE威胁分析                                            │
│   - 已识别所有信任边界                                              │
│   - 已定义攻击面                                                    │
│                                                                     │
│ □ 输入验证                                                          │
│   - 所有外部输入都有验证                                            │
│   - 验证在服务端完成                                                │
│   - 白名单验证优先于黑名单                                          │
│                                                                     │
│ □ 认证与授权                                                        │
│   - 强密码策略                                                      │
│   - 多因素认证支持                                                  │
│   - 最小权限原则                                                    │
│   - 会话管理安全                                                    │
│                                                                     │
│ □ 数据保护                                                          │
│   - 敏感数据加密存储                                                │
│   - 传输层加密(TLS)                                                 │
│   - 密钥管理方案                                                    │
│   - 数据分类分级                                                    │
│                                                                     │
│ □ 错误处理                                                          │
│   - 不泄露敏感信息                                                  │
│   - 统一错误处理                                                    │
│   - 安全日志记录                                                    │
│                                                                     │
│ □ 审计与监控                                                        │
│   - 安全事件日志                                                    │
│   - 异常检测机制                                                    │
│   - 告警响应流程                                                    │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 10.2 代码审计检查清单

```c
/**
 * C代码安全审计检查清单
 *
 * 复制此清单到每个源文件进行自查
 */

/*
 * □ 内存管理
 *   - 所有malloc都有对应的free
 *   - 已检查分配失败
 *   - 无双重释放
 *   - 无使用-after-free
 *   - 敏感数据已用memset_s清零
 *
 * □ 缓冲区
 *   - 所有字符串操作使用strlcpy/strlcat
 *   - 数组访问有边界检查
 *   - 整数运算检查溢出
 *
 * □ 输入处理
 *   - 外部输入经过验证
 *   - 格式化字符串使用"%s", 而非直接传入
 *   - 无命令注入风险
 *
 * □ 并发
 *   - 共享数据有适当同步
 *   - 无死锁风险
 *   - 原子操作使用正确
 *
 * □ 错误处理
 *   - 所有错误路径正确处理
 *   - 资源在错误时释放
 *   - 不泄露系统信息
 *
 * □ 密码学
 *   - 使用加密安全随机数
 *   - 密钥派生使用PBKDF2/Argon2
 *   - 敏感内存已锁定
 */
```

### 10.3 渗透测试指南

```bash
#!/bin/bash
# pentest-c-program.sh
# C程序渗透测试脚本

echo "=== C程序安全测试套件 ==="

TARGET=$1
if [ -z "$TARGET" ]; then
    echo "Usage: $0 <target_binary>"
    exit 1
fi

# 1. 基本信息收集
echo "--- 1. 基本信息 ---"
file $TARGET
readelf -h $TARGET 2>/dev/null || otool -h $TARGET 2>/dev/null
checksec --file=$TARGET 2>/dev/null || echo "checksec not installed"

# 2. 字符串分析
echo "--- 2. 字符串分析 ---"
strings -n 8 $TARGET | head -50

# 3. 模糊测试准备
echo "--- 3. 模糊测试 ---"
mkdir -p fuzz_in fuzz_out
echo "test" > fuzz_in/seed1

# 使用AFL++进行模糊测试（示例）
# afl-fuzz -i fuzz_in -o fuzz_out -- $TARGET @@

# 4. 动态分析
echo "--- 4. 动态分析 ---"

# 输入模糊测试
for i in {1..100}; do
    head -c $((RANDOM % 1024)) /dev/urandom | $TARGET 2>/dev/null
done

# 长输入测试
python3 -c "print('A'*10000)" | $TARGET 2>/dev/null

# 格式化字符串测试
echo "%s%s%s%s%s" | $TARGET 2>/dev/null
echo "%n%n%n%n" | $TARGET 2>/dev/null
echo "%p %p %p %p" | $TARGET 2>/dev/null

# 特殊字符测试
printf '\x00\x01\x02\x03' | $TARGET 2>/dev/null
printf '\xff\xfe\xfd\xfc' | $TARGET 2>/dev/null

echo "=== 测试完成 ==="
```

### 10.4 漏洞响应流程

```
┌─────────────────────────────────────────────────────────────────────┐
│                      漏洞响应流程                                    │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  1. 接收报告                                                        │
│     □ 建立安全邮箱 (security@example.com)                          │
│     □ 48小时内确认收到                                              │
│     □ 分配跟踪ID                                                    │
│                                                                     │
│  2. 评估与确认                                                      │
│     □ 复现漏洞                                                      │
│     □ 评估影响范围                                                  │
│     □ CVSS评分                                                      │
│     □ 确定修复优先级                                                │
│                                                                     │
│  3. 修复开发                                                        │
│     □ 创建私有修复分支                                              │
│     □ 代码审查                                                      │
│     □ 安全测试                                                      │
│     □ 回归测试                                                      │
│                                                                     │
│  4. 协调披露                                                        │
│     □ 通知受影响用户（如需要）                                      │
│     □ 准备安全公告                                                  │
│     □ CVE申请（如适用）                                             │
│     □ 设定披露日期                                                  │
│                                                                     │
│  5. 发布修复                                                        │
│     □ 合并修复到主分支                                              │
│     □ 发布安全版本                                                  │
│     □ 更新安全公告                                                  │
│     □ 公开披露（90天后）                                            │
│                                                                     │
│  6. 事后分析                                                        │
│     □ 根因分析                                                      │
│     □ 流程改进                                                      │
│     □ 更新安全开发指南                                              │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 附录A: 安全编码速查表

### 危险函数与替代方案

| 危险函数 | 安全问题 | 安全替代 |
|---------|---------|---------|
| `gets()` | 缓冲区溢出 | `fgets()`, `getline()` |
| `strcpy()` | 无长度检查 | `strlcpy()`, `strncpy_s()` |
| `strcat()` | 无长度检查 | `strlcat()`, `strncat_s()` |
| `sprintf()` | 缓冲区溢出 | `snprintf()` |
| `vsprintf()` | 缓冲区溢出 | `vsnprintf()` |
| `scanf()` | 缓冲区溢出 | 使用宽度限制 `%10s` |
| `memcpy()` | 重叠内存问题 | `memmove()` |
| `system()` | 命令注入 | `execve()` 系列 |
| `rand()` | 可预测 | `arc4random()`, `/dev/urandom` |
| `strlen()` | 无终止检查 | `strnlen()` |
| `strncpy()` | 不保证终止 | `strlcpy()` |

### 编译安全选项速查

```bash
# 最安全的GCC编译选项
gcc \
    -O2 -D_FORTIFY_SOURCE=3 \
    -fstack-protector-strong \
    -fstack-clash-protection \
    -fPIE -pie \
    -Wformat -Wformat-security \
    -Werror=format-security \
    -Wl,-z,relro,-z,now \
    -ftrapv \
    source.c -o program
```

---

## 附录B: 参考资源

### 标准与规范

- [C11 Standard (ISO/IEC 9899:2011)](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1548.pdf)
- [SEI CERT C Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c/SEI+CERT+C+Coding+Standard)
- [MISRA C:2012](https://www.misra.org.uk/)
- [OWASP C Secure Coding](https://owasp.org/www-project-c-secure-coding/)

### 工具资源

- [AFL++](https://github.com/AFLplusplus/AFLplusplus) - 模糊测试
- [Valgrind](https://valgrind.org/) - 内存调试
- [Clang Static Analyzer](https://clang-analyzer.llvm.org/)
- [CodeQL](https://codeql.github.com/)
- [Coverity Scan](https://scan.coverity.com/)

### 安全库

- [libsodium](https://libsodium.gitbook.io/doc/) - 现代加密库
- [OpenSSL](https://www.openssl.org/) - TLS/加密
- [libbsd](https://libbsd.freedesktop.org/) - strlcpy等BSD函数

---

*本文档遵循知识共享署名-相同方式共享 4.0 国际许可协议发布*


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
