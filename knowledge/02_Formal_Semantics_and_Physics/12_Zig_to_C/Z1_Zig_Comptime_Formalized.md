# Zig编译时计算：comptime的形式化语义

> **层级**: L6+
> **引用**: Zig Documentation, TDT (Turing Degree Theory), CTM (Comptime Metaprogramming)

---

## 【思维导图·Zig comptime】

```
Zig comptime
├── 编译时执行
│   ├── comptime关键字
│   ├── 类型作为值
│   └── 编译时循环
├── 泛型编程
│   ├── 类型参数
│   ├── 编译时多态
│   └── 代码生成
└── 元编程
    ├── comptime反射
    ├── 代码生成器
    └── 条件编译
```

---

## 【科学·comptime的形式化】

### 1. 两阶段计算模型

**定义**: Zig程序分两阶段执行

- **阶段1 (编译期)**: comptime代码执行
- **阶段2 (运行时)**: 生成的代码执行

**形式化**:

```
Program ::= (ComptimePhase, RuntimePhase)

ComptimePhase: Source → (Types, Constants, GeneratedCode)
RuntimePhase: GeneratedCode → IO
```

### 2. comptime的λ演算扩展

**语法**:

```
t ::= x | λx.t | t₁ t₂ | comptime t | type
```

**语义**:

- comptime t 在编译期求值
- type 是类型宇宙

**规约规则**:

```
comptime v → v (如果是值)
comptime (λx.t) v → comptime t[v/x]
```

### 3. 类型作为一等公民

**定理**: 在comptime上下文中，类型是值。

**形式化**:

```
Γ ⊢ comptime : type → value
```

**示例**:

```zig
fn ArrayList(comptime T: type) type {
  return struct {
    items: []T,
    capacity: usize,
  };
}
// ArrayList(i32) 在编译时生成类型
```

---

## 【工程·Zig到C编译】

### 1. comptime求值器

**实现**: Zig编译器内置解释器

**流程**:

```
Zig源码
  ↓
解析 → AST
  ↓
comptime求值 → 常量折叠、类型生成
  ↓
单态化 → 具体类型实例
  ↓
生成C代码
```

### 2. 泛型单态化

**Rust/Lean**: 泛型保持到编译后期
**Zig**: 编译期完全单态化

**生成的C**:

```c
// Zig: ArrayList(i32)
// C:
typedef struct {
  int32_t* items;
  size_t len;
  size_t capacity;
} ArrayList_i32;

// Zig: ArrayList(f64)
// C:
typedef struct {
  double* items;
  size_t len;
  size_t capacity;
} ArrayList_f64;
```

### 3. 编译时循环展开

**Zig**:

```zig
comptime var i = 0;
comptime while (i < 3) : (i += 1) {
  // 这段代码生成3次
}
```

**生成的C**:

```c
// 展开为:
// 第1次迭代代码
// 第2次迭代代码
// 第3次迭代代码
```

---

## 【多维矩阵·编译时能力对比】

```
语言        编译时执行    类型计算    代码生成    复杂度
─────────────────────────────────────────────────────────
Zig         完整         一等        结构体      中
Rust        常量求值      部分        宏          高
C++         模板          复杂        模板实例     极高
Lean 4      完整         依赖类型     提取        高
D           CTFE         部分        CTFE        中
```

---

## 【定理·comptime终止性】

**定理**: comptime代码必须终止。

**证明**: 如果comptime代码不终止，编译将无限进行，无法生成可执行文件。

**推论**: comptime代码不能包含：

- 无限循环（除非有break）
- 无界递归（除非有基本情况）
- 外部IO（阻塞操作）

---

## 参考

- Zig Language Reference
- Kelley, A. "Zig: A Programming Language Designed for Robustness" (2019)
- The D Language: Compile Time Function Execution
