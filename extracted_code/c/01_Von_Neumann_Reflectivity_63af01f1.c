/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\01_Von_Neumann_Reflectivity.md
 * Line: 624
 * Language: c
 * Block ID: 63af01f1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：修改后未刷新缓存
void wrong_cache(void) {
    uint8_t *code = mmap_executable(size);
    memcpy(code, new_code, size);
    // ❌ 直接执行可能看到旧指令
    ((void(*)(void))code)();
}

// 正确：刷新指令缓存
void correct_cache(void) {
    memcpy(code, new_code, size);
    __builtin___clear_cache(code, code + size);  // ✅
    ((void(*)(void))code)();
}
