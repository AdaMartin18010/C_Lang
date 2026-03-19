# MISRA C:2023 Rules 13.1-13.6 - 函数安全

> 函数设计、参数验证、返回值处理

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 13.1 | 函数长度 | Advisory | 限制函数长度 |
| Rule 13.2 | 参数数量 | Advisory | 限制参数数量 |
| Rule 13.3 | 参数验证 | Required | 验证所有参数 |
| Rule 13.4 | 返回值 | Required | 使用或检查返回值 |
| Rule 13.5 | 递归 | Required | 限制递归使用 |
| Rule 13.6 | 可变参数 | Required | 限制可变参数 |

---

## Rule 13.1 - 函数长度

### 规则原文

> **Rule 13.1 (Advisory)**: Functions should not exceed a defined length.
>
> **中文**: 函数不应超过定义的长度。

### 建议限制

| 指标 | 建议值 | 说明 |
|:-----|:-------|:-----|
| 源代码行数 | ≤ 60行 | 不包括注释和空行 |
| 圈复杂度 | ≤ 10 | McCabe复杂度 |
| 嵌套深度 | ≤ 4层 | if/for/while嵌套 |

### 重构示例

```c
/* ❌ 过长函数 */
void process_everything(void)
{
    /* 100+ 行代码 */
    /* 打开文件 */
    /* 读取配置 */
    /* 初始化硬件 */
    /* 主处理循环 */
    /* 保存结果 */
    /* 关闭资源 */
}

/* ✅ 拆分为小函数 */
static int load_config(Config *cfg);
static int init_hardware(void);
static int process_data(const Config *cfg);
static int save_results(void);

void process_everything(void)
{
    Config cfg = {0};

    if (load_config(&cfg) != 0) {
        return;
    }

    if (init_hardware() != 0) {
        return;
    }

    if (process_data(&cfg) != 0) {
        return;
    }

    save_results();
}
```

---

## Rule 13.2 - 参数数量

### 规则原文

> **Rule 13.2 (Advisory)**: The number of parameters should not exceed a defined limit.
>
> **中文**: 参数数量不应超过定义的限值。

### 建议限制

| 场景 | 建议最大参数 | 替代方案 |
|:-----|:-------------|:---------|
| 一般函数 | 4-5个 | 结构体封装 |
| 初始化函数 | 6-8个 | 配置结构体 |
| 回调函数 | 2-3个 | 上下文指针 |

### 重构示例

```c
/* ❌ 参数过多 */
void create_window(int x, int y, int width, int height,
                   bool visible, bool resizable, bool fullscreen,
                   const char *title, int style);

/* ✅ 使用结构体 */
typedef struct {
    int x, y;
    int width, height;
    bool visible;
    bool resizable;
    bool fullscreen;
    const char *title;
    int style;
} WindowConfig;

void create_window(const WindowConfig *config);

/* 使用 */
WindowConfig cfg = {
    .x = 100, .y = 100,
    .width = 800, .height = 600,
    .visible = true,
    .title = "My App"
};
create_window(&cfg);
```

---

## Rule 13.3 - 参数验证

### 规则原文

> **Rule 13.3 (Required)**: Pointer arguments shall be checked for null.
>
> **中文**: 指针参数应检查是否为null。

### 验证模式

```c
/* ✅ 防御性编程模式 */

/* 1. 早期返回模式 */
int process_data(const Data *input, Result *output)
{
    if (input == NULL || output == NULL) {
        return ERROR_INVALID_PARAM;
    }

    /* 处理 */
    return SUCCESS;
}

/* 2. 断言模式（调试）*/
int process_data_assert(const Data *input, Result *output)
{
    assert(input != NULL);
    assert(output != NULL);

    /* 处理 */
    return SUCCESS;
}

/* 3. 范围验证 */
int set_timeout(int milliseconds)
{
    if (milliseconds < MIN_TIMEOUT || milliseconds > MAX_TIMEOUT) {
        return ERROR_OUT_OF_RANGE;
    }

    g_timeout = milliseconds;
    return SUCCESS;
}

/* 4. 数组长度验证 */
int copy_array(const int *src, size_t src_len,
               int *dst, size_t dst_len)
{
    if (src == NULL || dst == NULL) {
        return ERROR_NULL_POINTER;
    }

    if (src_len > dst_len) {
        return ERROR_BUFFER_TOO_SMALL;
    }

    memcpy(dst, src, src_len * sizeof(int));
    return SUCCESS;
}
```

---

## Rule 13.4 - 返回值处理

### 规则原文

> **Rule 13.4 (Required)**: The return value of a function shall be used or explicitly cast to void.
>
> **中文**: 函数返回值应被使用或显式转换为void。

### 违反示例

```c
/* ❌ 违反 - 忽略错误 */
malloc(100);  /* 内存泄漏风险 */
strcpy(dest, src);  /* 溢出风险 */
fclose(fp);  /* 错误被忽略 */
remove("file");  /* 删除失败不知道 */

/* ❌ 违反 - 部分检查 */
printf("Value: %d\n", value);  /* 返回值忽略 */
```

### 合规示例

```c
/* ✅ 合规 - 检查返回值 */
void *mem = malloc(100);
if (mem == NULL) {
    /* 错误处理 */
}

/* ✅ 合规 - 显式忽略（有文档说明）*/
(void)printf("Debug: %d\n", value);  /* 显式转换void */

/* ✅ 合规 - 错误处理 */
if (fclose(fp) != 0) {
    log_error("Failed to close file");
}

/* ✅ 合规 - 包装函数 */
void safe_remove(const char *path)
{
    if (remove(path) != 0) {
        /* 记录但不一定终止 */
        log_warning("Failed to remove %s", path);
    }
}
```

---

## Rule 13.5 - 递归限制

### 规则原文

> **Rule 13.5 (Required)**: Recursion shall be controlled.
>
> **中文**: 应控制递归的使用。

### 安全递归模式

```c
/* ✅ 受控递归 - 有深度限制 */
#define MAX_RECURSION_DEPTH 100

static int recursive_helper(Node *node, int depth)
{
    if (node == NULL) {
        return 0;
    }

    if (depth > MAX_RECURSION_DEPTH) {
        return ERROR_DEPTH_EXCEEDED;
    }

    int left = recursive_helper(node->left, depth + 1);
    int right = recursive_helper(node->right, depth + 1);

    return left + right + node->value;
}

int process_tree(Node *root)
{
    return recursive_helper(root, 0);
}

/* ✅ 尾递归优化（编译器支持时）*/
int factorial(int n, int acc)
{
    if (n <= 1) {
        return acc;
    }
    return factorial(n - 1, n * acc);
}

/* ✅ 建议：迭代替代递归 */
int factorial_iterative(int n)
{
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}
```

---

## Rule 13.6 - 可变参数

### 规则原文

> **Rule 13.6 (Required)**: Variable arguments shall be used safely.
>
> **中文**: 可变参数应安全使用。

### 安全可变参数模式

```c
/* ✅ 类型安全的可变参数 */

/* 1. 使用宏包装（编译时检查）*/
#define LOG_INFO(...) log_message(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_ERROR(...) log_message(LOG_LEVEL_ERROR, __VA_ARGS__)

/* 2. 明确的格式字符串 */
void log_message(int level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    /* 格式字符串必须是常量 */
    vfprintf(log_file, fmt, args);

    va_end(args);
}

/* 3. 替代方案：使用结构体 */
typedef struct {
    const char *key;
    union {
        int i;
        double d;
        const char *s;
        bool b;
    } value;
    enum { TYPE_INT, TYPE_DOUBLE, TYPE_STRING, TYPE_BOOL } type;
} LogParam;

void log_structured(const char *message,
                    size_t param_count,
                    const LogParam *params);

/* 使用 */
LogParam params[] = {
    {"user_id", {.i = 123}, TYPE_INT},
    {"action", {.s = "login"}, TYPE_STRING}
};
log_structured("User action", 2, params);
```

---

## 函数安全检查清单

```markdown
□ 函数长度≤60行
□ 参数数量≤4-5个
□ 所有指针参数检查NULL
□ 返回值使用或显式忽略
□ 递归有深度限制
□ 可变参数类型安全
□ 单一出口（建议）
□ 圈复杂度≤10
```

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 13


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
