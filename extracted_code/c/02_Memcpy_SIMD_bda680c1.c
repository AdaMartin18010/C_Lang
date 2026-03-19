/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\02_Memcpy_SIMD.md
 * Line: 151
 * Language: c
 * Block ID: bda680c1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * SSE2 (128-bit):
 * - 16个XMM寄存器（XMM0-XMM15）
 * - 支持整数和浮点
 * - 16字节对齐要求最佳性能
 *
 * AVX/AVX2 (256-bit):
 * - 16个YMM寄存器（YMM0-YMM15）
 * - AVX2支持256-bit整数
 * - 32字节对齐
 * - 非对齐访问惩罚较小
 *
 * AVX-512 (512-bit):
 * - 32个ZMM寄存器
 * - 掩码操作支持
 * - 频率降低风险（热设计功耗）
 */

// SSE2 memcpy核心（16字节块）
void* memcpy_sse2(void *dst, const void *src, size_t n) {
    unsigned char *d = dst;
    const unsigned char *s = src;

    // 头部：处理到16字节对齐
    while (n > 0 && ((uintptr_t)d & 15)) {
        *d++ = *s++;
        n--;
    }

    // 主体：16字节SIMD复制
    while (n >= 64) {
        __m128i v0 = _mm_load_si128((__m128i*)(s + 0));
        __m128i v1 = _mm_load_si128((__m128i*)(s + 16));
        __m128i v2 = _mm_load_si128((__m128i*)(s + 32));
        __m128i v3 = _mm_load_si128((__m128i*)(s + 48));

        _mm_store_si128((__m128i*)(d + 0), v0);
        _mm_store_si128((__m128i*)(d + 16), v1);
        _mm_store_si128((__m128i*)(d + 32), v2);
        _mm_store_si128((__m128i*)(d + 48), v3);

        s += 64;
        d += 64;
        n -= 64;
    }

    // 剩余16字节块
    while (n >= 16) {
        _mm_store_si128((__m128i*)d,
                       _mm_load_si128((__m128i*)s));
        s += 16;
        d += 16;
        n -= 16;
    }

    // 尾部
    while (n > 0) {
        *d++ = *s++;
        n--;
    }

    return dst;
}

// AVX2 memcpy核心（32字节块）
void* memcpy_avx2(void *dst, const void *src, size_t n) {
    unsigned char *d = dst;
    const unsigned char *s = src;

    // 头部对齐到32字节
    while (n > 0 && ((uintptr_t)d & 31)) {
        *d++ = *s++;
        n--;
    }

    // 主体：256-bit AVX2
    while (n >= 256) {
        // 展开4次，使用8个YMM寄存器
        __m256i v0 = _mm256_load_si256((__m256i*)(s + 0));
        __m256i v1 = _mm256_load_si256((__m256i*)(s + 32));
        __m256i v2 = _mm256_load_si256((__m256i*)(s + 64));
        __m256i v3 = _mm256_load_si256((__m256i*)(s + 96));
        __m256i v4 = _mm256_load_si256((__m256i*)(s + 128));
        __m256i v5 = _mm256_load_si256((__m256i*)(s + 160));
        __m256i v6 = _mm256_load_si256((__m256i*)(s + 192));
        __m256i v7 = _mm256_load_si256((__m256i*)(s + 224));

        _mm256_store_si256((__m256i*)(d + 0), v0);
        _mm256_store_si256((__m256i*)(d + 32), v1);
        _mm256_store_si256((__m256i*)(d + 64), v2);
        _mm256_store_si256((__m256i*)(d + 96), v3);
        _mm256_store_si256((__m256i*)(d + 128), v4);
        _mm256_store_si256((__m256i*)(d + 160), v5);
        _mm256_store_si256((__m256i*)(d + 192), v6);
        _mm256_store_si256((__m256i*)(d + 224), v7);

        s += 256;
        d += 256;
        n -= 256;
    }

    // 继续处理剩余...

    return dst;
}
