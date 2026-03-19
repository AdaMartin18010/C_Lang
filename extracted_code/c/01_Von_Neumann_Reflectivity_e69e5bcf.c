/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\01_Von_Neumann_Reflectivity.md
 * Line: 607
 * Language: c
 * Block ID: e69e5bcf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：直接修改代码段
void wrong_self_modify(void) {
    void (*func)(void) = some_function;
    *(uint8_t*)func = 0xC3;  // ❌ 段错误！代码段只读
}

// 正确：先修改内存保护
void correct_self_modify(void) {
    void *page = (void*)((unsigned long)func & ~4095);
    mprotect(page, 4096, PROT_READ | PROT_WRITE | PROT_EXEC);
    *(uint8_t*)func = 0xC3;  // ✅ 现在可以修改
}
