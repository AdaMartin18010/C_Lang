/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\02_Memcpy_SIMD.md
 * Line: 466
 * Language: c
 * Block ID: dfaaeb7c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 根据拷贝大小选择最优算法：
 *
 * 0-64字节：    逐字节或rep movsb
 * 64-512字节：  SSE/AVX简单循环
 * 512-2KB：     AVX展开
 * 2KB-256KB：   AVX + 预取
 * 256KB+：      非临时存储
 */

void* memcpy_optimized(void *dst, const void *src, size_t n) {
    // 完全重叠检查（memmove语义）
    if (src < dst && (const char*)src + n > (char*)dst) {
        return memmove(dst, src, n);
    }

    // 小拷贝：rep movsb（现代CPU优化良好）
    if (n <= 64) {
        return memcpy_small(dst, src, n);
    }

    // 中拷贝：AVX2
    if (n <= 256 * 1024) {
        return memcpy_avx2_prefetch(dst, src, n);
    }

    // 大拷贝：非临时存储
    return memcpy_nt(dst, src, n);
}

// 超小拷贝（编译器内联友好）
static inline void* memcpy_tiny(void *dst, const void *src, size_t n) {
    unsigned char *d = dst;
    const unsigned char *s = src;

    // 手动展开减少分支
    if (n & 16) {
        __m128i v = _mm_loadu_si128((__m128i*)s);
        _mm_storeu_si128((__m128i*)d, v);
        s += 16; d += 16;
    }
    if (n & 8) {
        *(uint64_t*)d = *(uint64_t*)s;
        s += 8; d += 8;
    }
    if (n & 4) {
        *(uint32_t*)d = *(uint32_t*)s;
        s += 4; d += 4;
    }
    if (n & 2) {
        *(uint16_t*)d = *(uint16_t*)s;
        s += 2; d += 2;
    }
    if (n & 1) {
        *d = *s;
    }

    return dst;
}
