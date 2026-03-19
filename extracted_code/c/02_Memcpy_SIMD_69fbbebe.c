/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\02_Memcpy_SIMD.md
 * Line: 682
 * Language: c
 * Block ID: 69fbbebe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：直接使用AVX指令
void wrong_memcpy(void *dst, const void *src, size_t n) {
    __m256i v = _mm256_load_si256(src);  // ❌ 可能在旧CPU上崩溃
    _mm256_store_si256(dst, v);
}

// 正确：运行时检测
void* safe_memcpy(void *dst, const void *src, size_t n) {
    static MemcpyFunc impl = NULL;

    if (!impl) {
        impl = select_best_impl();  // 检测并选择
    }

    return impl(dst, src, n);
}
