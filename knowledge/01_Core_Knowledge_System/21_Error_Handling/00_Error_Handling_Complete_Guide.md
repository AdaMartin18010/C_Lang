# C 语言错误处理完全指南

> **难度**: L2-L4
> **对应标准**: ISO C §7.5 (errno), §7.21.6.2 (strerror), POSIX
> **核心目标**: 掌握从返回值检查到结构化异常处理的完整技术栈

---

## 1. C 错误处理的设计哲学

### 1.1 没有异常机制

与 C++、Java、Rust 不同，C 语言**没有内置异常机制**。错误处理完全依赖：

1. **函数返回值** — 最常用
2. **全局状态变量** (`errno`) — 系统调用标准
3. **`setjmp`/`longjmp`** — 非局部跳转
4. **回调错误处理器** — 事件驱动架构

```c
// C 的错误处理是显式的、局部的、程序员负责的
FILE *fp = fopen("data.txt", "r");
if (fp == NULL) {           // ← 必须显式检查
    perror("fopen");        // ← 手动处理错误
    return -1;
}
```

### 1.2 错误处理原则

```
┌─────────────────────────────────────────────────────────────┐
│                    C 错误处理铁律                              │
├─────────────────────────────────────────────────────────────┤
│  1. 每个可能失败的调用都必须检查返回值                         │
│  2. 错误处理路径必须释放已分配资源                             │
│  3. 错误信息必须包含上下文（哪个函数、为什么失败）              │
│  4. 不要忽略错误（即使你认为"不可能发生"）                     │
│  5. 向上传播错误时，保持错误链的完整性                         │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. 返回值模式

### 2.1 标准库返回值约定

| 函数族 | 成功返回值 | 错误返回值 | 错误详情 |
|:-------|:-----------|:-----------|:---------|
| `malloc` | 非 NULL 指针 | `NULL` | `errno = ENOMEM` |
| `fopen` | `FILE*` | `NULL` | `errno` |
| `fread` | 读取元素数 | `< 请求数` | `feof()` / `ferror()` |
| `strtol` | 转换值 | `0` 或 `LONG_MAX/MIN` | `errno = ERANGE` |
| `pthread_*` | `0` | 错误码 (正数) | 直接返回错误码 |
| `snprintf` | 写入字符数 | `< 0` 或 `>= size` | 编码/截断错误 |

### 2.2 自定义错误码设计

```c
// 统一错误码（推荐）
typedef enum {
    ERR_OK = 0,           // 必须有 0 = 成功
    ERR_NULL_PTR = -1,
    ERR_INVALID_ARG = -2,
    ERR_OOM = -3,
    ERR_IO = -4,
    ERR_NOT_FOUND = -5,
    ERR_OVERFLOW = -6,
    ERR_PERMISSION = -7,
    ERR_TIMEOUT = -8,
    ERR_INTERNAL = -99,
} ErrorCode;

// 错误码到字符串的映射（必须保持同步）
const char *error_string(ErrorCode code) {
    switch (code) {
        case ERR_OK:           return "success";
        case ERR_NULL_PTR:     return "null pointer";
        case ERR_INVALID_ARG:  return "invalid argument";
        case ERR_OOM:          return "out of memory";
        case ERR_IO:           return "I/O error";
        case ERR_NOT_FOUND:    return "not found";
        case ERR_OVERFLOW:     return "overflow";
        case ERR_PERMISSION:   return "permission denied";
        case ERR_TIMEOUT:      return "timeout";
        case ERR_INTERNAL:     return "internal error";
        default:               return "unknown error";
    }
}
```

### 2.3 多返回值模式（C23 `stdckdint.h`）

```c
#include <stdckdint.h>  // C23

// ❌ 旧方式：通过返回值返回，错误码通过指针
int parse_int_old(const char *s, int *value);

// ✅ C23 新方式：显式检测溢出
int value;
if (ckd_add(&value, a, b)) {  // 如果溢出
    // 处理溢出错误
}

// 或手动实现（C17及以下）
bool safe_add(int a, int b, int *result) {
    if (b > 0 && a > INT_MAX - b) return false;  // 正溢出
    if (b < 0 && a < INT_MIN - b) return false;  // 负溢出
    *result = a + b;
    return true;
}
```

---

## 3. 错误传播模式

### 3.1 嵌套错误检查（金字塔）

```c
// ❌ 反模式：深层嵌套，难以阅读
int process_file_bad(const char *path) {
    FILE *fp = fopen(path, "r");
    if (fp != NULL) {
        char *buf = malloc(1024);
        if (buf != NULL) {
            if (fread(buf, 1, 1024, fp) > 0) {
                if (process_buffer(buf)) {
                    free(buf);
                    fclose(fp);
                    return 0;
                }
            }
            free(buf);
        }
        fclose(fp);
    }
    return -1;
}

// ✅ 正确：提前返回（guard clause）
int process_file_good(const char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        log_error("fopen failed: %s", path);
        return ERR_IO;
    }

    char *buf = malloc(1024);
    if (buf == NULL) {
        fclose(fp);
        return ERR_OOM;
    }

    size_t n = fread(buf, 1, 1024, fp);
    if (n == 0 && ferror(fp)) {
        free(buf);
        fclose(fp);
        return ERR_IO;
    }

    ErrorCode err = process_buffer(buf, n);

    free(buf);
    fclose(fp);
    return err;
}
```

### 3.2 使用 `goto` 进行清理（C 语言惯用法）

```c
// ✅ 在 C 中，goto 用于错误清理是公认的好的实践
int process_file_goto(const char *path) {
    FILE *fp = NULL;
    char *buf = NULL;
    ErrorCode result = ERR_OK;

    fp = fopen(path, "r");
    if (fp == NULL) {
        result = ERR_IO;
        goto cleanup;
    }

    buf = malloc(1024);
    if (buf == NULL) {
        result = ERR_OOM;
        goto cleanup;
    }

    size_t n = fread(buf, 1, 1024, fp);
    if (n == 0 && ferror(fp)) {
        result = ERR_IO;
        goto cleanup;
    }

    result = process_buffer(buf, n);

cleanup:
    free(buf);       // free(NULL) 是安全的
    if (fp) fclose(fp);
    return result;
}
```

### 3.3 宏辅助的错误传播

```c
// 简化重复的错误检查
#define TRY(call) do { \
    ErrorCode _err = (call); \
    if (_err != ERR_OK) return _err; \
} while(0)

#define TRY_PTR(ptr) do { \
    if ((ptr) == NULL) return ERR_NULL_PTR; \
} while(0)

// 使用
ErrorCode process_pipeline(const char *input, const char *output) {
    TRY_PTR(input);
    TRY_PTR(output);

    Data *data = data_parse(input);
    TRY_PTR(data);

    TRY(data_validate(data));
    TRY(data_transform(data));
    TRY(data_save(data, output));

    data_free(data);
    return ERR_OK;
}
```

---

## 4. `errno` 与系统错误

### 4.1 `errno` 的正确使用

```c
#include <errno.h>
#include <string.h>

// ❌ 错误：在失败调用之间读取 errno
FILE *fp = fopen("file.txt", "r");
int saved = errno;  // 如果 fopen 成功，errno 值未定义！

// ✅ 正确：仅在确认失败后读取 errno
FILE *fp = fopen("file.txt", "r");
if (fp == NULL) {
    int saved_errno = errno;  // 立即保存
    // 现在可以安全调用其他函数（如日志）
    log_error("fopen failed: %s", strerror(saved_errno));
    return ERR_IO;
}
```

### 4.2 线程安全

```c
// C11 之前：errno 可能是线程局部变量（取决于实现）
// C11 之后：errno 是线程局部变量（保证）

// 使用 strerror_r (POSIX) 或 strerror_s (C11 Annex K)
char buf[256];
#if defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L)
    strerror_r(saved_errno, buf, sizeof(buf));
#else
    strerror_s(buf, sizeof(buf), saved_errno);
#endif
```

---

## 5. 资源管理：RAII 模式

### 5.1 C 语言的 RAII 模拟

```c
// 使用 __attribute__((cleanup)) (GCC/Clang)
static inline void autofree(void *p) {
    free(*(void **)p);
}

static inline void autoclose(FILE **pp) {
    if (*pp) {
        fclose(*pp);
        *pp = NULL;
    }
}

#define AUTO __attribute__((cleanup(autofree)))
#define AUTOCLOSE __attribute__((cleanup(autoclose)))

// 使用
void process_with_raii(void) {
    AUTO char *buf = malloc(1024);    // 函数退出时自动 free
    AUTOCLOSE FILE *fp = fopen("data", "r");  // 自动 fclose

    if (!fp) return;  // buf 和 fp 都会被自动清理

    fread(buf, 1, 1024, fp);
    // 正常退出也会自动清理
}
```

### 5.2 作用域守卫宏

```c
#define DEFER_IMPL(count) \
    void _defer_##count(void (*_defer_fn_##count)(void*), void *_defer_arg_##count); \
    __attribute__((cleanup(_defer_##count))) void *_defer_tag_##count = NULL; \
    void _defer_##count(void (*fn)(void*), void *arg) { if(fn) fn(arg); }

// 或使用 GNU C 嵌套函数（GCC 扩展）
#define WITH_FILE(name, path, mode) \
    for (FILE *name = fopen(path, mode); name; fclose(name), name = NULL)

// 使用
WITH_FILE(fp, "data.txt", "r") {
    if (!fp) break;
    char buf[1024];
    while (fgets(buf, sizeof(buf), fp)) {
        process_line(buf);
    }
}  // fp 自动关闭
```

---

## 6. `setjmp`/`longjmp`：非局部跳转

### 6.1 异常模拟

```c
#include <setjmp.h>

jmp_buf g_env;

// 模拟 try/catch
#define TRY if (setjmp(g_env) == 0)
#define CATCH else
#define THROW() longjmp(g_env, 1)

void risky_operation(void) {
    if (some_error_condition()) {
        THROW();
    }
}

// 使用
TRY {
    risky_operation();
    another_risky_operation();
} CATCH {
    printf("An error occurred\n");
}
```

### 6.2 限制与风险

```
⚠️ setjmp/longjmp 的陷阱：
1. 不会调用局部变量的析构函数（C 没有析构函数，但会跳过 cleanup 属性）
2. 跳过后续代码，可能导致资源泄漏
3. 可移植性问题：jmp_buf 大小因平台而异
4. 只能向上跳转（不能用于协程）

结论：仅在极端场景使用（如信号恢复），常规错误处理避免使用。
```

---

## 7. 错误处理策略矩阵

| 场景 | 推荐模式 | 示例 |
|:-----|:---------|:-----|
| 简单函数 | 返回错误码 | `ErrorCode do_something(void)` |
| 需要返回数据 | 输出参数 + 返回错误码 | `ErrorCode get_data(int *out)` |
| 分配资源 | goto cleanup | 文件/内存操作 |
| 深调用链 | 宏辅助传播 (`TRY`) | 业务逻辑层 |
| 不可恢复错误 | `abort()` / `exit()` | 内存损坏检测 |
| 异步操作 | 回调错误处理器 | 事件循环 |
| 库 API | 一致的错误码约定 | 所有公共函数 |

---

## 8. 检查清单

### 函数设计

- [ ] 所有可能失败的函数都有明确的错误返回值
- [ ] 错误码枚举包含 `OK = 0`
- [ ] 提供 `error_string()` 转换函数
- [ ] 文档说明每个错误条件的含义

### 调用方

- [ ] 每个返回值都被检查或使用 `(void)` 显式忽略
- [ ] 错误处理路径释放所有已分配资源
- [ ] 错误日志包含足够的上下文信息
- [ ] 不假设"这个调用不会失败"

---

## 9. 配套代码示例

本节配套可编译代码位于 `examples/error_handling/` 目录：

| 示例文件 | 演示内容 | 编译命令 |
|---------|---------|---------|
| `error_code_system.c` | 自定义错误码系统 + 安全检查函数 | `gcc -O2 -std=c11` |
| `goto_cleanup.c` | `goto cleanup` 统一释放多资源模式 | `gcc -O2 -std=c11` |
| `raii_simulation.c` | GCC `__attribute__((cleanup))` 模拟 RAII | `gcc -O2 -std=c11` |
| `setjmp_example.c` | `setjmp/longjmp` 模拟 TRY/CATCH/THROW | `gcc -O2 -std=c11` |

### 各模式适用场景对比

| 模式 | 适用场景 | 资源安全 | 性能开销 | 可移植性 |
|------|---------|---------|---------|---------|
| 错误码返回 | 简单函数、库 API | ⚠️ 需调用方处理 | 无 | 最高 |
| `goto cleanup` | 多资源分配函数 | ✅ 统一释放 | 无 | 高 |
| `cleanup` 属性 | GCC/Clang 项目 | ✅ 自动释放 | 无 | 低（编译器绑定） |
| `setjmp/longjmp` | 深调用链异常传播 | ❌ 可能泄漏 | 中 | 中 |

---

## 10. 常见错误模式

| 错误模式 | 后果 | 修复方案 |
|---------|------|---------|
| **忽略返回值** | 错误静默传播 | 使用 `__attribute__((warn_unused_result))` |
| **部分资源泄漏** | `goto` 跳过时部分分配的资源未释放 | 每个分配后立刻检查错误 |
| **错误码吞没** | 内部错误被覆盖，丢失根因 | 使用链式错误码或结构化日志 |
| `errno` 未立即保存 | 后续函数调用覆盖 errno | 出错后立即 `int saved = errno` |
| **双重释放** | 未定义行为 / 崩溃 | `free(ptr); ptr = NULL;` |
| **清理顺序错误** | 依赖已释放的资源 | 按分配逆序释放 |

---

> **最后更新**: 2026-05-11
> **参考**: CERT C ERR00-ERR99, Linux Kernel Error Handling Patterns
