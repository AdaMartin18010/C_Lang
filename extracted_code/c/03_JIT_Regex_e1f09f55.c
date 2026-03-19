/*
 * Auto-generated from: 03_System_Technology_Domains\03_JIT_Regex.md
 * Line: 404
 * Language: c
 * Block ID: e1f09f55
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 可执行内存可写
void* unsafe_jit = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, ...);

// ✅ 正确: W^X 原则 (Write XOR Execute)
void* safe_jit_compile(size_t code_size) {
    // 1. 分配RW内存
    void *mem = mmap(NULL, code_size, PROT_READ | PROT_WRITE, ...);

    // 2. 写入代码
    generate_code(mem);

    // 3. 切换为RX
    mprotect(mem, code_size, PROT_READ | PROT_EXEC);

    return mem;
}
