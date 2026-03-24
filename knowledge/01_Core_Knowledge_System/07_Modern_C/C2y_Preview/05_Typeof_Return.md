# C2y `typeof(return)` 表达式

---

## 🔗 知识关联网络

### 1. 全局导航

| 层级 | 文档 | 作用 |
|:-----|:-----|:-----|
| 总索引 | [../../../00_GLOBAL_INDEX.md](../../../00_GLOBAL_INDEX.md) | 完整知识图谱入口 |
| 本模块 | [../../../readme.md](../../../readme.md) | 模块总览与导航 |
| 学习路径 | [../../../06_Thinking_Representation/06_Learning_Paths/readme.md](../../../06_Thinking_Representation/06_Learning_Paths/readme.md) | 推荐学习路线 |

### 2. 前置知识依赖

| 文档 | 关系 | 掌握要求 |
|:-----|:-----|:---------|
| [../../../01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md](../../../01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md) | 语言基础 | 必须掌握 |
| [../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心机制 | 必须掌握 |
| [../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 内存基础 | 必须掌握 |

### 3. 同层横向关联

| 文档 | 关系 | 互补内容 |
|:-----|:-----|:---------|
| [../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 技术扩展 | 并发编程技术 |
| [../../../02_Formal_Semantics_and_Physics/readme.md](../../../02_Formal_Semantics_and_Physics/readme.md) | 理论支撑 | 形式化方法 |
| [../../../04_Industrial_Scenarios/readme.md](../../../04_Industrial_Scenarios/readme.md) | 实践应用 | 工业实践案例 |

### 4. 深层理论关联

| 文档 | 关系 | 理论深度 |
|:-----|:-----|:---------|
| [../../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md](../../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md) | 语义基础 | 操作语义、类型理论 |
| [../../../06_Thinking_Representation/05_Concept_Mappings/readme.md](../../../06_Thinking_Representation/05_Concept_Mappings/readme.md) | 概念映射 | 知识关联网络 |

### 5. 后续进阶延伸

| 文档 | 关系 | 进阶方向 |
|:-----|:-----|:---------|
| [../../../03_System_Technology_Domains/readme.md](../../../03_System_Technology_Domains/readme.md) | 系统技术 | 系统级开发 |
| [../../../01_Core_Knowledge_System/09_Safety_Standards/readme.md](../../../01_Core_Knowledge_System/09_Safety_Standards/readme.md) | 安全标准 | 安全编码规范 |
| [../../../07_Modern_Toolchain/readme.md](../../../07_Modern_Toolchain/readme.md) | 工具链 | 现代开发工具 |

### 6. 阶段学习定位

```
当前位置: 根据文档主题确定学习阶段
├─ 入门阶段: 基础语法、数据类型
├─ 基础阶段: 控制流程、函数
├─ 进阶阶段: 指针、内存管理 ⬅️ 可能在此
├─ 高级阶段: 并发、系统编程
└─ 专家阶段: 形式验证、编译器
```

### 7. 局部索引

本文件所属模块的详细内容：

- 参见本模块 [readme.md](../../../readme.md)
- 相关子目录文档


> **提案**: N3454 - typeof(return)
> **状态**: 审议中
> **作者**: Alejandro Colomar
> **预计支持**: GCC 17 / Clang 25

---

## 1. 概述

`typeof(return)` 是C2y提案中的新特性，允许在函数内部获取该函数返回值的类型。这在泛型编程和宏定义中特别有用，可以创建与函数返回类型相匹配的临时变量。

```c
// C2y: 获取函数返回类型
int process_data(void);

typeof(return) result = process_data();  // 类型为 int

// 在泛型宏中的应用
#define CACHE_RESULT(func, ...) ({ \
    static typeof(return) cache; \
    static int initialized = 0; \
    if (!initialized) { \
        cache = func(__VA_ARGS__); \
        initialized = 1; \
    } \
    cache; \
})
```

---

## 2. 语法与语义

### 2.1 基本用法

```c
// 在任何返回值的函数内部使用
typeof(return) var_name;
```

**规则**:

- 只能在函数内部使用
- 获取该函数声明的返回类型
- 包括const/volatile等限定符

### 2.2 与typeof的区别

| 表达式 | 含义 | 使用位置 |
|:-------|:-----|:---------|
| `typeof(expr)` | expr的类型 | 任何位置 |
| `typeof(return)` | 当前函数的返回类型 | 函数内部 |
| `typeof(*(T*)0)` | T的解引用类型 | 类型构造 |

```c
int foo(void) {
    int x;

    typeof(x) a;           // int (变量类型)
    typeof(return) b;      // int (函数返回类型)
    typeof(foo()) c;       // int (表达式类型)

    // 三者在这里等价，但typeof(return)更清晰表达意图
}

// 复杂返回类型
const int* bar(void);

void baz(void) {
    typeof(return) p = bar();  // const int* 类型

    // 对比
    typeof(bar()) q = bar();   // 同样效果，但需调用函数
    typeof(int) r = bar();     // 需要知道返回类型
}
```

---

## 3. 使用场景

### 3.1 泛型缓存宏

```c
// C2y: 函数结果缓存宏
#define memoize(func, ...) ({ \
    static typeof(return) cached_value; \
    static _Atomic int cache_initialized = 0; \
    \
    if (!atomic_load(&cache_initialized)) { \
        cached_value = func(__VA_ARGS__); \
        atomic_store(&cache_initialized, 1); \
    } \
    cached_value; \
})

// 使用
int expensive_computation(int x, int y);

int get_result(int x, int y) {
    // 自动推导返回类型
    return memoize(expensive_computation, x, y);
}
```

### 3.2 结果包装器

```c
// C2y: 带错误码的返回值包装
typedef struct {
    typeof(return) value;
    int error_code;
} Result;

#define TRY_CALL(func, ...) ({ \
    Result _result; \
    _result.error_code = 0; \
    _result.value = func(__VA_ARGS__); \
    if (_result.value < 0) { \
        _result.error_code = errno; \
    } \
    _result; \
})

// 使用
int open_file(const char* path);

void process(const char* path) {
    Result r = TRY_CALL(open_file, path);
    if (r.error_code != 0) {
        // 错误处理
        return;
    }
    // 使用 r.value
}
```

### 3.3 递归辅助函数

```c
// C2y: 递归算法中的临时变量
int factorial(int n) {
    if (n <= 1) return 1;

    typeof(return) partial = factorial(n - 1);  // 明确与返回类型一致
    typeof(return) result = n * partial;

    return result;
}

// 复杂递归
typedef struct Node Node;
struct Node {
    int value;
    Node* left;
    Node* right;
};

int tree_sum(const Node* root) {
    if (!root) return 0;

    typeof(return) left_sum = tree_sum(root->left);
    typeof(return) right_sum = tree_sum(root->right);
    typeof(return) total = root->value + left_sum + right_sum;

    return total;
}
```

### 3.4 链式调用辅助

```c
// C2y: 构建流式API
#define CHAIN_CALL(obj, method, ...) ({ \
    typeof(obj) _o = obj; \
    _o = _o.method(__VA_ARGS__); \
    _o; \
})

// 或更通用的版本
#define PIPE(value, func, ...) ({ \
    typeof(return) _result = func(value, __VA_ARGS__); \
    _result; \
})
```

---

## 4. 与C23 typeof结合

### 4.1 类型萃取模式

```c
// C23 + C2y: 类型萃取宏
#define make_result_type(func) typeof(func())
#define make_return_type(func) typeof(return)

// 构建相关类型
typedef typeof(return) base_type;
typedef base_type* ptr_type;
typedef const base_type const_type;
typedef base_type array_type[10];

// 函数原型
base_type compute_value(int n);

// 使用萃取类型
void process(void) {
    base_type values[100];      // 与返回类型匹配的数组
    ptr_type ptr = &values[0];  // 指向该类型的指针
    const_type c = values[0];   // const版本
}
```

### 4.2 auto与typeof(return)

```c
// C23 auto + C2y typeof(return)
auto process_data(void) {       // 返回类型推导
    auto result = fetch_data(); // 自动推导

    typeof(return) cached;      // 明确使用函数返回类型
    static _Bool has_cache = 0;

    if (!has_cache) {
        cached = result;
        has_cache = 1;
    }

    return cached;
}
```

---

## 5. 实际应用示例

### 5.1 日志包装器

```c
// C2y: 带日志的函数调用
#define LOG_CALL(func, ...) ({ \
    typeof(return) _start = clock(); \
    typeof(return) _result = func(__VA_ARGS__); \
    typeof(return) _end = clock(); \
    \
    printf("%s took %ld ms\n", #func, (_end - _start) * 1000 / CLOCKS_PER_SEC); \
    _result; \
})

// 使用
int heavy_computation(int n);

int logged_compute(int n) {
    return LOG_CALL(heavy_computation, n);
}
```

### 5.2 默认值处理

```c
// C2y: 带默认值的函数调用
typedef struct {
    typeof(return) value;
    _Bool is_valid;
} Optional;

#define OR_DEFAULT(func, default_val, ...) ({ \
    typeof(return) _result = func(__VA_ARGS__); \
    (_result == 0) ? (default_val) : _result; \
})

// 使用
int get_config(const char* key);

int get_config_or_default(const char* key, int default_val) {
    return OR_DEFAULT(get_config, default_val, key);
}
```

### 5.3 类型安全的状态机

```c
// C2y: 状态机返回值
enum State { STATE_IDLE, STATE_RUNNING, STATE_DONE };

typedef struct {
    enum State state;
    typeof(return) result;  // 与函数返回类型关联
} StateResult;

#define STATE_TRANSITION(func, ...) ({ \
    typeof(return) _res = func(__VA_ARGS__); \
    StateResult _sr = { STATE_RUNNING, _res }; \
    if (_res < 0) _sr.state = STATE_IDLE; \
    if (_res == 100) _sr.state = STATE_DONE; \
    _sr; \
})
```

---

## 6. 编译器支持

| 编译器 | 版本 | 支持状态 |
|:-------|:-----|:---------|
| GCC | - | 暂无 (提案阶段) |
| Clang | - | 暂无 (提案阶段) |
| MSVC | - | 未公布 |

**标准支持**: 预计C2y (2027-2028)

---

## 7. 与现有方案对比

### 7.1 替代方案

```c
// 方案1: 显式类型 (C89-C23)
int foo(void) {
    int result;  // 必须知道返回类型是int
    // ...
}

// 方案2: typeof表达式 (C23)
int foo(void) {
    typeof(foo()) result;  // 需要调用形式，非标准用法
    // ...
}

// 方案3: typeof(return) (C2y) - 最清晰
int foo(void) {
    typeof(return) result;  // 明确意图
    // ...
}
```

### 7.2 优势

```
可读性: typeof(return) > typeof(func()) > 显式类型
维护性: 返回类型修改时自动适应
意图表达: 明确表达"与返回类型相同"的语义
```

---

## 8. 注意事项

### 8.1 限制

```c
// 错误: 在函数外部使用
typeof(return) global_var;  // 错误! 不在函数内

// 错误: 在void函数中使用
void void_func(void) {
    typeof(return) x;  // 错误! void不是完整类型
}

// 正确: 有返回值函数内部使用
int int_func(void) {
    typeof(return) x;  // OK, int类型
    return x;
}
```

### 8.2 类型限定符

```c
// 返回类型带限定符
const volatile int qualified_func(void);

void test(void) {
    typeof(return) x;  // const volatile int 类型

    // 移除限定符
    typeof((int)0) y;  // int 类型
}
```

---

## 9. 参考

- **提案文档**: N3454 - typeof(return) (2025-02-26)
- **作者**: Alejandro Colomar
- **WG14日志**: <https://www.open-std.org/jtc1/sc22/wg14/www/wg14_document_log>
- **相关提案**: N3450 (typeof)

---

**状态**: 审议中
**最后更新**: 2026-03-24
