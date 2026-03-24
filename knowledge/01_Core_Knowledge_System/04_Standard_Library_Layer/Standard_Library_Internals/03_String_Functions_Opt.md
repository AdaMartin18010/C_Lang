---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
# C标准库字符串函数优化实现

> **函数**: strlen, strcpy, strcmp, memcpy, memmove
> **优化技术**: 字长对齐、向量化、循环展开

---

## 1. strlen优化

### 1.1 基础实现

```c
// 朴素实现 - 逐字节比较
size_t strlen_slow(const char* s) {
    const char* p = s;
    while (*p != '\0') {
        p++;
    }
    return p - s;
}
```

### 1.2 字长对齐优化

```c
// glibc风格优化实现
#include <stdint.h>
#include <limits.h>

// 检查一个size_t是否包含零字节
// 原理: 0x80在字节中，如果该字节为0，减法借位会改变高位
#define HAS_ZERO(x) (((x) - 0x01010101) & ~(x) & 0x80808080)

size_t strlen_fast(const char* str) {
    const char* s = str;
    const char* p;

    // 1. 逐字节直到对齐到字边界
    while ((uintptr_t)s & (sizeof(size_t) - 1)) {
        if (*s == '\0') return s - str;
        s++;
    }

    // 2. 每次检查一个size_t (4或8字节)
    p = s;
    while (1) {
        size_t w = *(const size_t*)p;
        if (HAS_ZERO(w)) break;
        p += sizeof(size_t);
    }

    // 3. 找到具体位置
    while (*p != '\0') p++;
    return p - str;
}

// 解释HAS_ZERO:
// 对于32位: 0x01010101 是重复的字节1
// 如果w中有字节为0:
//   w = 0x??00????
//   w - 0x01010101: 低位字节借位
//   ~(w): 0字节位置变0xFF
//   结果在0字节位置有0x80
```

### 1.3 SIMD优化 (AVX2)

```c
#include <immintrin.h>

size_t strlen_avx2(const char* str) {
    const char* s = str;

    // 对齐到32字节
    while ((uintptr_t)s & 31) {
        if (*s == '\0') return s - str;
        s++;
    }

    // AVX2每次处理32字节
    const __m256i zero = _mm256_setzero_si256();

    while (1) {
        __m256i data = _mm256_load_si256((const __m256i*)s);
        __m256i cmp = _mm256_cmpeq_epi8(data, zero);
        int mask = _mm256_movemask_epi8(cmp);

        if (mask != 0) {
            // 找到零字节位置
            return (s - str) + __builtin_ctz(mask);
        }

        s += 32;
    }
}
```

---

## 2. memcpy优化

### 2.1 分层实现策略

```c
#include <stdint.h>
#include <stddef.h>

// glibc memcpy策略:
// 1. < 16 bytes: 逐字节或小规模复制
// 2. 16-128 bytes: SSE/AVX
// 3. > 128 bytes: 非临时存储 (绕过缓存)

void* memcpy_optimized(void* dst, const void* src, size_t n) {
    unsigned char* d = dst;
    const unsigned char* s = src;

    // 小数据: 逐字节
    if (n < 16) {
        while (n--) *d++ = *s++;
        return dst;
    }

    // 对齐目标地址
    while ((uintptr_t)d & 7) {
        *d++ = *s++;
        n--;
    }

    // 8字节复制
    uint64_t* dw = (uint64_t*)d;
    const uint64_t* sw = (const uint64_t*)s;

    while (n >= 8) {
        *dw++ = *sw++;
        n -= 8;
    }

    // 剩余字节
    d = (unsigned char*)dw;
    s = (const unsigned char*)sw;
    while (n--) *d++ = *s++;

    return dst;
}
```

### 2.2 非临时存储 (大块数据)

```c
#include <immintrin.h>

// 对于> 128KB的数据，使用非临时存储避免缓存污染
void memcpy_nt(void* dst, const void* src, size_t n) {
    char* d = dst;
    const char* s = src;

    // 前导未对齐部分
    while ((uintptr_t)d & 31) {
        *d++ = *s++;
        n--;
    }

    // AVX2非临时存储
    while (n >= 32) {
        __m256i data = _mm256_loadu_si256((const __m256i*)s);
        _mm256_stream_si256((__m256i*)d, data);
        s += 32;
        d += 32;
        n -= 32;
    }

    //  fence保证顺序
    _mm_sfence();

    // 尾部
    while (n--) *d++ = *s++;
}
```

---

## 3. strcpy/strncpy优化

### 3.1 安全实现

```c
// strncpy问题: 不保证null终止，填充0到n字节
// 推荐实现: 保证终止，不填充

char* strlcpy_impl(char* dst, const char* src, size_t size) {
    if (size == 0) return dst;

    size_t i;
    for (i = 0; i < size - 1 && src[i] != '\0'; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';

    // 返回源字符串长度
    while (src[i] != '\0') i++;
    return i;
}

// BSD strlcpy风格
size_t strlcpy_bsd(char* dst, const char* src, size_t size) {
    const char* s = src;
    size_t n = size;

    // 复制
    if (n != 0) {
        while (--n != 0) {
            if ((*dst++ = *s++) == '\0') break;
        }
    }

    // 如果需要，添加终止符
    if (n == 0) {
        if (size != 0) *dst = '\0';
        // 计算剩余长度
        while (*s++) ;
    }

    return s - src - 1;
}
```

---

## 4. strcmp优化

### 4.1 字长比较

```c
int strcmp_fast(const char* s1, const char* s2) {
    // 对齐到字边界
    while ((uintptr_t)s1 & (sizeof(size_t) - 1)) {
        unsigned char c1 = *s1++;
        unsigned char c2 = *s2++;
        if (c1 != c2 || c1 == '\0') return c1 - c2;
    }

    // 字长比较 (需要处理零字节)
    while (1) {
        size_t w1 = *(const size_t*)s1;
        size_t w2 = *(const size_t*)s2;

        // 如果有差异或零字节
        if (w1 != w2 || HAS_ZERO(w1)) {
            // 逐字节比较
            for (int i = 0; i < sizeof(size_t); i++) {
                unsigned char c1 = s1[i];
                unsigned char c2 = s2[i];
                if (c1 != c2 || c1 == '\0') return c1 - c2;
            }
        }

        s1 += sizeof(size_t);
        s2 += sizeof(size_t);
    }
}
```

---

## 5. 性能对比

### 5.1 理论加速比

| 函数 | 朴素实现 | 优化实现 | 加速比 |
|:-----|:---------|:---------|:-------|
| strlen | O(n) 逐字节 | O(n/8) 字长 | ~4-8x |
| memcpy | 逐字节 | 字长+SIMD | ~10-50x |
| strcpy | 逐字节 | 字长复制 | ~4-8x |
| strcmp | 逐字节 | 字长比较 | ~4-8x |

### 5.2 测试方法

```c
#include <time.h>
#include <stdio.h>

// 性能测试模板
void benchmark(const char* name,
               void* (*func)(void*, const void*, size_t),
               size_t size) {
    char* src = aligned_alloc(32, size);
    char* dst = aligned_alloc(32, size);

    // 预热
    func(dst, src, size);

    clock_t start = clock();
    for (int i = 0; i < 10000; i++) {
        func(dst, src, size);
    }
    clock_t end = clock();

    double ms = (double)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("%s: %.2f ms, %.2f GB/s\n",
           name, ms, (double)size * 10000 / ms / 1e6);

    free(src);
    free(dst);
}
```

---

## 6. 使用建议

### 6.1 现代C项目

```c
// 使用标准库 (已优化)
#include <string.h>
memcpy(dst, src, n);  // glibc已高度优化

// 或使用C11安全函数
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
memcpy_s(dst, dst_size, src, n);
```

### 6.2 嵌入式系统

```c
// 简单的优化实现 (避免复杂的glibc)
size_t strlen_embedded(const char* s) {
    const char* p = s;
    while (*p) p++;
    return p - s;
}
```

---

**最后更新**: 2026-03-24
