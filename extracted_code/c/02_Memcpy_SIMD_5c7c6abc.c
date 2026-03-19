/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\02_Memcpy_SIMD.md
 * Line: 396
 * Language: c
 * Block ID: 5c7c6abc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 内存对齐对性能的影响：
 *
 * - 未对齐访问可能导致：
 *   * 跨缓存行访问（2x cache misses）
 *   * 页边界跨越（更慢）
 *   * SIMD操作异常（某些旧CPU）
 *
 * 现代x86支持未对齐SIMD，但仍有性能惩罚
 */

// 处理源和目的对齐不匹配的情况
void* memcpy_unaligned(void *dst, const void *src, size_t n) {
    unsigned char *d = dst;
    const unsigned char *s = src;

    // 情况分析
    int dst_align = (uintptr_t)d & 31;
    int src_align = (uintptr_t)s & 31;

    if (dst_align == src_align) {
        // 相对对齐：可以同时处理
        // 头部处理到32字节对齐
        while (n > 0 && ((uintptr_t)d & 31)) {
            *d++ = *s++;
            n--;
        }

        // 现在相对对齐，可以使用对齐指令
        // 即使绝对地址未对齐
        while (n >= 256) {
            __m256i v0 = _mm256_loadu_si256((__m256i*)(s + 0));
            __m256i v1 = _mm256_loadu_si256((__m256i*)(s + 32));
            // ...
            _mm256_storeu_si256((__m256i*)(d + 0), v0);
            _mm256_storeu_si256((__m256i*)(d + 32), v1);
            // ...
            s += 256;
            d += 256;
            n -= 256;
        }
    } else {
        // 未对齐：使用未对齐加载/存储
        // 或逐字节处理头部直到对齐

        // 选项1：直接使用未对齐指令（现代CPU惩罚小）
        while (n >= 32) {
            _mm256_storeu_si256((__m256i*)d,
                               _mm256_loadu_si256((__m256i*)s));
            s += 32;
            d += 32;
            n -= 32;
        }
    }

    // 尾部
    while (n > 0) {
        *d++ = *s++;
        n--;
    }

    return dst;
}
