# C 语言库设计指南

> **难度**: L3-L5
> **核心目标**: 设计稳定、可维护、跨版本的 C 库接口

---

## 1. ABI 稳定性原则

### 1.1 什么是 ABI

ABI（Application Binary Interface）定义了编译后的二进制兼容性：

```
┌─────────────────────────────────────────────────────────────┐
│                     ABI 包含的内容                           │
├─────────────────────────────────────────────────────────────┤
│  • 数据类型的大小和对齐方式                                    │
│  • 函数调用约定（参数传递、返回值）                            │
│  • 结构体布局（成员顺序、填充）                                │
│  • 符号命名（name mangling，C 中通常无）                       │
│  • 虚表布局（C++，C 中通过函数指针表模拟）                     │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 ABI 破坏与保持

```c
// ✅ ABI 兼容：添加新函数
// v1.0
void lib_init(void);
void lib_process(int x);

// v1.1 — 安全：只添加新函数
void lib_init(void);
void lib_process(int x);
void lib_process_fast(int x);  // 新函数

// ❌ ABI 破坏：改变现有函数签名
// v1.0
void lib_process(int x);

// v2.0 — 破坏：改变参数类型
void lib_process(long x);  // 链接失败！

// ❌ ABI 破坏：改变结构体布局
typedef struct {
    int x;      // v1.0
} Config;

// v2.0 — 破坏：添加成员到中间
typedef struct {
    int x;
    int new_y;  // 已有代码的偏移量变了！
} Config;
```

---

## 2. 版本控制策略

### 2.1 语义化版本与 ABI

| 版本变更 | 允许的操作 | 示例 |
|:---------|:-----------|:-----|
| **Major** (x.0.0) | 任何不兼容修改 | 删除函数、改变结构体布局 |
| **Minor** (1.x.0) | 添加新功能（保持ABI） | 新函数、新结构体、新枚举值 |
| **Patch** (1.0.x) | Bug修复（不改变ABI） | 内部算法优化、错误处理修复 |

### 2.2 符号版本控制（Linux）

```c
// libexample.map
LIBEXAMPLE_1.0 {
    global:
        lib_init;
        lib_process;
    local:
        *;
};

LIBEXAMPLE_1.1 {
    global:
        lib_process_fast;
} LIBEXAMPLE_1.0;

// 编译时链接版本脚本
gcc -shared -Wl,--version-script,libexample.map -o libexample.so
```

### 2.3 Windows DLL 导出控制

```c
// 统一跨平台的导出宏
#ifdef _WIN32
    #ifdef MYLIB_EXPORTS
        #define MYLIB_API __declspec(dllexport)
    #else
        #define MYLIB_API __declspec(dllimport)
    #endif
#else
    #define MYLIB_API __attribute__((visibility("default")))
#endif

// 只导出公共API
MYLIB_API void lib_init(void);
MYLIB_API int  lib_process(int x);

// 内部函数不导出
void internal_helper(void);  // 不暴露
```

---

## 3. 不透明类型（Opaque Types）

### 3.1 完全隐藏实现

```c
// mylib.h — 只暴露接口，不暴露结构体定义
typedef struct MyContext MyContext;

MyContext *context_create(void);
void       context_destroy(MyContext *ctx);
int        context_do_work(MyContext *ctx, int input);

// mylib.c — 实现细节完全隐藏
struct MyContext {
    int state;
    void *internal_buffer;
    // ... 可以随时修改，不影响ABI
};
```

### 3.2 前向声明的局限

```c
// ❌ 错误：用户不知道大小，无法栈分配
MyContext ctx;        // 编译错误：不完整类型
MyContext ctx[10];    // 编译错误

// ✅ 正确：必须通过API分配
MyContext *ctx = context_create();

// 如果需要栈分配，提供固定大小的包装
typedef struct {
    char opaque[64];  // 足够大的缓冲区（版本间可能变化！）
} MyContextStorage;

MyContext *context_init_inplace(MyContextStorage *storage);
```

---

## 4. 错误处理与日志

### 4.1 库的错误报告策略

```c
// 策略1: 返回错误码（推荐）
typedef enum {
    MYLIB_OK = 0,
    MYLIB_ERR_INVALID_ARG = -1,
    MYLIB_ERR_OOM = -2,
    MYLIB_ERR_IO = -3,
} MyLibError;

MyLibError mylib_open(const char *path, MyLibHandle **out);

// 策略2: 线程局部错误状态（类似 errno）
const char *mylib_last_error(void);
int mylib_last_error_code(void);

// 策略3: 回调错误处理器（适合异步库）
typedef void (*MyLibErrorCallback)(int code, const char *msg, void *user_data);
void mylib_set_error_callback(MyLibErrorCallback cb, void *user_data);
```

### 4.2 日志级别控制

```c
// 不强制使用特定日志系统，提供回调
typedef enum {
    MYLIB_LOG_DEBUG,
    MYLIB_LOG_INFO,
    MYLIB_LOG_WARN,
    MYLIB_LOG_ERROR,
} MyLibLogLevel;

typedef void (*MyLibLogCallback)(MyLibLogLevel level, const char *file,
                                  int line, const char *fmt, va_list args);

void mylib_set_log_callback(MyLibLogCallback cb);
```

---

## 5. 配置与扩展点

### 5.1 运行时配置

```c
// 使用配置结构体而非大量参数
typedef struct {
    int buffer_size;
    int max_connections;
    int timeout_ms;
    int flags;
} MyLibConfig;

#define MYLIB_DEFAULT_CONFIG { \
    .buffer_size = 4096, \
    .max_connections = 100, \
    .timeout_ms = 5000, \
    .flags = 0 \
}

MyContext *context_create_with_config(const MyLibConfig *config);

// 使用
MyLibConfig cfg = MYLIB_DEFAULT_CONFIG;
cfg.buffer_size = 8192;  // 只修改需要的字段
MyContext *ctx = context_create_with_config(&cfg);
```

### 5.2 插件/扩展机制

```c
// 插件接口
typedef struct {
    int version;  // 插件API版本
    const char *name;
    int (*init)(void *ctx);
    int (*process)(void *ctx, const void *input, void *output);
    void (*cleanup)(void *ctx);
} MyLibPlugin;

int mylib_register_plugin(MyContext *ctx, const MyLibPlugin *plugin);
```

---

## 6. 线程安全设计

### 6.1 三种线程安全级别

| 级别 | 说明 | 示例 |
|:-----|:-----|:-----|
| **线程安全** | 多线程可同时调用 | 纯函数、只读操作 |
| **线程兼容** | 调用方负责同步 | 大多数 mutable API |
| **线程不安全** | 必须在单线程使用 | 全局状态、非重入函数 |

```c
// 明确标注线程安全属性
// 线程安全：不可变操作
int mylib_version_major(void);

// 线程兼容：需要外部同步（或每个线程一个上下文）
int mylib_context_process(MyContext *ctx, ...);

// 线程不安全：全局状态
int mylib_global_init(void);  // 只能调用一次
```

---

## 7. 检查清单

- [ ] 公共头文件只暴露必要的符号
- [ ] 使用不透明类型隐藏实现细节
- [ ] 所有函数有明确的错误返回值
- [ ] 不依赖全局状态（上下文参数传递）
- [ ] 提供默认配置，允许运行时自定义
- [ ] 文档说明每个函数的线程安全级别
- [ ] Minor版本只添加，不改变现有ABI
- [ ] 提供版本查询API（编译期和运行期）

---

> **最后更新**: 2026-05-11
> **参考**: libuv API design, SQLite API stability, GLib ABI policy
