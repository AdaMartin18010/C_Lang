# C 语言位操作完全指南

> **难度**: L2-L4
> **对应标准**: ISO C §6.5.10-6.5.12 (位运算符)
> **核心目标**: 掌握位级数据表示、操作和优化技巧

---

## 1. 位操作基础

### 1.1 六种位运算符

```c
// 按位与 — 掩码提取
uint8_t flags = 0b11001100;
uint8_t mask  = 0b00001111;
uint8_t low_nibble = flags & mask;   // 0b00001100

// 按位或 — 设置位
uint8_t result = flags | 0b00000001;  // 设置最低位: 0b11001101

// 按位异或 — 切换位
uint8_t toggled = flags ^ 0b11110000; // 翻转高4位: 0b00111100

// 按位取反 — 翻转所有位
uint8_t inverted = ~flags;            // 0b00110011

// 左移 — 乘以2的幂
uint8_t shifted = flags << 2;         // 0b00110000

// 右移 — 除以2的幂（注意：逻辑右移 vs 算术右移）
int8_t signed_val = -8;    // 0b11111000
int8_t arith_right = signed_val >> 1;  // 0b11111100 (-4) 算术右移
uint8_t unsigned_val = 248; // 0b11111000
uint8_t logic_right = unsigned_val >> 1; // 0b01111100 (124) 逻辑右移
```

### 1.2 常用位操作宏

```c
// 位操作基础宏（适用于无符号整数）
#define BIT_SET(x, n)     ((x) |= (1U << (n)))
#define BIT_CLEAR(x, n)   ((x) &= ~(1U << (n)))
#define BIT_TOGGLE(x, n)  ((x) ^= (1U << (n)))
#define BIT_CHECK(x, n)   (((x) >> (n)) & 1U)
#define BIT_MASK(n)       ((1U << (n)) - 1U)

// 使用示例
uint32_t reg = 0;
BIT_SET(reg, 3);       // reg = 0b1000
BIT_SET(reg, 0);       // reg = 0b1001
if (BIT_CHECK(reg, 3)) {
    BIT_CLEAR(reg, 3); // reg = 0b0001
}
```

---

## 2. 位掩码与标志位

### 2.1 标志位枚举设计

```c
// ✅ 使用 1 << n 确保不重叠
typedef enum {
    FLAG_READ   = 1 << 0,   // 0x01
    FLAG_WRITE  = 1 << 1,   // 0x02
    FLAG_EXEC   = 1 << 2,   // 0x04
    FLAG_CREATE = 1 << 3,   // 0x08
    FLAG_TRUNC  = 1 << 4,   // 0x10
} FileFlags;

// 组合标志
FileFlags mode = FLAG_READ | FLAG_WRITE;  // 0x03

// 检查标志
if (mode & FLAG_READ) {   // 检查读权限
    // 可以读取
}

// 同时检查多个标志
if ((mode & (FLAG_READ | FLAG_WRITE)) == (FLAG_READ | FLAG_WRITE)) {
    // 既有读权限又有写权限
}
```

### 2.2 位域 (Bit Fields)

```c
// 紧凑打包多个小整数
struct PacketHeader {
    uint8_t version : 4;    // 4 bits
    uint8_t flags   : 4;    // 4 bits
    uint16_t length : 12;   // 12 bits
    uint16_t type   : 4;    // 4 bits
} __attribute__((packed));  // 防止填充

// ⚠️ 位域的可移植性问题：
// 1. 布局因编译器而异（大端 vs 小端）
// 2. 不能取位域地址
// 3. 跨平台数据交换建议用手动掩码代替
```

---

## 3. 经典位操作技巧 (Bit Hacks)

### 3.1 判断奇偶

```c
// ❌ 常见但较慢
bool is_odd(int x) { return x % 2 != 0; }

// ✅ 位运算版本
bool is_odd_fast(int x) { return x & 1; }
```

### 3.2 交换两个数（无临时变量）

```c
// ✅ 异或交换（仅适用于整数）
void swap_xor(int *a, int *b) {
    if (a != b) {           // 必须检查同一地址！
        *a ^= *b;
        *b ^= *a;
        *a ^= *b;
    }
}

// ⚠️ 现代编译器对普通 swap 的优化已经很好，不必刻意使用
```

### 3.3 计算二进制中1的个数 (Population Count)

```c
// 软件实现
int popcount32(uint32_t x) {
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x = x + (x >> 8);
    x = x + (x >> 16);
    return x & 0x3F;
}

// ✅ 硬件指令 (GCC/Clang)
int popcount_fast(uint32_t x) {
    return __builtin_popcount(x);   // 编译为 POPCNT 指令
}
```

### 3.4 获取最低设置位的位置

```c
// 返回最低设置位的索引（0-based）
int lowest_set_bit(uint32_t x) {
    return __builtin_ctz(x);   // Count Trailing Zeros
}

// 如果没有设置位，行为未定义，需先检查
if (x != 0) {
    int pos = __builtin_ctz(x);
}

// 相关内置函数：
// __builtin_clz(x)   — 前导零个数
// __builtin_ctz(x)   — 后导零个数
// __builtin_popcount(x) — 1的个数
```

### 3.5 判断2的幂

```c
bool is_power_of_2(uint32_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}
// 原理: 2^n 的二进制只有一位是1
// x     = 0b00100000
// x - 1 = 0b00011111
// x & (x-1) = 0b00000000 → 是2的幂
```

### 3.6 对齐到2的幂边界

```c
// 将 x 向上对齐到 align 的倍数（align 必须是2的幂）
uintptr_t align_up(uintptr_t x, size_t align) {
    return (x + align - 1) & ~(align - 1);
}

// 示例
align_up(100, 64);  // 128
align_up(64, 64);   // 64
align_up(1, 4096);  // 4096 (页对齐)
```

---

## 4. 字节序 (Endianness)

### 4.1 检测字节序

```c
#include <stdint.h>

// 编译期检测（现代编译器支持）
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    #define IS_LITTLE_ENDIAN 1
#else
    #define IS_LITTLE_ENDIAN 0
#endif

// 运行期检测
bool is_little_endian(void) {
    union { uint16_t val; uint8_t bytes[2]; } test;
    test.val = 0x0102;
    return test.bytes[0] == 0x02;
}
```

### 4.2 字节序转换

```c
// 标准库函数（POSIX）
#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong);   // host to network (big-endian)
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);    // network to host
uint16_t ntohs(uint16_t netshort);

// 手动实现（无依赖）
uint16_t bswap16(uint16_t x) {
    return (x >> 8) | (x << 8);
}

uint32_t bswap32(uint32_t x) {
    return ((x >> 24) & 0xFF)      |
           ((x >> 8)  & 0xFF00)    |
           ((x << 8)  & 0xFF0000)  |
           ((x << 24) & 0xFF000000);
}

// GCC/Clang 内置
uint32_t bswap32_fast(uint32_t x) {
    return __builtin_bswap32(x);
}
```

---

## 5. 位操作在嵌入式和系统编程中的应用

### 5.1 寄存器操作

```c
// 硬件寄存器映射
volatile uint32_t * const GPIO_CTRL = (uint32_t *)0x40020000;

// 安全修改寄存器的特定位（读-改-写）
#define REG_SET_BITS(reg, mask)   do { *(reg) |= (mask); } while(0)
#define REG_CLEAR_BITS(reg, mask) do { *(reg) &= ~(mask); } while(0)
#define REG_MODIFY(reg, clear_mask, set_mask) do { \
    uint32_t _val = *(reg); \
    _val &= ~(clear_mask); \
    _val |= (set_mask); \
    *(reg) = _val; \
} while(0)

// 使用
REG_CLEAR_BITS(GPIO_CTRL, 0xFF);        // 清除低8位
REG_SET_BITS(GPIO_CTRL, 0x05);          // 设置位0和位2
REG_MODIFY(GPIO_CTRL, 0x0F, 0x05);      // 清除低4位，设置为0x05
```

### 5.2 位图分配器

```c
// 简单的位图内存分配器
#define BITMAP_BITS 1024
#define WORD_BITS 64

uint64_t bitmap[BITMAP_BITS / WORD_BITS];

int bitmap_alloc(void) {
    for (int i = 0; i < BITMAP_BITS / WORD_BITS; i++) {
        if (bitmap[i] != ~0ULL) {  // 不是所有位都被占用
            int bit = __builtin_ctzll(~bitmap[i]);  // 找到第一个0位
            bitmap[i] |= (1ULL << bit);
            return i * WORD_BITS + bit;
        }
    }
    return -1;  // 无空闲位
}

void bitmap_free(int index) {
    int word = index / WORD_BITS;
    int bit = index % WORD_BITS;
    bitmap[word] &= ~(1ULL << bit);
}
```

---

## 6. 安全注意事项

### 6.1 移位操作的陷阱

```c
// ❌ 未定义行为
int x = 1 << 33;        // 移位 >= 类型宽度 (UB)
int y = -1 << 1;        // 左移负数 (UB)
int z = 1 << -1;        // 负移位 (UB)

// ✅ 安全移位
unsigned int a = 1U << 31;           // OK
uint64_t b = 1ULL << 33;             // OK
unsigned int c = 1U >> 2;            // OK (右移 unsigned 总是逻辑右移)

// 安全移位包装
uint32_t safe_lshift(uint32_t x, unsigned int n) {
    if (n >= 32) return 0;
    return x << n;
}
```

### 6.2 符号位与无符号运算

```c
// ❌ 混合有符号和无符号
int a = -1;
unsigned int b = 1;
if (a < b) {   // a 被提升为 unsigned，变成很大的正数！
    // 不会执行！
}

// ✅ 位操作始终使用无符号类型
uint32_t mask = 0xFFFFFFFFU;  // 明确指定 unsigned
```

---

## 7. 检查清单

- [ ] 位操作使用无符号类型 (`uintN_t`)
- [ ] 移位前检查移位量 < 类型宽度
- [ ] 不混合有符号和无符号位运算
- [ ] 跨平台代码避免依赖位域布局
- [ ] 网络/文件数据使用显式字节序转换
- [ ] 关键路径使用编译器内置函数替代软件实现

---

> **最后更新**: 2026-05-11
> **参考**: Hacker's Delight (Warren), Bit Twiddling Hacks (Stanford)
