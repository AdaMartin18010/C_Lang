/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\02_Memcpy_SIMD.md
 * Line: 725
 * Language: c
 * Block ID: 8f862592
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：小拷贝使用复杂SIMD
void* wrong_small_copy(void *dst, const void *src, size_t n) {
    // 为8字节数据设置AVX寄存器！
    __m256i v = _mm256_loadu_si256(src);  // 越界读取！
    _mm256_storeu_si256(dst, v);
}

// 正确：小拷贝使用简单方法
void* correct_small_copy(void *dst, const void *src, size_t n) {
    if (n <= 8) {
        // 使用整数拷贝
        *(uint64_t*)dst = *(uint64_t*)src;
    } else {
        // 使用SIMD
    }
}
