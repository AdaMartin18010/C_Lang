/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\01_JIT_Basics.md
 * Line: 802
 * Language: c
 * Block ID: 8dbfb540
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 现代系统要求W^X（写或执行，不同时）

// 错误：同时可写可执行（安全漏洞）
void wrong_protection(void) {
    mmap(..., PROT_READ | PROT_WRITE | PROT_EXEC, ...);  // ❌ RWX
}

// 正确：先写后执行
void correct_protection(void) {
    // 方法1：先RW，后切换为RE
    void *mem = mmap(..., PROT_READ | PROT_WRITE, ...);
    // 写入代码...
    mprotect(mem, size, PROT_READ | PROT_EXEC);  // ✅

    // 方法2：双重映射
    int fd = memfd_create("jit", 0);
    // 写入fd...
    void *exec = mmap(..., PROT_READ | PROT_EXEC, MAP_SHARED, fd, 0);
    void *write = mmap(..., PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}
