# C 到 Zig 类型系统映射与形式化对应

> **定位**: 08_Zig_C_Connection / 类型系统 | **难度**: ⭐⭐⭐⭐ | **目标**: 建立形式化类型对应关系

---

## 目录

- [C 到 Zig 类型系统映射与形式化对应](#c-到-zig-类型系统映射与形式化对应)
  - [目录](#目录)
  - [一、类型映射总览](#一类型映射总览)
    - [1.1 基本类型对应表](#11-基本类型对应表)
    - [1.2 类型等价性定义](#12-类型等价性定义)
  - [二、整数类型](#二整数类型)
    - [2.1 固定宽度整数](#21-固定宽度整数)
    - [2.2 平台相关整数](#22-平台相关整数)
    - [2.3 形式化语义](#23-形式化语义)
  - [三、浮点类型](#三浮点类型)
    - [3.1 IEEE 754 映射](#31-ieee-754-映射)
    - [3.2 扩展精度](#32-扩展精度)
  - [四、指针类型](#四指针类型)
    - [4.1 指针类型对应](#41-指针类型对应)
    - [4.2 空指针语义](#42-空指针语义)
    - [4.3 函数指针](#43-函数指针)
  - [五、复合类型](#五复合类型)
    - [5.1 结构体](#51-结构体)
    - [5.2 联合体](#52-联合体)
    - [5.3 位域](#53-位域)
    - [5.4 数组](#54-数组)
  - [六、类型限定符](#六类型限定符)
    - [6.1 const 映射](#61-const-映射)
    - [6.2 volatile 映射](#62-volatile-映射)
    - [6.3 restrict 映射](#63-restrict-映射)
  - [七、枚举类型](#七枚举类型)
  - [八、类型别名](#八类型别名)
  - [九、形式化证明框架](#九形式化证明框架)
    - [9.1 布局等价性](#91-布局等价性)
    - [9.2 ABI 兼容性](#92-abi-兼容性)
  - [十、实用转换表](#十实用转换表)
    - [快速参考](#快速参考)
    - [常见模式](#常见模式)

---

## 一、类型映射总览

### 1.1 基本类型对应表

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                        C ↔ Zig 类型映射全景图                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  C Type Category          C Type              Zig Type           Equivalence │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  Fixed-width integers                                                      │
│  ─────────────────────────────────────────────────────────────────────────  │
│  int8_t                   int8_t              i8                 ≅          │
│  uint8_t                  uint8_t             u8                 ≅          │
│  int16_t                  int16_t             i16                ≅          │
│  uint16_t                 uint16_t            u16                ≅          │
│  int32_t                  int32_t             i32                ≅          │
│  uint32_t                 uint32_t            u32                ≅          │
│  int64_t                  int64_t             i64                ≅          │
│  uint64_t                 uint64_t            u64                ≅          │
│                                                                              │
│  Platform-dependent integers                                               │
│  ─────────────────────────────────────────────────────────────────────────  │
│  char                     char                u8 / i8            ≈          │
│  short                    short               c_short            ≈          │
│  int                      int                 c_int              ≈          │
│  long                     long                c_long             ≈          │
│  long long                long long           c_longlong         ≈          │
│  size_t                   size_t              usize              ≈          │
│  ptrdiff_t                ptrdiff_t           isize              ≈          │
│                                                                              │
│  Boolean                                                                   │
│  ─────────────────────────────────────────────────────────────────────────  │
│  bool (C23)               bool                bool               ≅          │
│  _Bool                    _Bool               bool               ≅          │
│                                                                              │
│  Floating-point                                                            │
│  ─────────────────────────────────────────────────────────────────────────  │
│  float                    float               f32                ≅          │
│  double                   double              f64                ≅          │
│  long double              long double         c_longdouble       ≈          │
│  _Float16                 _Float16            f16                ≅          │
│  _Float32                 _Float32            f32                ≅          │
│  _Float64                 _Float64            f64                ≅          │
│                                                                              │
│  Void                                                                      │
│  ─────────────────────────────────────────────────────────────────────────  │
│  void                     void                void               ≅          │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘

图例:
  ≅ : 同构 (isomorphic) - 完全等价，可安全转换
  ≈ : 近似 (approximately equivalent) - 平台相关，需谨慎
```

### 1.2 类型等价性定义

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                          类型等价性形式化定义                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定义 1: 布局等价 (Layout Equivalence)                                       │
│  ─────────────────────────────────────────                                   │
│  类型 T₁ 和 T₂ 布局等价，当且仅当:                                           │
│  ∀x: T₁, ∃y: T₂, sizeof(T₁) = sizeof(T₂) ∧ alignof(T₁) = alignof(T₂)        │
│  ∧ bit_pattern(x) = bit_pattern(y)                                          │
│                                                                              │
│  定义 2: 语义等价 (Semantic Equivalence)                                     │
│  ─────────────────────────────────────────                                   │
│  类型 T₁ 和 T₂ 语义等价，当且仅当:                                           │
│  T₁ ≅ T₂ ∧ 对 T₁ 的所有有效操作对 T₂ 同样有效且语义一致                      │
│                                                                              │
│  定义 3: ABI 等价 (ABI Equivalence)                                          │
│  ─────────────────────────────────                                           │
│  类型 T₁ 和 T₂ ABI 等价，当且仅当:                                           │
│  在函数调用约定中，T₁ 和 T₂ 使用相同的寄存器/栈位置传递                        │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 二、整数类型

### 2.1 固定宽度整数

```c
// C 代码
#include <stdint.h>

int8_t   i8  = -128;
uint8_t  u8  = 255;
int16_t  i16 = -32768;
uint16_t u16 = 65535;
int32_t  i32 = -2147483648;
uint32_t u32 = 4294967295;
int64_t  i64 = -9223372036854775807LL;
uint64_t u64 = 18446744073709551615ULL;
```

```zig
// Zig 代码
const i8: i8 = -128;
const u8: u8 = 255;
const i16: i16 = -32768;
const u16: u16 = 65535;
const i32: i32 = -2147483648;
const u32: u32 = 4294967295;
const i64: i64 = -9223372036854775807;
const u64: u64 = 18446744073709551615;
```

**形式化证明**:

```text
定理: ∀n ∈ {8, 16, 32, 64}, C int{n}_t ≅ Zig i{n} ∧ C uint{n}_t ≅ Zig u{n}

证明:
  1. 大小相等: sizeof(C int{n}_t) = sizeof(Zig i{n}) = n/8 bytes
  2. 对齐相等: alignof(C int{n}_t) = alignof(Zig i{n}) = n/8 bytes
  3. 值域相等: both ∈ [-2^{n-1}, 2^{n-1}-1]
  4. 操作等价: 算术运算、位运算语义一致
  ∴ 布局等价且语义等价 ∎
```

### 2.2 平台相关整数

```c
// C 代码 - 平台相关大小
char c = 'A';           // 通常 8 位，但可能为 16/32 位（罕见）
short s = 100;          // 至少 16 位
int i = 1000;           // 至少 16 位，通常为 32 位
long l = 10000L;        // 至少 32 位，LP64 上为 64 位
long long ll = 100000LL; // 至少 64 位
```

```zig
// Zig 代码 - 使用 c_ 前缀类型
const c: u8 = 'A';      // char 通常映射为 u8
const s: c_short = 100;      // 导入的 C short
const i: c_int = 1000;       // 导入的 C int
const l: c_long = 10000;     // 导入的 C long
const ll: c_longlong = 100000; // 导入的 C long long

// 注意: Zig 推荐使用固定宽度类型
const better_i: i32 = 1000;  // 明确的 32 位
```

**平台差异表**:

| 平台 | C int | C long | Zig c_int | Zig c_long | Zig c_longlong |
|------|-------|--------|-----------|------------|----------------|
| ILP32 (32位 Linux) | 32 | 32 | 32 | 32 | 64 |
| LP64 (64位 Linux/macOS) | 32 | 64 | 32 | 64 | 64 |
| LLP64 (Windows) | 32 | 32 | 32 | 32 | 64 |

### 2.3 形式化语义

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                      平台相关整数形式化语义                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  语义函数:                                                                   │
│  ─────────────────                                                           │
│  size_of_c_int : Platform → BitWidth                                         │
│  size_of_c_int(ILP32) = 32                                                   │
│  size_of_c_int(LP64)  = 32                                                   │
│  size_of_c_int(LLP64) = 32                                                   │
│                                                                              │
│  Zig 的解决方案:                                                             │
│  ─────────────────                                                           │
│  Zig c_int 在编译时根据目标平台确定大小                                        │
│  @sizeOf(c_int) = compile_time_eval(size_of_c_int(target_platform))         │
│                                                                              │
│  安全性分析:                                                                 │
│  ─────────────────                                                           │
│  C: int 可能为 16 或 32 位，跨平台代码易出错                                  │
│  Zig: 使用 i32 保证 32 位，使用 c_int 显式表达 C 兼容性需求                    │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 三、浮点类型

### 3.1 IEEE 754 映射

```c
// C 代码
float f = 3.14159f;           // IEEE 754 binary32
double d = 2.718281828;       // IEEE 754 binary64
```

```zig
// Zig 代码
const f: f32 = 3.14159;       // IEEE 754 binary32
const d: f64 = 2.718281828;   // IEEE 754 binary64
```

**等价性证明**:

```text
定理: C float ≅ Zig f32 ∧ C double ≅ Zig f64

证明:
  1. 都遵循 IEEE 754-2008 标准
  2. binary32: 1 位符号 + 8 位指数 + 23 位尾数
  3. binary64: 1 位符号 + 11 位指数 + 52 位尾数
  4. 特殊值 (NaN, Inf) 编码一致
  5. 舍入模式相同（默认就近偶舍入）
  ∴ 位模式等价，语义等价 ∎
```

### 3.2 扩展精度

```c
// C 代码 - 扩展精度平台相关
long double ld = 3.14159265358979323846L;
// x86: 80-bit extended precision
// ARM: 64-bit double precision 或 128-bit quadruple
// PowerPC: 64-bit 或 128-bit
```

```zig
// Zig 代码
const ld: c_longdouble = 3.14159265358979323846;
// 映射到平台的 long double

// 如果需要确定精度，使用:
const q: f128 = 3.14159265358979323846;  // IEEE 754 binary128
```

---

## 四、指针类型

### 4.1 指针类型对应

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                         C ↔ Zig 指针对应表                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  C Pointer                Zig Pointer              Notes                     │
│  ────────────────────────────────────────────────────────────────────────   │
│                                                                              │
│  int*                     *i32                     单元素指针               │
│  const int*               *const i32               指向不可变数据           │
│  int* const               *i32 (可变绑定)          Zig 中绑定默认可变        │
│  const int* const         *const i32               双重 const              │
│                                                                              │
│  void*                    ?*anyopaque              通用指针，可为空         │
│  const void*              ?*const anyopaque        只读通用指针             │
│                                                                              │
│  int[]                    [*]i32                   多元素指针（长度未知）    │
│  int[N]                   *[N]i32                  指向 N 个元素            │
│                                                                              │
│  char*                    [*c]u8                   C 风格字符串            │
│  char[]                   []u8                     Zig 切片（胖指针）        │
│                                                                              │
│  int**                    **i32                    多级指针                │
│  int (*arr)[10]           *[10]i32                 指向数组                │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 4.2 空指针语义

```c
// C23 代码
#include <stddef.h>

int* p1 = NULL;        // 可能为 0 或 (void*)0
int* p2 = nullptr;     // C23: 类型安全的空指针
bool is_null = (p2 == nullptr);
```

```zig
// Zig 代码
var p1: ?*i32 = null;           // 可选指针，可为 null
var p2: *i32 = undefined;       // 非空指针，必须初始化

// 空值检查
if (p1) |value| {
    // p1 非空，value 是 *i32
} else {
    // p1 为空
}

// 解包（运行时检查）
const value = p1.?;  // 如果 p1 为空则 panic

// 带默认值的解包
const value = p1 orelse &default_value;
```

**语义对比**:

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                        空指针语义对比                                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  特性                     C (NULL)        C23 (nullptr)    Zig (?*T)        │
│  ────────────────────────────────────────────────────────────────────────   │
│  类型                     int/void*       nullptr_t          ?*T            │
│  可转整数                 ✅               ❌                 ❌              │
│  编译时检查               ❌               部分              ✅              │
│  运行时检查               ❌               ❌                 可选 (orelse)   │
│  解引用前必须检查         否               否                是              │
│                                                                              │
│  Zig 优势:                                                                 │
│  - 可选类型系统强制检查空指针                                                │
│  - 编译时知道指针是否可能为空                                                │
│  - `orelse` 提供优雅的空值处理                                               │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 4.3 函数指针

```c
// C 代码
// 函数指针类型
typedef int (*binary_op)(int, int);

// 函数接受函数指针
void apply(int* arr, size_t n, binary_op op) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = op(arr[i], i);
    }
}

// 函数返回函数指针
binary_op get_operator(char op);
```

```zig
// Zig 代码
// 函数指针类型
const BinaryOp = *const fn (i32, i32) callconv(.C) i32;

// 函数接受函数指针
fn apply(arr: []i32, op: BinaryOp) void {
    for (arr, 0..) |*item, i| {
        item.* = op(item.*, @intCast(i));
    }
}

// 函数返回函数指针
fn getOperator(op: u8) ?BinaryOp {
    return switch (op) {
        '+' => add,
        '-' => sub,
        else => null,
    };
}

fn add(a: i32, b: i32) callconv(.C) i32 { return a + b; }
fn sub(a: i32, b: i32) callconv(.C) i32 { return a - b; }
```

**关键区别**:

- Zig 函数指针必须显式指定调用约定 (`callconv(.C)`)
- Zig 使用 `?*const fn` 表示可能为空的函数指针
- Zig 不支持可变参数函数指针（需使用 C 互操作）

---

## 五、复合类型

### 5.1 结构体

```c
// C 代码
struct Point {
    int x;
    int y;
};

// 带 packed 的结构体
struct __attribute__((packed)) PackedPoint {
    int x;
    char c;
    int y;  // 无填充
};

// 匿名结构体成员 (C11)
struct Wrapper {
    struct {
        int a;
        int b;
    };
    int c;
};
```

```zig
// Zig 代码
// extern struct 保证 C 兼容布局
const Point = extern struct {
    x: i32,
    y: i32,
};

// packed struct
const PackedPoint = packed struct {
    x: i32,
    c: u8,
    y: i32,  // 无填充
};

// 普通 Zig struct（不保证 C 兼容）
const ZigPoint = struct {
    x: i32,
    y: i32,
};

// 注意: Zig 不支持匿名结构体成员
```

**布局等价性**:

```text
定理: C struct Point ≅ Zig extern struct Point

条件:
  1. 字段顺序相同
  2. 每个对应字段类型布局等价
  3. 使用相同的对齐方式（默认或显式 packed）

证明:
  sizeof(C struct Point) = sizeof(i32) + sizeof(i32) + padding
                       = 4 + 4 + 0 = 8
  sizeof(Zig extern struct Point) = 8

  offsetof(C, x) = offsetof(Zig, x) = 0
  offsetof(C, y) = offsetof(Zig, y) = 4
  ∴ 布局等价 ∎
```

### 5.2 联合体

```c
// C 代码
union Data {
    int i;
    float f;
    char c[4];
};

// 带标记的联合体（手动实现）
struct TaggedData {
    enum { INT, FLOAT, BYTES } tag;
    union Data data;
};
```

```zig
// Zig 代码
const Data = extern union {
    i: i32,
    f: f32,
    c: [4]u8,
};

// Zig 提供带标签的联合体（ tagged union）
const TaggedData = union(enum) {
    int: i32,
    float: f32,
    bytes: [4]u8,
};

// 使用
data: TaggedData = .{ .int = 42 };
switch (data) {
    .int => |i| std.debug.print("int: {d}\n", .{i}),
    .float => |f| std.debug.print("float: {d}\n", .{f}),
    .bytes => |b| std.debug.print("bytes: {x}\n", .{b}),
}
```

**关键差异**:

- Zig `extern union` 保证 C 兼容
- Zig `union(enum)` 提供类型安全的 tagged union，C 需要手动实现

### 5.3 位域

```c
// C 代码
struct Flags {
    unsigned int flag1 : 1;
    unsigned int flag2 : 1;
    unsigned int value : 6;
};
```

```zig
// Zig 代码 - 使用 packed struct 模拟
const Flags = packed struct {
    flag1: u1,
    flag2: u1,
    value: u6,
};

// 注意: Zig 的 packed struct 字段从最低有效位开始
// C 的位域布局是实现定义的
```

**警告**: C 位域布局高度依赖实现，不推荐在 C/Zig 边界使用。

### 5.4 数组

```c
// C 代码
int arr[10];                    // 固定大小数组
int* dynamic = malloc(10 * sizeof(int));  // 动态数组
int matrix[3][4];               // 多维数组
```

```zig
// Zig 代码
var arr: [10]i32 = undefined;   // 固定大小数组
var dynamic: []i32 = allocator.alloc(i32, 10) catch unreachable;  // 切片
var matrix: [3][4]i32 = undefined;  // 多维数组

// 关键区别: Zig 切片是胖指针 [ptr, len]
// C 数组退化为指针，丢失长度信息
```

---

## 六、类型限定符

### 6.1 const 映射

```c
// C 代码
const int x = 10;           // x 不可修改
int* const p = &x;          // p 不可修改，但 *p 可以
const int* const q = &x;    // q 不可修改，*q 也不可
```

```zig
// Zig 代码
const x: i32 = 10;          // x 是编译时常量（不是 const 限定）
var p: *i32 = &x;           // 错误！x 的地址在 comptime

// 正确的 const 语义
var y: i32 = 10;
const p1: *i32 = &y;        // p1 不可重新绑定，但 *p1 可以修改
const p2: *const i32 = &y;  // p2 不可重新绑定，*p2 也不可修改
```

**语义差异**:

- C `const` 是类型限定符
- Zig `const` 是绑定修饰符（类似 C++ `const` 引用）
- Zig `*const T` 等价于 C `const T*`

### 6.2 volatile 映射

```c
// C 代码
volatile int status_reg;    // 每次访问都从内存读取
```

```zig
// Zig 代码
var status_reg: volatile i32 = undefined;
```

### 6.3 restrict 映射

```c
// C 代码
void add(int* restrict a, int* restrict b, int n) {
    // 编译器可假设 a 和 b 指向不重叠的内存
    for (int i = 0; i < n; i++) {
        a[i] += b[i];
    }
}
```

```zig
// Zig 代码 - 无直接对应
// Zig 使用切片语义自动获得 restrict 优化机会
fn add(a: []i32, b: []const i32) void {
    // 编译器知道 a 和 b 不重叠（因为切片是借用）
    for (a, b) |*pa, pb| {
        pa.* += pb;
    }
}
```

---

## 七、枚举类型

```c
// C 代码
typedef enum {
    RED = 0,
    GREEN = 1,
    BLUE = 2
} Color;

// 指定底层类型 (C23)
typedef enum : uint8_t {
    SMALL_RED,
    SMALL_GREEN,
} SmallColor;
```

```zig
// Zig 代码
const Color = enum {
    red,
    green,
    blue,
};

// 指定底层类型
const SmallColor = enum(u8) {
    red,
    green,
};

// 显式指定值
const HttpStatus = enum(u16) {
    ok = 200,
    not_found = 404,
    server_error = 500,
};
```

**关键差异**:

- C 枚举值在全局命名空间
- Zig 枚举值在类型命名空间（`Color.red`）
- C 枚举底层类型通常是 `int`
- Zig 枚举默认 `u32`，可显式指定

---

## 八、类型别名

```c
// C 代码
typedef unsigned int uint;
typedef struct Node Node;

// C23 typeof
typedef typeof(int) my_int;
typedef typeof_unqual(const int) plain_int;
```

```zig
// Zig 代码
const Uint = u32;  // 直接类型别名

// 复杂类型别名
const String = []const u8;
const IntPtr = *i32;

// 函数类型别名
const Callback = *const fn (i32) void;

// 注意: Zig 没有 typedef，直接使用 const
```

---

## 九、形式化证明框架

### 9.1 布局等价性

```coq
(* Coq 风格的形式化证明框架 *)

Require Import ZArith.

(* 类型表示 *)
Inductive CType : Type :=
  | CInt : Z -> CType          (* 位宽 *)
  | CFloat : Z -> CType
  | CPointer : CType -> CType
  | CStruct : list (string * CType) -> CType
  | CUnion : list (string * CType) -> CType
  | CArray : Z -> CType -> CType.

Inductive ZigType : Type :=
  | ZInt : bool -> Z -> ZigType   (* 有符号? 位宽 *)
  | ZFloat : Z -> ZigType
  | ZPointer : bool -> ZigType -> ZigType  (* 可选? *)
  | ZStruct : bool -> list (string * ZigType) -> ZigType  (* extern? *)
  | ZUnion : bool -> list (string * ZigType) -> ZigType
  | ZArray : Z -> ZigType -> ZigType.

(* 布局计算 *)
Fixpoint c_sizeof (t: CType) : Z := ...
Fixpoint zig_sizeof (t: ZigType) : Z := ...

(* 等价关系 *)
Inductive TypeEquiv : CType -> ZigType -> Prop :=
  | EquivInt : forall s n,
      s = true ->  (* C int 有符号 *)
      TypeEquiv (CInt n) (ZInt s n)
  | EquivPointer : forall ct zt,
      TypeEquiv ct zt ->
      TypeEquiv (CPointer ct) (ZPointer false zt)
  | EquivStruct : forall fields cfields zfields,
      Forall2 (fun cf zf => fst cf = fst zf /\ TypeEquiv (snd cf) (snd zf))
              cfields zfields ->
      TypeEquiv (CStruct cfields) (ZStruct true zfields).

(* 等价性定理 *)
Theorem layout_equivalence :
  forall ct zt,
    TypeEquiv ct zt ->
    c_sizeof ct = zig_sizeof zt /\
    c_alignof ct = zig_alignof zt.
Proof.
  (* 归纳证明 *)
Admitted.
```

### 9.2 ABI 兼容性

```coq
(* 调用约定兼容性 *)

(* 参数传递分类 *)
Inductive ArgClass :=
  | IntegerReg          (* 整型寄存器 *)
  | SSEReg              (* SSE 寄存器 *)
  | SSEUp               (* SSE 寄存器上部 *)
  | X87                 (* x87 FPU *)
  | X87Up
  | ComplexX87
  | Memory.             (* 栈内存 *)

(* System V AMD64 ABI 分类 *)
Fixpoint classify_c_type (t: CType) : list ArgClass := ...
Fixpoint classify_zig_type (t: ZigType) : list ArgClass := ...

(* ABI 等价性 *)
Theorem abi_equivalence :
  forall ct zt,
    TypeEquiv ct zt ->
    classify_c_type ct = classify_zig_type zt.
Proof.
  (* 证明：等价的类型在 System V AMD64 ABI 下有相同的分类 *)
Admitted.
```

---

## 十、实用转换表

### 快速参考

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                      C → Zig 快速转换参考                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  C                              Zig                                          │
│  ────────────────────────────────────────────────────────────────────────   │
│  int8_t                         i8                                           │
│  uint8_t                        u8                                           │
│  int16_t                        i16                                          │
│  uint16_t                       u16                                          │
│  int32_t                        i32                                          │
│  uint32_t                       u32                                          │
│  int64_t                        i64                                          │
│  uint64_t                       u64                                          │
│  size_t                         usize                                        │
│  ptrdiff_t                      isize                                        │
│  intptr_t                       isize                                        │
│  uintptr_t                      usize                                        │
│  char                           u8 (或 i8)                                   │
│  unsigned char                  u8                                           │
│  short                          c_short                                      │
│  unsigned short                 c_ushort                                     │
│  int                            c_int                                        │
│  unsigned int                   c_uint                                       │
│  long                           c_long                                       │
│  unsigned long                  c_ulong                                      │
│  long long                      c_longlong                                   │
│  unsigned long long             c_ulonglong                                  │
│  float                          f32                                          │
│  double                         f64                                          │
│  bool                           bool                                         │
│  void                           void                                         │
│                                                                              │
│  T*                             *T 或 ?*T                                    │
│  const T*                       *const T                                     │
│  T* const                       *T (绑定不可变)                              │
│  void*                          ?*anyopaque                                  │
│  void (*f)(T)                   ?*const fn (T) void                          │
│  T (*f)(U)                      *const fn (U) T                              │
│  T[]                            [*]T (C 风格) 或 []T (Zig 切片)              │
│  T[N]                           [N]T (数组) 或 *[N]T (指针)                  │
│  T[N][M]                        [N][M]T                                      │
│  struct S {...}                 extern struct S {...}                        │
│  union U {...}                  extern union U {...}                         │
│  enum E {...}                   const E = enum {...}                         │
│  enum : T {...}                 const E = enum(T) {...}                      │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 常见模式

```c
// C 模式 → Zig 模式

// 1. 字符串
const char* str = "hello";              // C
const str: [*:0]const u8 = "hello";     // Zig C 互操作
const str: []const u8 = "hello";        // Zig 惯用法

// 2. 动态数组
int* arr = malloc(n * sizeof(int));     // C
var arr: []i32 = allocator.alloc(i32, n) catch unreachable;  // Zig

// 3. 回调函数
typedef void (*callback_t)(int, void*);
const Callback = *const fn (i32, ?*anyopaque) callconv(.C) void;

// 4. 不透明指针
typedef struct Handle Handle;
const Handle = opaque {};
```

---

> **文档状态**: 核心内容完成 | **验证状态**: 理论框架 | **最后更新**: 2026-03-12
