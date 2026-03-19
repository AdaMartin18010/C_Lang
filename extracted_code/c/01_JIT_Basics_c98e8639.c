/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\01_JIT_Basics.md
 * Line: 765
 * Language: c
 * Block ID: c98e8639
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：生成代码后未刷新指令缓存
void* compile_without_flush(void) {
    void *code = mmap(..., PROT_EXEC);
    // 写入代码...
    return code;  // ❌ 可能执行旧数据！
}

// 正确：刷新指令缓存
void* compile_with_flush(void) {
    void *code = mmap(..., PROT_EXEC);
    // 写入代码...
    __builtin___clear_cache(code, (char*)code + size);  // ✅
    return code;
}
