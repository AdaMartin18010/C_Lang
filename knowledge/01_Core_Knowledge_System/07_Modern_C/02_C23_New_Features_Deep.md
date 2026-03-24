# C23新特性深度解析

> **层级定位**: 01 Core Knowledge System / 07 Modern C
> **难度级别**: L3-L4
> **预估学习时间**: 4-6 小时

---

## 1. C23标准概览

### 1.1 C23主要改进

| 类别 | 特性 | 说明 |
|:-----|:-----|:-----|
| **类型安全** | nullptr | 类型安全的空指针 |
| | typeof/typeof_unqual | 类型推导 |
| | constexpr | 编译期常量 |
| **位运算** | stdbit.h | 标准位操作函数 |
| | _BitInt(N) | 任意宽度整数 |
| **字符串** | strdup/strndup | 标准字符串复制 |
| | strlcpy/strlcat | 安全字符串操作 |
| **属性** | [[attribute]] | 现代属性语法 |
| **预处理** | #embed | 嵌入二进制资源 |
| | **VA_OPT** | 可变参数改进 |

---

## 2. nullptr详解

### 2.1 为什么需要nullptr

```c
// C11及之前的问题
void foo(int *p);
void foo(int n);   // 重载（C++）或冲突

foo(NULL);         // 歧义！NULL可能是0

// C23解决方案
foo(nullptr);      // 明确是空指针，不是整数0
```

### 2.2 nullptr特性

```c
// nullptr是一个关键字
int *p1 = nullptr;      // OK
void *p2 = nullptr;     // OK
int n = nullptr;        // ERROR! 不能转换为整数

// 类型是nullptr_t
#include <stddef.h>
nullptr_t np = nullptr;

// 可以与任何指针比较
if (p == nullptr) { }   // OK
if (nullptr == nullptr) { }  // OK

// 不能进行算术运算
// nullptr + 1;  // ERROR!
// nullptr++;    // ERROR!
```

### 2.3 迁移建议

```c
// 旧代码
#define NULL ((void*)0)
int *p = NULL;

// C23推荐
int *p = nullptr;

// 兼容性宏
#if __STDC_VERSION__ >= 202311L
    // 使用nullptr
#else
    #define nullptr ((void*)0)  // 模拟
#endif
```

---

## 3. typeof详解

### 3.1 typeof使用

```c
// 推导表达式类型
int x = 10;
typeof(x) y = 20;        // y是int

typeof(int*) p1;
typeof(p1) p2;           // p2也是int*

// 复杂表达式
typeof(int[10]) arr;     // arr是int[10]类型

// 用于宏
#define max(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a > _b ? _a : _b; \
})

// 现在可以安全使用
int i = 5; double d = 3.14;
auto r = max(i, d);  // 推导为正确的类型
```

### 3.2 typeof_unqual

```c
// typeof保留限定符
const int ci = 10;
typeof(ci) x;            // x是const int

typeof_unqual(ci) y;     // y只是int，无const

// 在泛型编程中有用
#define clone(ptr) ((typeof_unqual(*ptr)*)malloc(sizeof(*ptr)))
```

---

## 4. constexpr详解

### 4.1 constexpr函数

```c
// C23 constexpr函数
constexpr int square(int x) {
    return x * x;
}

// 编译期计算
static_assert(square(5) == 25);  // 编译期验证

enum {
    SIZE = square(10)  // 编译期常量
};

// 限制：
// - 不能有副作用
// - 不能分配内存
// - 不能使用goto
// - 不能访问文件作用域变量
```

### 4.2 constexpr变量

```c
constexpr int max_size = 1024;
char buffer[max_size];   // OK，编译期已知

// 与const的区别
const int size1 = 100;       // 运行时常量（可能）
constexpr int size2 = 100;   // 编译期常量
```

---

## 5. stdbit.h详解

### 5.1 位计数函数

```c
#include <stdbit.h>

// 计算置位位数（population count）
unsigned int count_ones(uint32_t x) {
    return stdc_count_ones_ui(x);   // 硬件popcnt指令
}

// 前导零计数
unsigned int leading_zeros(uint32_t x) {
    return stdc_count_zeros_ui(x);
}

// 末尾零计数
unsigned int trailing_zeros(uint32_t x) {
    return stdc_count_trailing_zeros_ui(x);
}

// 最高置位位置
int bit_width(uint32_t x) {
    return stdc_bit_width_ui(x);  // 1 + floor(log2(x))
}

// 2的幂向上取整
uint32_t next_pow2(uint32_t x) {
    return stdc_bit_ceil_ui(x);
}
```

### 5.2 字节序操作

```c
#include <stdbit.h>

// 字节序转换
uint32_t be = 0x12345678;
uint32_t le = stdc_byteswap32(be);  // 0x78563412

// 编译器内置，高效实现
```

---

## 6. _BitInt详解

### 6.1 任意宽度整数

```c
// 定义128位整数
_BitInt(128) big_num;

// 定义1024位整数
_BitInt(1024) huge_num;

// 使用
_BitInt(128) a = 12345678901234567890wb;  // wb后缀
_BitInt(128) b = a * a;

// 范围
static_assert(stdc_leading_zeros((_BitInt(256))-1) == 0);
```

### 6.2 应用场景

```c
// 加密算法
_BitInt(256) private_key;
_BitInt(512) signature;

// 精确金融计算
_BitInt(96) decimal;  // 96位定点数

// 科学计算
_BitInt(2048) high_precision;
```

---

## 7. 属性语法

### 7.1 C23属性

```c
// 新语法
[[noreturn]] void exit(int status);
[[deprecated("use new_func instead")]] void old_func();

[[fallthrough]]  // switch case穿透
[[nodiscard]]    // 返回值不能忽略
[[maybe_unused]] // 可能未使用

// 函数属性
[[clang::always_inline]]
static inline int fast_func() { }

// 变量属性
[[gnu::aligned(64)]] char buffer[1024];

// 复合属性
[[gnu::hot, gnu::flatten]]
void critical_func() { }
```

---

## 8. #embed指令

### 8.1 嵌入二进制资源

```c
// 将文件内容嵌入为字节数组
const unsigned char shader_code[] = {
#embed "shader.spv"
};

// 带参数
const unsigned char icon[] = {
#embed "icon.png" prefix({0x89, 0x50}) suffix({0x0})
};

// 相当于
const unsigned char icon[] = {
    0x89, 0x50,
    /* 文件内容... */
    0x0
};
```

---

## 9. 字符串改进

### 9.1 strdup标准化

```c
#include <string.h>

// C23终于标准化了这些函数
char *strdup(const char *s);          // 分配副本
char *strndup(const char *s, size_t n);  // 限制长度

// 安全版本（来自BSD）
size_t strlcpy(char *dst, const char *src, size_t size);
size_t strlcat(char *dst, const char *src, size_t size);

// 使用示例
char *copy = strdup(original);
char limited[100];
strlcpy(limited, original, sizeof(limited));  // 总是以\0结尾
```

---

## 关联导航

### 前置知识

- [C11特性](./01_C11_Features.md)
- [现代C编程](./readme.md)

### 后续延伸

- [C23标准库](../../04_Standard_Library_Layer/C23_Standard_Library/readme.md)
- [位运算](../../01_Basic_Layer/06_Bit_Operations.md)

### 参考

- [C23标准草案](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf)
