/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\02_Memcpy_SIMD.md
 * Line: 335
 * Language: c
 * Block ID: 8822fb0b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 软件预取：在需要数据之前将其加载到缓存
 *
 * _MM_HINT_T0: 预取到所有缓存级别
 * _MM_HINT_T1: 预取到L2（不进入L1）
 * _MM_HINT_T2: 预取到L3
 * _MM_HINT_NTA: 非临时预取（一次性使用）
 */

// 带预取的memcpy
void* memcpy_prefetch(void *dst, const void *src, size_t n) {
    unsigned char *d = dst;
    const unsigned char *s = src;

    // 预取距离（缓存行数）
    const int PREFETCH_DISTANCE = 8;  // 8 * 64 = 512字节

    // 头部对齐
    while (n > 0 && ((uintptr_t)d & 31)) {
        *d++ = *s++;
        n--;
    }

    // 主体：预取 + 拷贝
    while (n >= 256) {
        // 预取未来数据
        _mm_prefetch((const char*)s + PREFETCH_DISTANCE * 64, _MM_HINT_T0);

        // 拷贝当前数据
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

    // 剩余处理...

    return dst;
}
