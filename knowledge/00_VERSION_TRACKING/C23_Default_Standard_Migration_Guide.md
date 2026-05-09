# GCC 15 C23 默认标准迁移指南

> **文档版本**: 1.0.0
> **最后更新**: 2026-05-10
> **适用场景**: 从 GCC 14 (C17默认) 迁移到 GCC 15+ (C23默认)

---

## 概述

GCC 15 将C编译的默认标准从 `-std=gnu17` 改为 `-std=gnu23`。这是C语言50年历史上**默认标准的最大跃迁**，可能导致现有代码在升级GCC后**静默编译失败**或**行为变更**。

---

## 破坏性变更清单

### 1. `bool` / `true` / `false` 成为真正关键字

**影响等级**: 🔴 高

C23之前，`bool`/`true`/`false` 是 `<stdbool.h>` 中的宏。C23中它们成为**真正关键字**，不能再用作标识符。

```c
// ❌ GCC 15+ 编译失败
int bool = 1;      // error: 'bool' is a keyword
int true = 1;      // error: 'true' is a keyword
void false(void) {} // error: 'false' is a keyword

// ✅ 需要重命名
int bool_flag = 1;
int true_value = 1;
void false_handler(void) {}
```

**检测脚本**:

```bash
grep -rn '\bbool\b\|\btrue\b\|\bfalse\b' --include='*.c' --include='*.h' src/
```

### 2. 空参数列表 `()` 语义变更

**影响等级**: 🟡 中

C23之前，`int func()` 表示"参数未指定"（接受任意参数）。C23中它明确表示"零参数"。

```c
// C17: 接受任意参数（K&R遗留）
// C23: 零参数，与 int func(void) 等价
int func();

// ✅ 最安全的写法：显式使用 (void)
int func(void);
```

**注意**：虽然语义变化在大多数代码中不会导致运行时问题，但某些依赖旧语义的代码（如可变参数包装器）可能需要审查。

### 3. `INFINITY` / `NAN` 从 `<math.h>` 迁移到 `<float.h>`

**影响等级**: 🟢 低

C23中，`INFINITY` 和 `NAN` 宏正式定义在 `<float.h>` 中，在 `<math.h>` 中标记为**已废弃**。

```c
// ⚠️ C23 中仍可用，但可能产生废弃警告
#include <math.h>
double x = INFINITY;

// ✅ C23 推荐写法
#include <float.h>
double x = INFINITY;
```

### 4. `constexpr` 变量语法

**影响等级**: 🟢 低

C23新增 `constexpr` 关键字用于编译期常量对象。与C++不同，C23的 `constexpr` **不能用于函数**。

```c
// ✅ C23: constexpr 对象
constexpr int max_size = 1024;
int buffer[max_size];  // 编译期确定的VLA替代

// ❌ C23: 不支持 constexpr 函数（这是C2y提案）
// constexpr int square(int x) { return x * x; }  // 错误！
```

### 5. 隐式函数声明被移除

**影响等级**: 🔴 高（遗留代码）

C23正式移除了隐式函数声明。任何未声明就调用的函数都会导致**约束违规**。

```c
// ❌ C23: 编译错误
int main() {
    foo();  // error: implicit declaration of function 'foo'
}

// ✅ 必须显式声明或包含头文件
void foo(void);
int main() {
    foo();
}
```

---

## 迁移策略

### 策略A：显式锁定旧标准（快速兼容）

```makefile
## Makefile
CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra  # 锁定C17，拒绝C23变更
```

**适用场景**：

- 遗留代码库，无资源全面审查
- 安全关键系统，需最小化变更
- 短期过渡方案

### 策略B：逐步迁移到C23（推荐）

```makefile
## 阶段1：启用C23兼容警告
CFLAGS = -std=gnu17 -Wc23-c2y-compat

## 阶段2：切换到C23，但允许GNU扩展
CFLAGS = -std=gnu23 -Wall -Wextra

## 阶段3：严格C23（无GNU扩展）
CFLAGS = -std=c23 -Wall -Wextra -pedantic
```

### 策略C：CI/CD多版本测试

```yaml
## .github/workflows/ci.yml
strategy:
  matrix:
    std: [c17, c23, c2y]
    compiler: [gcc-14, gcc-15, gcc-16, clang-20]
steps:
  - run: ${{ matrix.compiler }} -std=${{ matrix.std }} -Werror -c src/*.c
```

---

## 编译器版本矩阵

| 编译器 | C17默认 | C23默认 | 切换版本 |
|--------|---------|---------|---------|
| GCC | ≤14 | 15+ | GCC 15 (2025-04) |
| Clang | ≤19 | 20+? | 尚未切换 |
| MSVC | 始终部分 | 无计划 | N/A |

---

## 参考

- [GCC 15 Porting Guide](https://gcc.gnu.org/gcc-15/porting-to.html)
- [C23 Standard (ISO/IEC 9899:2024)](https://www.iso.org/standard/83616.html)
- [C23/C2y Support Matrix](../05_C_Language_Standards/C23_C2Y_SUPPORT_MATRIX.md)
