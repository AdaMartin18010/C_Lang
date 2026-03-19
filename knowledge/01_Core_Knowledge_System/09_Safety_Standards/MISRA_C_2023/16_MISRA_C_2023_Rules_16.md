# MISRA C:2023 Rules 16.1-16.7 - 函数调用

> 函数参数、递归、返回值处理

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 16.1 | 参数数量 | Required | 参数数量匹配 |
| Rule 16.2 | 参数类型 | Required | 参数类型兼容 |
| Rule 16.3 | 可变参数 | Required | 可变参数限制 |
| Rule 16.4 | 递归控制 | Required | 递归深度控制 |
| Rule 16.5 | 回调函数 | Required | 回调安全 |
| Rule 16.6 | 间接调用 | Required | 限制函数指针调用 |
| Rule 16.7 | 未使用参数 | Required | 处理未使用参数 |

---

## Rule 16.1 - 参数数量

### 规则原文

> **Rule 16.1 (Required)**: The number of arguments passed to a function shall match the number of parameters.
>
> **中文**: 传递给函数的参数数量应与参数数量匹配。

### 违反示例

```c
/* ❌ 违反 - 参数不匹配 */
void func(int a, int b);
func(1);        /* 参数太少 */
func(1, 2, 3);  /* 参数太多 */

/* ❌ 违反 - 省略号函数滥用 */
void bad_func(...);  /* 无固定参数，危险 */
bad_func();  /* 无参数 */
bad_func(1, "test", 3.14);  /* 随意传递 */
```

### 合规示例

```c
/* ✅ 合规 - 原型声明 */
void func(int a, int b);

void call(void)
{
    func(1, 2);  /* 匹配 */
}

/* ✅ 合规 - 使用结构体传递多参数 */
typedef struct {
    int x;
    int y;
    int width;
    int height;
} Rect;

void draw_rect(const Rect *r);

/* 使用 */
Rect r = {.x = 0, .y = 0, .width = 100, .height = 100};
draw_rect(&r);
```

---

## Rule 16.2 - 参数类型

### 规则原文

> **Rule 16.2 (Required)**: The type of arguments passed to a function shall match the types of the parameters.
>
> **中文**: 传递给函数的参数类型应与参数类型匹配。

### 违反示例

```c
/* ❌ 违反 - 类型不匹配 */
void process(int value);
float f = 3.14f;
process(f);  /* 隐式float转int */

/* ❌ 违反 - 指针类型不匹配 */
void use_buffer(char *buf);
int data[10];
use_buffer((char *)data);  /* 不安全 */

/* ❌ 违反 - 符号性不匹配 */
void set_size(unsigned int size);
int len = -1;
set_size(len);  /* 负值变正值 */
```

### 合规示例

```c
/* ✅ 合规 - 类型匹配 */
void process(int value);
process(42);  /* 匹配 */

/* ✅ 合规 - 显式转换 */
float f = 3.14f;
process((int)f);  /* 显式 */

/* ✅ 合规 - 使用正确类型 */
void set_size(size_t size);
size_t len = 100;
set_size(len);

/* ✅ 合规 - 类型安全包装 */
void process_float(float f)
{
    int value = (int)round(f);  /* 正确处理 */
    process(value);
}
```

---

## Rule 16.3 - 可变参数

### 规则原文

> **Rule 16.3 (Required)**: Variable argument lists shall be used safely.
>
> **中文**: 可变参数列表应安全使用。

### 安全可变参数模式

```c
#include <stdarg.h>

/* ✅ 合规 - 类型安全的日志函数 */
typedef enum {
    ARG_INT,
    ARG_FLOAT,
    ARG_STRING,
    ARG_POINTER
} ArgType;

typedef struct {
    ArgType type;
    union {
        int i;
        float f;
        const char *s;
        void *p;
    } value;
} TypedArg;

void log_message(const char *fmt, size_t arg_count, const TypedArg *args)
{
    /* 解析fmt，使用arg_count限制访问 */
    for (size_t i = 0; i < arg_count; i++) {
        switch (args[i].type) {
            case ARG_INT:    /* 使用args[i].value.i */ break;
            case ARG_FLOAT:  /* 使用args[i].value.f */ break;
            case ARG_STRING: /* 使用args[i].value.s */ break;
            case ARG_POINTER:/* 使用args[i].value.p */ break;
        }
    }
}

/* ✅ 合规 - 标准printf的安全使用 */
#define LOG_INFO(...) do { \
    printf("[INFO] " __VA_ARGS__); \
    printf("\n"); \
} while(0)

/* 注意：__VA_ARGS__在编译时检查格式字符串 */
```

---

## Rule 16.4 - 递归控制

### 规则原文

> **Rule 16.4 (Required)**: Recursion shall be controlled.
>
> **中文**: 应控制递归的使用。

### 受控递归模式

```c
/* ✅ 受控递归 - 有深度限制 */
#define MAX_RECURSION_DEPTH 100

static int recursive_helper(Node *node, int depth, int *result)
{
    if (node == NULL) {
        return 0;
    }
    
    if (depth > MAX_RECURSION_DEPTH) {
        return ERROR_DEPTH_EXCEEDED;
    }
    
    int left_result, right_result;
    int err;
    
    err = recursive_helper(node->left, depth + 1, &left_result);
    if (err != 0) return err;
    
    err = recursive_helper(node->right, depth + 1, &right_result);
    if (err != 0) return err;
    
    *result = combine(left_result, right_result, node->value);
    return 0;
}

int process_tree(Node *root, int *result)
{
    return recursive_helper(root, 0, result);
}

/* ✅ 建议 - 迭代替代递归 */
int factorial_iterative(int n)
{
    if (n < 0) return ERROR;
    
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}
```

---

## Rule 16.5 - 回调函数

### 规则原文

> **Rule 16.5 (Required)**: Callback functions shall be used safely.
>
> **中文**: 回调函数应安全使用。

### 安全回调模式

```c
/* ✅ 合规 - 带上下文的安全回调 */
typedef void (*Callback)(void *context, int result);

typedef struct {
    Callback callback;
    void *context;
    bool valid;
} CallbackInfo;

static CallbackInfo registered_callback = {NULL, NULL, false};

void register_callback(Callback cb, void *context)
{
    if (cb == NULL) {
        registered_callback.valid = false;
        return;
    }
    
    registered_callback.callback = cb;
    registered_callback.context = context;
    registered_callback.valid = true;
}

void trigger_callback(int result)
{
    if (registered_callback.valid) {
        registered_callback.callback(
            registered_callback.context, 
            result
        );
    }
}

void unregister_callback(void)
{
    registered_callback.valid = false;
    registered_callback.callback = NULL;
    registered_callback.context = NULL;
}
```

---

## Rule 16.6 - 间接调用

### 规则原文

> **Rule 16.6 (Required)**: The function called indirectly shall be compatible with the call.
>
> **中文**: 间接调用的函数应与调用兼容。

### 合规示例

```c
/* ✅ 合规 - 使用函数指针表 */
typedef int (*Operation)(int, int);

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }

static const Operation operations[] = {
    [0] = add,
    [1] = sub,
    [2] = mul
};

#define NUM_OPS (sizeof(operations) / sizeof(operations[0]))

int execute_op(int op_code, int a, int b, int *result)
{
    if (op_code < 0 || (size_t)op_code >= NUM_OPS) {
        return ERROR_INVALID_OPCODE;
    }
    
    if (operations[op_code] == NULL) {
        return ERROR_NOT_IMPLEMENTED;
    }
    
    *result = operations[op_code](a, b);
    return 0;
}
```

---

## Rule 16.7 - 未使用参数

### 规则原文

> **Rule 16.7 (Required)**: Unused parameters shall be indicated as such.
>
> **中文**: 未使用的参数应标明。

### 合规示例

```c
/* ✅ 合规 - 显式标记未使用参数 */
void callback(void *user_data, int event)
{
    (void)user_data;  /* 明确不使用 */
    
    process_event(event);
}

/* ✅ 合规 - 使用属性标记 */
#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

void handler(int required, int UNUSED optional)
{
    /* 只使用required */
    process(required);
}

/* ✅ 合规 - 接口兼容 */
void interface_impl(int a, int b, int c)
{
    /* 此实现不需要b和c */
    (void)b;
    (void)c;
    
    use(a);
}
```

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 16


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
