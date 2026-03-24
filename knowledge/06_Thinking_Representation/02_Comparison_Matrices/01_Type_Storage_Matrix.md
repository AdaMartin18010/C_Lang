# C类型存储对比矩阵


---

## 📑 目录

- [C类型存储对比矩阵](#c类型存储对比矩阵)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [基本数据类型大小和对齐](#基本数据类型大小和对齐)
    - [C17标准保证的最小范围](#c17标准保证的最小范围)
    - [实际平台对比](#实际平台对比)
    - [对齐要求获取](#对齐要求获取)
  - [不同平台对比矩阵](#不同平台对比矩阵)
    - [32位 vs 64位对比](#32位-vs-64位对比)
    - [x86 vs ARM 对比](#x86-vs-arm-对比)
  - [结构体内存布局和对齐规则](#结构体内存布局和对齐规则)
    - [对齐规则](#对齐规则)
    - [结构体内存分析工具](#结构体内存分析工具)
    - [内存布局对比表](#内存布局对比表)
  - [位域和 packed 结构](#位域和-packed-结构)
    - [位域(Bit Fields)](#位域bit-fields)
    - [Packed 结构体](#packed-结构体)
  - [对比矩阵表格](#对比矩阵表格)
    - [完整类型对比矩阵](#完整类型对比矩阵)
    - [结构体内存布局对比](#结构体内存布局对比)
  - [C代码: 类型信息打印、结构体布局分析](#c代码-类型信息打印结构体布局分析)
  - [编译和运行](#编译和运行)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 概述

C语言数据类型的存储特性直接影响内存使用效率和程序性能。
理解不同平台下类型的大小、对齐要求以及结构体内存布局是编写高效、可移植C代码的基础。

## 基本数据类型大小和对齐

### C17标准保证的最小范围

| 类型 | 最小大小(位) | 最小范围 | 格式符 |
|------|-------------|----------|--------|
| `char` | 8 | -128 ~ 127 或 0 ~ 255 | `%c`或`%d` |
| `short` | 16 | -32768 ~ 32767 | `%hd` |
| `int` | 16 | -32768 ~ 32767 | `%d` |
| `long` | 32 | -2147483648 ~ 2147483647 | `%ld` |
| `long long` | 64 | -(2^63) ~ (2^63-1) | `%lld` |
| `float` | 32 | IEEE 754 single precision | `%f` |
| `double` | 64 | IEEE 754 double precision | `%lf` |
| `long double` | 64+ | 扩展精度 | `%Lf` |

### 实际平台对比

```c
// type_sizes.c - 打印各平台类型大小
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// 平台检测宏
#if defined(__x86_64__) || defined(_M_X64)
    #define PLATFORM "x86-64"
#elif defined(__i386__) || defined(_M_IX86)
    #define PLATFORM "x86-32"
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define PLATFORM "ARM64"
#elif defined(__arm__) || defined(_M_ARM)
    #define PLATFORM "ARM32"
#else
    #define PLATFORM "Unknown"
#endif

// 编译器检测
#if defined(__GNUC__)
    #define COMPILER "GCC " __VERSION__
#elif defined(__clang__)
    #define COMPILER "Clang"
#elif defined(_MSC_VER)
    #define COMPILER "MSVC"
#else
    #define COMPILER "Unknown"
#endif

int main(void) {
    printf("========================================\n");
    printf("Platform: %s\n", PLATFORM);
    printf("Compiler: %s\n", COMPILER);
    printf("========================================\n\n");

    printf("=== 整数类型 ===\n");
    printf("sizeof(char)      = %zu bytes (%zu bits)\n",
           sizeof(char), sizeof(char) * 8);
    printf("sizeof(short)     = %zu bytes (%zu bits)\n",
           sizeof(short), sizeof(short) * 8);
    printf("sizeof(int)       = %zu bytes (%zu bits)\n",
           sizeof(int), sizeof(int) * 8);
    printf("sizeof(long)      = %zu bytes (%zu bits)\n",
           sizeof(long), sizeof(long) * 8);
    printf("sizeof(long long) = %zu bytes (%zu bits)\n",
           sizeof(long long), sizeof(long long) * 8);
    printf("sizeof(size_t)    = %zu bytes (%zu bits)\n",
           sizeof(size_t), sizeof(size_t) * 8);
    printf("sizeof(ptrdiff_t) = %zu bytes (%zu bits)\n",
           sizeof(ptrdiff_t), sizeof(ptrdiff_t) * 8);

    printf("\n=== 固定宽度整数类型 (stdint.h) ===\n");
    printf("sizeof(int8_t)    = %zu bytes\n", sizeof(int8_t));
    printf("sizeof(int16_t)   = %zu bytes\n", sizeof(int16_t));
    printf("sizeof(int32_t)   = %zu bytes\n", sizeof(int32_t));
    printf("sizeof(int64_t)   = %zu bytes\n", sizeof(int64_t));
    printf("sizeof(uintptr_t) = %zu bytes\n", sizeof(uintptr_t));
    printf("sizeof(intptr_t)  = %zu bytes\n", sizeof(intptr_t));

    printf("\n=== 浮点类型 ===\n");
    printf("sizeof(float)       = %zu bytes (%zu bits)\n",
           sizeof(float), sizeof(float) * 8);
    printf("sizeof(double)      = %zu bytes (%zu bits)\n",
           sizeof(double), sizeof(double) * 8);
    printf("sizeof(long double) = %zu bytes (%zu bits)\n",
           sizeof(long double), sizeof(long double) * 8);

    printf("\n=== 指针类型 ===\n");
    printf("sizeof(void*)       = %zu bytes\n", sizeof(void*));
    printf("sizeof(char*)       = %zu bytes\n", sizeof(char*));
    printf("sizeof(int*)        = %zu bytes\n", sizeof(int*));
    printf("sizeof(double*)     = %zu bytes\n", sizeof(double*));
    printf("sizeof(void(*)())   = %zu bytes\n", sizeof(void(*)(void)));

    printf("\n=== 其他类型 ===\n");
    printf("sizeof(bool)        = %zu bytes (stdbool.h)\n", sizeof(bool));
    printf("sizeof(wchar_t)     = %zu bytes\n", sizeof(wchar_t));

    return 0;
}
```

### 对齐要求获取

```c
// alignment.c - 类型对齐查询
#include <stdio.h>
#include <stdalign.h>

// C11方式获取对齐值
#define PRINT_ALIGNMENT(type) \
    printf("alignof(%-12s) = %zu\n", #type, alignof(type))

// 传统方式: 使用offsetof技巧
#define ALIGNOF_TYPE(type) \
    offsetof(struct { char c; type member; }, member)

int main(void) {
    printf("=== C11 alignof 值 ===\n");
    PRINT_ALIGNMENT(char);
    PRINT_ALIGNMENT(short);
    PRINT_ALIGNMENT(int);
    PRINT_ALIGNMENT(long);
    PRINT_ALIGNMENT(long long);
    PRINT_ALIGNMENT(float);
    PRINT_ALIGNMENT(double);
    PRINT_ALIGNMENT(long double);
    PRINT_ALIGNMENT(void*);

    return 0;
}
```

## 不同平台对比矩阵

### 32位 vs 64位对比

| 类型 | ILP32 (32位) | LP64 (64位Linux/Unix) | LLP64 (64位Windows) |
|------|--------------|----------------------|---------------------|
| `char` | 1 | 1 | 1 |
| `short` | 2 | 2 | 2 |
| `int` | 4 | 4 | 4 |
| `long` | 4 | 8 | 4 |
| `long long` | 8 | 8 | 8 |
| `size_t` | 4 | 8 | 8 |
| `void*` | 4 | 8 | 8 |

### x86 vs ARM 对比

| 特性 | x86/x86-64 | ARM/ARM64 |
|------|-----------|-----------|
| 对齐要求 | 宽松(硬件处理) | 严格(可能崩溃) |
| `long double` | 80-bit (x87) | 128-bit (软件)或64-bit |
| 字节序 | 小端 | 通常小端(可配置) |
| 结构体对齐 | 通常4/8 | 通常4/8 |

```c
// platform_differences.c
#include <stdio.h>
#include <stdint.h>

// 字节序检测
int is_little_endian(void) {
    union {
        uint16_t val;
        uint8_t bytes[2];
    } test = {0x0102};
    return test.bytes[0] == 0x02;
}

// 对齐检测
typedef struct {
    char c;
    int i;
} test_align_t;

int main(void) {
    printf("=== 平台特性检测 ===\n");
    printf("字节序: %s\n", is_little_endian() ? "小端" : "大端");
    printf("char + int 结构体大小: %zu\n", sizeof(test_align_t));
    printf("预期对齐填充: %zu bytes\n", sizeof(test_align_t) - sizeof(char) - sizeof(int));

    return 0;
}
```

## 结构体内存布局和对齐规则

### 对齐规则

```
结构体内存布局规则:

1. 结构体成员按声明顺序分配
2. 每个成员的偏移量必须是该成员大小的整数倍
3. 结构体总大小必须是最大对齐成员的整数倍
4. 对齐值 = min(成员大小, 平台最大对齐值)
```

### 结构体内存分析工具

```c
// struct_layout.c - 结构体内存布局可视化
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

// 打印结构体布局的宏
#define PRINT_STRUCT_LAYOUT(type) do { \
    printf("\n=== %s 布局 ===\n", #type); \
    printf("总大小: %zu bytes\n", sizeof(type)); \
    printf("对齐要求: %zu bytes\n", alignof(type)); \
} while(0)

#define PRINT_MEMBER_OFFSET(type, member) \
    printf("  offsetof(%s, %s) = %zu\n", #type, #member, offsetof(type, member))

// 示例结构体
struct Simple {
    char a;    // 1 byte
    int b;     // 4 bytes
    char c;    // 1 byte
};

struct Optimized {
    int b;     // 4 bytes
    char a;    // 1 byte
    char c;    // 1 byte
};

struct Complex {
    char a;           // 1 byte
    double d;         // 8 bytes
    int i;            // 4 bytes
    short s;          // 2 bytes
    char b;           // 1 byte
};

struct ComplexOptimized {
    double d;         // 8 bytes
    int i;            // 4 bytes
    short s;          // 2 bytes
    char a;           // 1 byte
    char b;           // 1 byte
};

// 包含数组的结构体
struct WithArray {
    int id;
    char name[17];    // 17 bytes (不是4的倍数)
    double score;
};

// 嵌套结构体
struct NestedOuter {
    int a;
    struct Simple inner;
    char b;
};

// 可视化内存布局
void visualize_struct_simple(void) {
    struct Simple s = {'A', 0x12345678, 'B'};
    unsigned char *p = (unsigned char *)&s;

    printf("\n=== Simple 内存 dump (大小: %zu) ===\n", sizeof(s));
    for (size_t i = 0; i < sizeof(s); i++) {
        printf("[%02zu] 0x%02X ", i, p[i]);
        if (i == offsetof(struct Simple, a)) printf("<- a");
        if (i == offsetof(struct Simple, b)) printf("<- b[%zu]", i - offsetof(struct Simple, b));
        if (i == offsetof(struct Simple, c)) printf("<- c");
        printf("\n");
    }
}

int main(void) {
    PRINT_STRUCT_LAYOUT(struct Simple);
    PRINT_MEMBER_OFFSET(struct Simple, a);
    PRINT_MEMBER_OFFSET(struct Simple, b);
    PRINT_MEMBER_OFFSET(struct Simple, c);
    // 布局: [a:1][pad:3][b:4][c:1][pad:3] = 12 bytes

    PRINT_STRUCT_LAYOUT(struct Optimized);
    PRINT_MEMBER_OFFSET(struct Optimized, b);
    PRINT_MEMBER_OFFSET(struct Optimized, a);
    PRINT_MEMBER_OFFSET(struct Optimized, c);
    // 布局: [b:4][a:1][c:1][pad:2] = 8 bytes (节省4字节)

    PRINT_STRUCT_LAYOUT(struct Complex);
    PRINT_MEMBER_OFFSET(struct Complex, a);
    PRINT_MEMBER_OFFSET(struct Complex, d);
    PRINT_MEMBER_OFFSET(struct Complex, i);
    PRINT_MEMBER_OFFSET(struct Complex, s);
    PRINT_MEMBER_OFFSET(struct Complex, b);
    // 布局: [a:1][pad:7][d:8][i:4][s:2][b:1][pad:5] = 32 bytes

    PRINT_STRUCT_LAYOUT(struct ComplexOptimized);
    PRINT_MEMBER_OFFSET(struct ComplexOptimized, d);
    PRINT_MEMBER_OFFSET(struct ComplexOptimized, i);
    PRINT_MEMBER_OFFSET(struct ComplexOptimized, s);
    PRINT_MEMBER_OFFSET(struct ComplexOptimized, a);
    PRINT_MEMBER_OFFSET(struct ComplexOptimized, b);
    // 布局: [d:8][i:4][s:2][a:1][b:1] = 16 bytes (节省一半)

    PRINT_STRUCT_LAYOUT(struct WithArray);
    PRINT_MEMBER_OFFSET(struct WithArray, id);
    PRINT_MEMBER_OFFSET(struct WithArray, name);
    PRINT_MEMBER_OFFSET(struct WithArray, score);
    // 布局: [id:4][name:17][pad:3][score:8] = 32 bytes

    PRINT_STRUCT_LAYOUT(struct NestedOuter);
    PRINT_MEMBER_OFFSET(struct NestedOuter, a);
    PRINT_MEMBER_OFFSET(struct NestedOuter, inner);
    PRINT_MEMBER_OFFSET(struct NestedOuter, b);

    visualize_struct_simple();

    printf("\n=== 优化建议 ===\n");
    printf("1. 按成员大小降序排列结构体成员\n");
    printf("2. 使用#pragma pack谨慎压缩对齐\n");
    printf("3. 考虑使用位域减少空间占用\n");
    printf("4. 数组大小尽量对齐到自然边界\n");

    return 0;
}
```

### 内存布局对比表

| 结构体定义 | 大小 | 填充字节 | 说明 |
|-----------|------|---------|------|
| `struct {char a; int b; char c;}` | 12 | 6 | 未优化，填充多 |
| `struct {int b; char a; char c;}` | 8 | 2 | 优化后，节省4字节 |
| `struct {char a; double d; int i;}` | 24 | 11 | 差的对齐 |
| `struct {double d; int i; char a;}` | 16 | 3 | 优化后，节省8字节 |

## 位域和 packed 结构

### 位域(Bit Fields)

```c
// bitfields.c - 位域使用详解
#include <stdio.h>
#include <stdint.h>

// 位域结构体 - 用于硬件寄存器映射
struct RegisterFlags {
    unsigned int enable : 1;      // 1 bit: 使能标志
    unsigned int mode : 3;        // 3 bits: 工作模式 (0-7)
    unsigned int priority : 4;    // 4 bits: 优先级 (0-15)
    unsigned int reserved : 8;    // 8 bits: 保留
    unsigned int status : 16;     // 16 bits: 状态码
};

// 内存紧凑的标志结构
struct CompactFlags {
    uint8_t is_active : 1;
    uint8_t is_visible : 1;
    uint8_t is_enabled : 1;
    uint8_t reserved : 5;
};

// 网络协议头示例 (IPv4头部简化)
struct IPHeader {
#if defined(__LITTLE_ENDIAN__)
    unsigned int ihl : 4;         // 头部长度
    unsigned int version : 4;     // 版本
#elif defined(__BIG_ENDIAN__)
    unsigned int version : 4;
    unsigned int ihl : 4;
#else
    #error "Unknown endianness"
#endif
    unsigned int tos : 8;         // 服务类型
    unsigned int total_length : 16;
    // ... 更多字段
};

// 位域操作示例
void bitfield_demo(void) {
    struct RegisterFlags reg = {0};

    printf("=== 位域操作 ===\n");
    printf("sizeof(struct RegisterFlags) = %zu bytes\n", sizeof(reg));

    // 设置位域
    reg.enable = 1;
    reg.mode = 5;
    reg.priority = 10;

    printf("enable = %u\n", reg.enable);
    printf("mode = %u\n", reg.mode);
    printf("priority = %u\n", reg.priority);

    // 位域溢出行为 (实现定义)
    reg.mode = 10;  // 超过3位最大值(7)
    printf("mode overflow = %u (truncated)\n", reg.mode);
}

int main(void) {
    bitfield_demo();

    printf("\n=== 位域注意事项 ===\n");
    printf("1. 位域布局是实现定义的，不可移植\n");
    printf("2. 不能取位域的地址 (&reg.enable 非法)\n");
    printf("3. 位域成员必须是整型类型\n");
    printf("4. 无名位域可用于填充对齐\n");
    printf("5. 零宽度位域强制对齐到下一个存储单元\n");

    return 0;
}
```

### Packed 结构体

```c
// packed_structs.c - 紧凑结构体
#include <stdio.h>
#include <stdint.h>

// 标准结构体 (有填充)
struct StandardPacket {
    uint8_t type;      // 1 byte
    uint32_t length;   // 4 bytes (填充3 bytes)
    uint16_t port;     // 2 bytes (填充2 bytes)
    uint8_t data[6];   // 6 bytes
};
// 总计: 16 bytes (含6 bytes填充)

// 紧凑结构体 - 使用编译器扩展
#ifdef __GNUC__
    #define PACKED __attribute__((packed))
#elif defined(_MSC_VER)
    #define PACKED #pragma pack(push, 1)
    #define PACKED_POP #pragma pack(pop)
#else
    #define PACKED
#endif

struct PACKED PackedPacket {
    uint8_t type;      // 1 byte
    uint32_t length;   // 4 bytes (无填充)
    uint16_t port;     // 2 bytes
    uint8_t data[6];   // 6 bytes
};
// 总计: 13 bytes (无填充)

// 可移植的紧凑结构体 (手动填充)
struct PortablePacket {
    uint8_t type;
    uint8_t length_bytes[4];  // 手动序列化
    uint8_t port_bytes[2];
    uint8_t data[6];
};

// 网络字节序转换函数
uint32_t read_be32(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) |
           ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) |
           (uint32_t)p[3];
}

void write_be32(uint8_t *p, uint32_t val) {
    p[0] = (val >> 24) & 0xFF;
    p[1] = (val >> 16) & 0xFF;
    p[2] = (val >> 8) & 0xFF;
    p[3] = val & 0xFF;
}

int main(void) {
    printf("=== 结构体大小对比 ===\n");
    printf("sizeof(StandardPacket) = %zu bytes\n", sizeof(struct StandardPacket));
    printf("sizeof(PackedPacket)   = %zu bytes\n", sizeof(struct PackedPacket));
    printf("sizeof(PortablePacket) = %zu bytes\n", sizeof(struct PortablePacket));

    printf("\n=== 紧凑结构体利弊 ===\n");
    printf("优点:\n");
    printf("  - 节省内存\n");
    printf("  - 精确匹配网络/文件协议\n");
    printf("  - 减少内存带宽需求\n");
    printf("\n缺点:\n");
    printf("  - 访问可能变慢(未对齐访问)\n");
    printf("  - 某些平台可能崩溃(如ARM)\n");
    printf("  - 不可移植(编译器扩展)\n");
    printf("  - 调试困难\n");

    return 0;
}
```

## 对比矩阵表格

### 完整类型对比矩阵

| 类型 | 典型大小(64位) | 对齐(64位) | 典型大小(32位) | 对齐(32位) | 用途 |
|------|---------------|-----------|---------------|-----------|------|
| `_Bool`/`bool` | 1 | 1 | 1 | 1 | 布尔值 |
| `char` | 1 | 1 | 1 | 1 | 字符/小整数 |
| `signed char` | 1 | 1 | 1 | 1 | 有符号8位 |
| `unsigned char` | 1 | 1 | 1 | 1 | 无符号8位 |
| `short` | 2 | 2 | 2 | 2 | 16位整数 |
| `unsigned short` | 2 | 2 | 2 | 2 | 无符号16位 |
| `int` | 4 | 4 | 4 | 4 | 标准整数 |
| `unsigned int` | 4 | 4 | 4 | 4 | 无符号32位 |
| `long` (LP64) | 8 | 8 | 4 | 4 | 长整数 |
| `long` (LLP64) | 4 | 4 | 4 | 4 | Windows长整数 |
| `unsigned long` | 8/4 | 8/4 | 4 | 4 | 无符号长整数 |
| `long long` | 8 | 8 | 8 | 8 | 64位整数 |
| `float` | 4 | 4 | 4 | 4 | 单精度浮点 |
| `double` | 8 | 8 | 8 | 4/8 | 双精度浮点 |
| `long double` | 16 | 16 | 12/16 | 4 | 扩展精度 |
| `void*` | 8 | 8 | 4 | 4 | 通用指针 |
| `size_t` | 8 | 8 | 4 | 4 | 大小类型 |
| `ptrdiff_t` | 8 | 8 | 4 | 4 | 指针差值 |
| `intptr_t` | 8 | 8 | 4 | 4 | 整数可存指针 |
| `uintptr_t` | 8 | 8 | 4 | 4 | 无符号整数存指针 |
| `wchar_t` | 4 | 4 | 4/2 | 4/2 | 宽字符 |
| `char16_t` | 2 | 2 | 2 | 2 | UTF-16 |
| `char32_t` | 4 | 4 | 4 | 4 | UTF-32 |

### 结构体内存布局对比

| 场景 | 未优化大小 | 优化后大小 | 节省空间 | 优化方法 |
|------|-----------|-----------|---------|---------|
| 简单结构体 | 12 bytes | 8 bytes | 33% | 按大小排序 |
| 复杂结构体 | 32 bytes | 16 bytes | 50% | 重排成员 |
| 带数组结构体 | 32 bytes | 29 bytes | 9% | 调整数组大小 |
| 嵌套结构体 | 24 bytes | 16 bytes | 33% | 扁平化/重排 |
| 网络包结构 | 16 bytes | 13 bytes | 19% | packed属性 |
| 位域结构体 | 8 bytes | 4 bytes | 50% | 使用位域 |

## C代码: 类型信息打印、结构体布局分析

```c
// comprehensive_type_analysis.c
#include <stdio.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>

// 类型信息结构体
typedef struct {
    const char *name;
    size_t size;
    size_t align;
} type_info_t;

#define TYPE_INFO(t) {#t, sizeof(t), alignof(t)}

// 打印类型信息表
void print_type_table(void) {
    type_info_t types[] = {
        TYPE_INFO(char),
        TYPE_INFO(short),
        TYPE_INFO(int),
        TYPE_INFO(long),
        TYPE_INFO(long long),
        TYPE_INFO(float),
        TYPE_INFO(double),
        TYPE_INFO(long double),
        TYPE_INFO(void*),
        TYPE_INFO(size_t),
        TYPE_INFO(bool),
        TYPE_INFO(wchar_t),
        TYPE_INFO(int8_t),
        TYPE_INFO(int16_t),
        TYPE_INFO(int32_t),
        TYPE_INFO(int64_t),
    };

    int count = sizeof(types) / sizeof(types[0]);

    printf("┌──────────────────┬──────────┬──────────┐\n");
    printf("│ Type             │ Size     │ Align    │\n");
    printf("├──────────────────┼──────────┼──────────┤\n");

    for (int i = 0; i < count; i++) {
        printf("│ %-16s │ %3zu bytes│ %3zu bytes│\n",
               types[i].name, types[i].size, types[i].align);
    }

    printf("└──────────────────┴──────────┴──────────┘\n");
}

// 结构体布局分析器
typedef struct {
    const char *name;
    size_t offset;
    size_t size;
} member_info_t;

#define MEMBER_INFO(s, m) {#m, offsetof(s, m), sizeof(((s*)0)->m)}

// 分析并打印结构体布局
#define ANALYZE_STRUCT(sname, ...) do { \
    member_info_t members[] = {__VA_ARGS__}; \
    int n = sizeof(members) / sizeof(members[0]); \
    size_t total_size = sizeof(sname); \
    size_t max_align = alignof(sname); \
    \
    printf("\n=== " #sname " 布局分析 ===\n"); \
    printf("总大小: %zu bytes, 对齐: %zu bytes\n\n", total_size, max_align); \
    printf("┌──────────────────┬─────────┬─────────┬─────────────────┐\n"); \
    printf("│ Member           │ Offset  │ Size    │ Padding After   │\n"); \
    printf("├──────────────────┼─────────┼─────────┼─────────────────┤\n"); \
    \
    size_t prev_end = 0; \
    for (int i = 0; i < n; i++) { \
        size_t padding = members[i].offset - prev_end; \
        printf("│ %-16s │ %5zu   │ %5zu   │ %5zu bytes     │\n", \
               members[i].name, members[i].offset, members[i].size, padding); \
        prev_end = members[i].offset + members[i].size; \
    } \
    size_t tail_padding = total_size - prev_end; \
    printf("│ %-16s │ %5s   │ %5s   │ %5zu bytes     │\n", \
           "[end]", "-", "-", tail_padding); \
    printf("└──────────────────┴─────────┴─────────┴─────────────────┘\n"); \
} while(0)

// 测试结构体
struct Example {
    char a;
    int b;
    short c;
    double d;
    char e;
};

struct ExampleOptimized {
    double d;
    int b;
    short c;
    char a;
    char e;
};

// 内存可视化
void visualize_memory(void *ptr, size_t size, const char *label) {
    printf("\n=== %s 内存视图 ===\n", label);
    unsigned char *p = ptr;

    for (size_t i = 0; i < size; i += 16) {
        printf("%04zx: ", i);

        // 十六进制
        for (size_t j = 0; j < 16 && i + j < size; j++) {
            printf("%02x ", p[i + j]);
        }

        // 对齐填充
        for (size_t j = size - i; j < 16 && i + j >= size; j++) {
            printf("   ");
        }

        printf(" |");

        // ASCII
        for (size_t j = 0; j < 16 && i + j < size; j++) {
            unsigned char c = p[i + j];
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }

        printf("|\n");
    }
}

int main(void) {
    printf("========================================\n");
    printf("    C 类型存储完整分析工具\n");
    printf("========================================\n");

    printf("\n=== 基本类型信息表 ===\n");
    print_type_table();

    ANALYZE_STRUCT(struct Example,
        MEMBER_INFO(struct Example, a),
        MEMBER_INFO(struct Example, b),
        MEMBER_INFO(struct Example, c),
        MEMBER_INFO(struct Example, d),
        MEMBER_INFO(struct Example, e)
    );

    ANALYZE_STRUCT(struct ExampleOptimized,
        MEMBER_INFO(struct ExampleOptimized, d),
        MEMBER_INFO(struct ExampleOptimized, b),
        MEMBER_INFO(struct ExampleOptimized, c),
        MEMBER_INFO(struct ExampleOptimized, a),
        MEMBER_INFO(struct ExampleOptimized, e)
    );

    // 可视化
    struct Example ex = {'X', 0x12345678, 0xABCD, 3.14159, 'Y'};
    visualize_memory(&ex, sizeof(ex), "struct Example");

    printf("\n=== 数值范围 ===\n");
    printf("CHAR_BIT   = %d\n", CHAR_BIT);
    printf("INT_MIN    = %d\n", INT_MIN);
    printf("INT_MAX    = %d\n", INT_MAX);
    printf("UINT_MAX   = %u\n", UINT_MAX);
    printf("LONG_MIN   = %ld\n", LONG_MIN);
    printf("LONG_MAX   = %ld\n", LONG_MAX);
    printf("FLT_MIN    = %e\n", FLT_MIN);
    printf("FLT_MAX    = %e\n", FLT_MAX);
    printf("DBL_MIN    = %e\n", DBL_MIN);
    printf("DBL_MAX    = %e\n", DBL_MAX);

    return 0;
}
```

## 编译和运行

```bash
# 编译类型分析工具
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
gcc -std=c17 -o type_analysis comprehensive_type_analysis.c
gcc -std=c17 -o struct_layout struct_layout.c

# 运行查看结果
./type_analysis
./struct_layout
```


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
