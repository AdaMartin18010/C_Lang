/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\02_Memcpy_SIMD.md
 * Line: 703
 * Language: c
 * Block ID: 102ade44
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：NT存储后缺少fence
void wrong_nt(void *dst, const void *src, size_t n) {
    while (n >= 32) {
        _mm256_stream_si256(dst, _mm256_load_si256(src));
        // ...
    }
    // ❌ 数据可能还在write-combining buffer
}

// 正确：使用sfence
void correct_nt(void *dst, const void *src, size_t n) {
    while (n >= 32) {
        _mm256_stream_si256(dst, _mm256_load_si256(src));
        // ...
    }
    _mm_sfence();  // ✅ 确保NT存储完成
}
