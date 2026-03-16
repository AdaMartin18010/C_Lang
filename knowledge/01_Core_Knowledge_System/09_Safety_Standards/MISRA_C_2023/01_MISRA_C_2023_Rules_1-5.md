# MISRA C:2023 Rules 1.1-1.5 - 语言标准符合性

> 逐条详解：规则原文、违反示例、合规代码、检测工具配置

---

## Rule 1.1 - 标准C符合性（Required）

### 规则原文

> The program shall contain no violations of the standard C syntax and constraints, and shall not exceed the implementation's translation limits.
>
> **中文**: 程序不得违反标准C的语法和约束，且不得超过实现的翻译限制。

### 详细解释

这条规则要求代码必须是严格符合ISO C标准的，不能使用编译器特定的扩展。

### C11翻译限制（必须遵守）

| 限制项 | C11最小值 | 超标后果 |
|:-------|:----------|:---------|
| 嵌套块层数 | 127 | 编译失败 |
| 函数参数 | 127 | 编译失败 |
| 标识符有效长度 | 63 | 名称冲突 |
| 逻辑源行字符数 | 4095 | 截断/编译错误 |

### 违反示例

```c
/* ❌ 违反 - 使用GCC嵌套函数扩展（非标准C） */
void foo(void)
{
    void bar(void) {  /* GCC扩展 */
        printf("nested\n");
    }
    bar();
}

/* ❌ 违反 - 超过127个函数参数 */
void func(int a0, ..., int a127, int a128);  /* 128个参数 */
```

### 合规示例

```c
/* ✅ 合规 - 标准C */
void foo(void);

static void bar(void) {  /* 独立的静态函数 */
    printf("standard C\n");
}

void foo(void)
{
    bar();
}
```

### 检测配置

```bash
# GCC
gcc -std=c11 -pedantic -Werror -c file.c

# PC-lint
pc-lint -std=c11 +strict file.c
```

---

## Rule 1.2 - 语言扩展（Advisory）

### 规则原文

> Language extensions should not be used.

### 常见扩展与替代

| 扩展 | GCC/Clang | 可移植替代 |
|:-----|:----------|:-----------|
| `__attribute__((packed))` | ✅ | 使用位域或手动填充 |
| `typeof` | ✅ | C23 `typeof` |
| `__thread` | ✅ | C11 `_Thread_local` |

### 违反示例

```c
/* ❌ 违反 - GCC packed属性 */
struct __attribute__((packed)) Packet {
    uint8_t header;
    uint32_t data;
};
```

### 合规示例

```c
/* ✅ 合规 - 手动填充 */
struct Packet {
    uint8_t header;
    uint8_t _padding[3];
    uint32_t data;
};

_Static_assert(sizeof(struct Packet) == 8, "Size check");
```

---

## Rule 1.3 - 未定义行为（Required）

### 规则原文

> There shall be no occurrence of undefined or critical unspecified behaviour.

### C11常见UB（必须避免）

| UB类型 | 示例 | 修复方法 |
|:-------|:-----|:---------|
| 有符号溢出 | `INT_MAX + 1` | 使用无符号或检查溢出 |
| 空指针解引用 | `*NULL` | 检查指针非空 |
| 数组越界 | `arr[10]` 当大小为10 | 使用`<`而非`<=` |
| 未初始化变量 | `int x; use(x);` | 始终初始化 |

### 违反示例

```c
/* ❌ 违反 - 有符号整数溢出（UB） */
int add(int a, int b) {
    return a + b;  /* 溢出 = 灾难 */
}

/* ❌ 违反 - 未检查空指针 */
void process(int *p) {
    *p = 42;  /* p可能为NULL */
}
```

### 合规示例

```c
/* ✅ 合规 - C23溢出检查 */
#include <stdckdint.h>
int add_safe(int a, int b, int *result) {
    return ckd_add(result, a, b);  /* 返回true表示溢出 */
}

/* ✅ 合规 - 空指针检查 */
void process_safe(int *p) {
    if (p != NULL) {
        *p = 42;
    }
}
```

### 运行时检测

```bash
# Clang UB Sanitizer
clang -fsanitize=undefined -g program.c
./program  # 运行时检测
```

---

## Rule 1.4 - 运行时错误（Required）

### 规则原文

> The implementation shall not invoke abort, exit, quick_exit, or thrd_exit.

### 嵌入式系统错误处理策略

```c
/* ✅ 合规 - 错误码+恢复机制 */
typedef enum {
    ERR_NONE = 0,
    ERR_HARDWARE,
    ERR_MEMORY
} error_t;

static volatile error_t g_last_error = ERR_NONE;

void handle_error(error_t error)
{
    g_last_error = error;
    log_error(error);
    /* 尝试恢复或进入安全状态 */
    enter_safe_state();
}

/* 不使用exit/abort */
```

---

## Rule 1.5 - 字符集（Advisory）

### 规则原文

> Characters and strings should only use the C11 standard source character set.

### 策略

```c
/* ✅ 源代码使用ASCII */
const char *msg = "Temperature: 25C";

/* ✅ 本地化通过资源文件 */
const char *localized = get_string(STRING_ID_TEMP);
```

---

## 总结

| 规则 | 强制 | 关键行动 |
|:----:|:----:|:---------|
| 1.1 | Required | 使用`-std=c11 -pedantic`编译 |
| 1.2 | Advisory | 避免`__attribute__`等扩展 |
| 1.3 | Required | 使用UBSan检测未定义行为 |
| 1.4 | Required | 用错误码替代exit/abort |
| 1.5 | Advisory | 源代码纯ASCII |

---

**参考**: MISRA C:2023 Guidelines, ISO/IEC 9899:2011 Annex J
