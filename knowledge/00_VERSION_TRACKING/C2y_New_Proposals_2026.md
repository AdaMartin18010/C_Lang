# C2y 新提案补充 (2026年3月更新)

> **标准**: C2y (C27) | **更新日期**: 2026-03-15 | **来源**: WG14文档日志 (N3700-N3786)

---

## 目录

- [C2y 新提案补充 (2026年3月更新)](#c2y-新提案补充-2026年3月更新)
  - [目录](#目录)
  - [新增提案概览](#新增提案概览)
  - [1. Elvis操作符 `?:` (N3753)](#1-elvis操作符--n3753)
    - [提案内容](#提案内容)
    - [语义规则](#语义规则)
    - [与现有语法的对比](#与现有语法的对比)
    - [使用场景](#使用场景)
    - [状态与兼容性](#状态与兼容性)
  - [2. Named Loops (命名循环) (N3658)](#2-named-loops-命名循环-n3658)
    - [提案内容](#提案内容-1)
    - [语法规范](#语法规范)
    - [完整示例](#完整示例)
    - [编译器支持](#编译器支持)
  - [3. 闭包/函数字面量 (N3657, N3645)](#3-闭包函数字面量-n3657-n3645)
    - [提案内容](#提案内容-2)
    - [语法规范](#语法规范-1)
    - [内存管理挑战](#内存管理挑战)
    - [状态](#状态)
  - [4. `__COUNTER__` 宏 (N3636)](#4-__counter__-宏-n3636)
    - [提案内容](#提案内容-3)
    - [语义规则](#语义规则-1)
    - [与GCC扩展的兼容性](#与gcc扩展的兼容性)
    - [状态](#状态-1)
  - [5. Dependent Structure Types (N3745)](#5-dependent-structure-types-n3745)
    - [提案内容](#提案内容-4)
    - [状态](#状态-2)
  - [更新建议](#更新建议)
    - [知识库更新清单](#知识库更新清单)
    - [代码示例准备](#代码示例准备)

## 新增提案概览

根据WG14 2026年3月最新文档日志，以下提案已接受或正在审议中，需补充至主路线图：

| 提案号 | 标题 | 作者 | 状态 | 预计支持 |
|:-------|:-----|:-----|:----:|:---------|
| N3753 | Elvis操作符 `?:` | Colomar | 审议中 | GCC 17? / Clang 24? |
| N3658 | Named Loops (命名循环) | Celeste | 讨论中 | Clang 22+ 实验性 |
| N3657 | 闭包/函数字面量 | Meneide | 讨论中 | 待定 |
| N3636 | `__COUNTER__` 宏 | Johnson | 讨论中 | GCC 17? |
| N3745 | Dependent Structure Types | Uecker | 审议中 | 待定 |
| N3747 | 整数集合 | Seacord | 审议中 | 待定 |
| N3645 | Literal Functions (函数字面量) | Celeste | 讨论中 | 待定 |

---

## 1. Elvis操作符 `?:` (N3753)

### 提案内容

简化空值检查语法，类似于其他语言中的Elvis操作符：

```c
// C23及之前
void* ptr = get_ptr();
void* safe_ptr = ptr ? ptr : default_ptr;

// C2y 提案 (N3753)
void* safe_ptr = ptr ?: default_ptr;  // 如果ptr为真，返回ptr，否则返回default_ptr
```

### 语义规则

```text
elvis_expr:
    condition ?: fallback

语义:
    - 评估condition
    - 如果condition为真（非零），返回condition的值
    - 如果condition为假（零），返回fallback的值
    - 类似于 (condition ? condition : fallback)，但condition只评估一次
```

### 与现有语法的对比

| 表达式 | 等价展开 | 评估次数 |
|:-------|:---------|:--------:|
| `a ?: b` | `(a) ? (a) : (b)` | a: 1次 |
| `a ? a : b` | `(a) ? (a) : (b)` | a: 2次 |

### 使用场景

```c
// 默认配置
Config* cfg = load_config() ?: &default_config;

// 错误处理
Error* err = do_something() ?: &no_error;

// 链式默认值
int port = cfg->port ?: env_port ?: 8080;
```

### 状态与兼容性

- **状态**: 审议中 (N3753, 2025-12-07提交)
- **编译器支持**: 暂无
- **风险**: 可能与GCC扩展冲突（GCC已支持`?:`作为扩展）

---

## 2. Named Loops (命名循环) (N3658)

### 提案内容

允许为循环命名，实现break/continue到指定外层循环：

```c
// C23及之前: 需要goto或额外标志
for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
        if (condition) {
            goto outer_done;  // 跳出外层循环
        }
    }
}
outer_done:

// C2y 提案 (N3658)
outer: for (int i = 0; i < 10; i++) {
    inner: for (int j = 0; j < 10; j++) {
        if (condition) {
            break outer;  // 跳出外层循环
        }
    }
}
```

### 语法规范

```text
named_loop:
    identifier ':' for_statement
    identifier ':' while_statement
    identifier ':' do_statement

break_statement:
    break identifier ';'  // 跳出指定循环

continue_statement:
    continue identifier ';'  // 继续指定循环
```

### 完整示例

```c
// 矩阵搜索，找到后立即退出所有循环
search: for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        if (matrix[i][j] == target) {
            result_row = i;
            result_col = j;
            break search;  // 直接跳出外层search循环
        }
    }
}

// 嵌套continue
outer: while (condition1) {
    inner: while (condition2) {
        if (skip_outer) {
            continue outer;  // 直接继续外层循环
        }
        // ...
    }
}
```

### 编译器支持

- **Clang**: 22+ 实验性支持
- **GCC**: 暂无官方支持
- **状态**: 讨论中 (N3658, 2025-07-24提交)

---

## 3. 闭包/函数字面量 (N3657, N3645)

### 提案内容

允许定义带有捕获环境的匿名函数（闭包）：

```c
// C2y 提案 (N3657)

// 基本闭包
int multiplier = 10;
auto fn = [](int x) { return x * multiplier; };  // 捕获multiplier
int result = fn(5);  // 50

// 显式捕获
int a = 1, b = 2;
auto sum = `[a, b](void) { return a + b; };`
```

### 语法规范

```text
closure_literal:
    '[' capture_list ']' parameter_list compound_statement

capture_list:
    /* 空 - 无捕获 */
    identifier_list
    '&'  /* 捕获所有变量 */
```

### 内存管理挑战

```c
// 问题：闭包生命周期管理
int* create_multiplier(int factor) {
    auto fn = `[factor](int x) { return x * factor; };`
    return fn;  // 错误：factor在栈上，函数返回后失效
}

// 可能的解决方案：堆分配闭包
int* create_multiplier_heap(int factor) {
    auto fn = heap_closure `[factor](int x) { return x * factor; };`
    return fn;  // 需要手动释放
}
```

### 状态

- **状态**: 早期讨论 (N3657, N3645)
- **挑战**: 与C内存模型的兼容性、生命周期管理
- **预计时间线**: C3x (2030+) 更可能

---

## 4. `__COUNTER__` 宏 (N3636)

### 提案内容

预处理器宏，每次求值产生递增的唯一整数：

```c
// C2y 提案 (N3636)

// 生成唯一标识符
#define UNIQUE_NAME(prefix) prefix##__COUNTER__

int UNIQUE_NAME(var_);  // 展开为 var_0
int UNIQUE_NAME(var_);  // 展开为 var_1
int UNIQUE_NAME(var_);  // 展开为 var_2

// 用于注册表
#define REGISTER(func) \
    static void register_##func(void) __attribute__((constructor)); \
    static void register_##func(void) { \
        registry[__COUNTER__] = func; \
    }
```

### 语义规则

```text
__COUNTER__:
    - 从0开始
    - 每次宏展开时递增
    - 在翻译单元内唯一
    - 不跨翻译单元同步
```

### 与GCC扩展的兼容性

GCC已支持`__COUNTER__`作为扩展，提案旨在标准化：

```c
// GCC扩展 (已存在)
#ifdef __GNUC__
    // 使用 __COUNTER__
#endif

// C2y 标准
#if __STDC_VERSION__ >= 202700L  // C2y
    // 使用标准 __COUNTER__
#endif
```

### 状态

- **状态**: 讨论中 (N3636, 2025-07-02提交)
- **GCC**: 已作为扩展支持
- **预计**: C2y可能包含

---

## 5. Dependent Structure Types (N3745)

### 提案内容

允许结构体成员引用自身类型参数：

```c
// C2y 提案 (N3745)

// 类似于C++的依赖类型
struct Node<T> {
    T value;
    struct Node<T>* next;  // 依赖类型参数
};

// 使用
struct Node<int> int_list;
struct Node<double> double_list;
```

### 状态

- **状态**: 审议中 (N3745, 2025-12-07提交)
- **复杂度**: 高
- **预计**: 可能推迟至C3x

---

## 更新建议

### 知识库更新清单

| 文件 | 更新内容 |
|:-----|:---------|
| `C23_to_C2y_Roadmap.md` | 添加N3753/N3658/N3657/N3636/N3745 |
| `03_C23_Core_Features.md` | 添加Elvis操作符和Named Loops预览 |
| 新文件 | 创建`C2y_Preview/`目录存放详细提案分析 |

### 代码示例准备

由于这些提案尚未标准化，代码示例应：

1. 明确标注为**提案阶段**
2. 提供编译器检测宏
3. 包含fallback实现

```c
// 示例：Named Loops的兼容性写法
#if __STDC_VERSION__ >= 202700L && __has_feature(named_loops)
    // 使用C2y Named Loops
    outer: for (...) { break outer; }
#else
    // Fallback: 使用goto
    for (...) { goto outer_done; }
    outer_done:
#endif
```

---

> **最后更新**: 2026-03-15 | **WG14文档**: <https://www.open-std.org/jtc1/sc22/wg14/www/wg14_document_log>
