# C2y `__COUNTER__` 宏

> **提案**: N3636 - The `__COUNTER__` predefined macro
> **状态**: 讨论中
> **作者**: Aaron Johnson
> **预计支持**: GCC 17 / Clang 25
> **已有实现**: GCC/Clang长期作为扩展支持

---

## 1. 概述

`__COUNTER__` 是一个预处理器宏，在每次宏展开时自动递增，生成翻译单元内唯一的整数序列。用于生成唯一标识符、自动注册代码等场景。

```c
// C2y: __COUNTER__ 自动生成唯一ID
#define UNIQUE_NAME(prefix) prefix##__COUNTER__

int UNIQUE_NAME(var_);  // 展开为 var_0
int UNIQUE_NAME(var_);  // 展开为 var_1
int UNIQUE_NAME(var_);  // 展开为 var_2
```

---

## 2. 语法与语义

### 2.1 基本规则

```c
__COUNTER__  // 每次宏展开时从0开始递增
```

**特性**:

- 从 `0` 开始
- 每次宏展开时递增
- 在**翻译单元**内唯一
- 不跨翻译单元同步
- 多次编译可能产生不同值

### 2.2 与 `__LINE__` 对比

| 宏 | 值 | 用途 |
|:---|:---|:-----|
| `__LINE__` | 当前行号 | 错误定位、日志 |
| `__COUNTER__` | 递增序列 | 生成唯一ID、注册 |

```c
// __LINE__ 示例
#define LOG(msg) printf("[%s:%d] %s\n", __FILE__, __LINE__, msg)

// __COUNTER__ 示例
#define UNIQUE_ID __COUNTER__
```

---

## 3. 使用场景

### 3.1 自动生成唯一变量名

```c
// C2y: 生成唯一变量名，避免冲突
#define GUARD_VAR(name) _guard_##name##_##__COUNTER__

void example(void) {
    int GUARD_VAR(lock) = acquire_lock();
    defer release_lock(GUARD_VAR(lock));  // 配合defer使用

    // 使用...
}
```

### 3.2 自动注册表

```c
// C2y: 函数自动注册系统

#define MAX_REGISTERED 100

typedef void (*handler_t)(void);

static handler_t registry[MAX_REGISTERED];
static int registry_count = 0;

#define REGISTER(func) \
    static void _register_##func##_##__COUNTER__(void) __attribute__((constructor)); \
    static void _register_##func##_##__COUNTER__(void) { \
        if (registry_count < MAX_REGISTERED) { \
            registry[__COUNTER__] = func; \
            registry_count++; \
        } \
    }

// 使用
void handler1(void) { printf("Handler 1\n"); }
void handler2(void) { printf("Handler 2\n"); }
void handler3(void) { printf("Handler 3\n"); }

REGISTER(handler1)  // 注册到 registry[0]
REGISTER(handler2)  // 注册到 registry[1]
REGISTER(handler3)  // 注册到 registry[2]

// 执行所有注册的handler
void execute_all(void) {
    for (int i = 0; i < registry_count; i++) {
        registry[i]();
    }
}
```

### 3.3 测试用例自动生成

```c
// C2y: 测试框架
#include <stdio.h>

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    static void test_##name(void); \
    static void __attribute__((constructor)) register_test_##name##_##__COUNTER__(void) { \
        printf("Registering test: %s\n", #name); \
        test_##name(); \
    } \
    static void test_##name(void)

#define ASSERT_EQ(a, b) do { \
    tests_run++; \
    if ((a) == (b)) { \
        tests_passed++; \
        printf("  [PASS] %s == %s\n", #a, #b); \
    } else { \
        printf("  [FAIL] %s != %s (%d vs %d)\n", #a, #b, (a), (b)); \
    } \
} while(0)

// 定义测试
TEST(addition) {
    ASSERT_EQ(1 + 1, 2);
    ASSERT_EQ(2 + 2, 4);
}

TEST(subtraction) {
    ASSERT_EQ(5 - 3, 2);
    ASSERT_EQ(10 - 5, 5);
}

// 测试总结
static void __attribute__((destructor)) print_summary(void) {
    printf("\n=== Test Summary ===\n");
    printf("Run: %d, Passed: %d, Failed: %d\n",
           tests_run, tests_passed, tests_run - tests_passed);
}
```

### 3.4 序列化字段编号

```c
// C2y: 协议缓冲区风格的字段编号

typedef enum {
#define DEFINE_FIELD(name, type) FIELD_##name = __COUNTER__

    DEFINE_FIELD(id, int),      // 0
    DEFINE_FIELD(name, string), // 1
    DEFINE_FIELD(age, int),     // 2
    DEFINE_FIELD(email, string) // 3

#undef DEFINE_FIELD
} FieldNumber;

// 序列化代码可以使用这些编号
void serialize_int(FieldNumber field, int value) {
    printf("Field %d: %d\n", field, value);
}

// 使用
serialize_int(FIELD_id, 42);    // Field 0: 42
serialize_int(FIELD_age, 25);   // Field 2: 25
```

### 3.5 状态机状态自动生成

```c
// C2y: 自动生成状态机状态值

typedef enum {
#define STATE(name) STATE_##name = __COUNTER__

    STATE(IDLE),      // 0
    STATE(INIT),      // 1
    STATE(RUNNING),   // 2
    STATE(PAUSED),    // 3
    STATE(STOPPING),  // 4
    STATE(STOPPED)    // 5

#undef STATE
} State;

static const char* state_names[] = {
#define STATE(name) [STATE_##name] = #name
    STATE(IDLE),
    STATE(INIT),
    STATE(RUNNING),
    STATE(PAUSED),
    STATE(STOPPING),
    STATE(STOPPED)
#undef STATE
};

const char* get_state_name(State s) {
    if (s < sizeof(state_names)/sizeof(state_names[0])) {
        return state_names[s];
    }
    return "UNKNOWN";
}

// 使用
printf("State: %s\n", get_state_name(STATE_RUNNING));  // State: RUNNING
```

---

## 4. 与现有GCC扩展的关系

### 4.1 GCC/Clang已支持

```bash
# GCC/Clang已长期支持__COUNTER__作为扩展
gcc -std=c11 -Wall test.c  # __COUNTER__ 可用
```

### 4.2 标准化变化

C2y提案旨在标准化现有行为，主要明确：

- 确切的展开时机
- 与标准宏的交互
- 跨平台一致性

```c
// GCC/Clang扩展 (当前)
#ifdef __GNUC__
    int x##__COUNTER__;  // 可用
#endif

// C2y标准 (预计)
#if __STDC_VERSION__ >= 202700L
    int x##__COUNTER__;  // 标准语法
#endif
```

---

## 5. 注意事项

### 5.1 翻译单元隔离

```c
// file1.c
int var_##__COUNTER__;  // var_0

// file2.c
int var_##__COUNTER__;  // var_0 (独立的计数器)
```

**注意**: 每个翻译单元独立计数，不能保证全局唯一。

### 5.2 多次编译不一致

```c
// 第一次编译
int a##__COUNTER__;  // a_0
int b##__COUNTER__;  // b_1

// 修改代码后重新编译
int x##__COUNTER__;  // x_0 (可能在原来a_0的位置)
int a##__COUNTER__;  // a_1 (可能变为1)
```

**警告**: `__COUNTER__` 值在不同编译间可能变化，不能依赖具体数值。

### 5.3 与条件编译的交互

```c
// 不推荐: 条件编译可能导致意外结果
#ifdef FEATURE_A
    int var_##__COUNTER__;  // 如果FEATURE_A定义，占用0
#endif

int always_present_##__COUNTER__;  // 可能是0或1，取决于FEATURE_A
```

---

## 6. 编译器支持

| 编译器 | 版本 | 支持 |
|:-------|:-----|:-----|
| GCC | 4.3+ | 作为扩展支持 |
| Clang | 所有版本 | 作为扩展支持 |
| MSVC | 部分版本 | 有限支持 |
| ICC | 部分版本 | 支持 |

**标准支持**: 预计C2y (2027-2028)

---

## 7. 最佳实践

### 7.1 使用场景选择

```c
// 推荐: __COUNTER__适用场景

// ✅ 生成唯一标识符
#define UNIQUE_ID prefix##__COUNTER__

// ✅ 自动注册系统
#define REGISTER(func) /* 使用__COUNTER__作为索引 */

// ✅ 枚举值自动生成
#define AUTO_ENUM(name) ENUM_##name = __COUNTER__

// 不推荐: 以下场景
// ❌ 依赖具体数值 (值可能在不同编译间变化)
// ❌ 跨翻译单元同步 (每个TU独立计数)
// ❌ 持久化存储 (值不稳定)
```

### 7.2 封装使用

```c
// 推荐: 封装__COUNTER__，便于移植

#ifndef __COUNTER__
    #define __COUNTER__ __LINE__  // 退化方案
#endif

#define CONCAT(a, b) a##b
#define CONCAT_EXP(a, b) CONCAT(a, b)
#define UNIQUE_NAME(prefix) CONCAT_EXP(prefix, __COUNTER__)

// 使用
int UNIQUE_NAME(var_);  // var_<N>
```

---

## 8. 参考

- **提案文档**: N3636 - The `__COUNTER__` predefined macro (2025-07-02)
- **作者**: Aaron Johnson
- **WG14日志**: <https://www.open-std.org/jtc1/sc22/wg14/www/wg14_document_log>
- **GCC文档**: <https://gcc.gnu.org/onlinedocs/gcc/Common-Predefined-Macros.html>

---

**状态**: 讨论中
**最后更新**: 2026-03-24
