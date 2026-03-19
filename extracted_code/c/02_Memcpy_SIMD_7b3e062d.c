/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\02_Memcpy_SIMD.md
 * Line: 263
 * Language: c
 * Block ID: 7b3e062d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 非临时存储（Non-temporal stores）：
 *
 * 问题：大块内存拷贝会污染L1/L2缓存
 * 解决方案：使用非临时存储，绕过缓存直接写入内存
 *
 * 适用场景：
 * - 拷贝大小超过L2缓存容量
 * - 数据不会立即重用
 */

// 带非临时存储的memcpy
void* memcpy_nt(void *dst, const void *src, size_t n) {
    // 阈值：超过L2缓存大小使用NT存储
    const size_t NT_THRESHOLD = 256 * 1024;  // 256KB

    if (n < NT_THRESHOLD) {
        // 小拷贝：使用普通存储
        return memcpy_avx2(dst, src, n);
    }

    unsigned char *d = dst;
    const unsigned char *s = src;

    // 头部：正常拷贝到对齐边界
    while (n > 0 && ((uintptr_t)d & 31)) {
        *d++ = *s++;
        n--;
    }

    // 主体：非临时存储
    while (n >= 32) {
        __m256i v = _mm256_load_si256((__m256i*)s);
        _mm256_stream_si256((__m256i*)d, v);  // NT store

        s += 32;
        d += 32;
        n -= 32;
    }

    // 需要mfence确保NT存储完成
    _mm_sfence();

    // 尾部：正常拷贝
    while (n > 0) {
        *d++ = *s++;
        n--;
    }

    return dst;
}

// 完全NT路径（超大块）
void* memcpy_nt_full(void *dst, const void *src, size_t n) {
    // 使用_mm_stream_load_si128 + _mm_stream_si128
    // 或普通的load + NT store

    // 预取数据到L2（不进入L1）
    for (size_t i = 0; i < n; i += 64) {
        _mm_prefetch((const char*)src + i + 512, _MM_HINT_T1);
    }

    // NT存储主体...

    _mm_sfence();
    return dst;
}
