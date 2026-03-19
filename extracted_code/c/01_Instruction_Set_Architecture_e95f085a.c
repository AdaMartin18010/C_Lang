/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\01_Instruction_Set_Architecture.md
 * Line: 104
 * Language: c
 * Block ID: e95f085a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// x86-64架构的状态空间（简化）

typedef struct {
    // 程序计数器
    uint64_t rip;

    // 通用寄存器
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;

    // RFLAGS寄存器
    struct {
        uint64_t cf : 1;  // 进位标志
        uint64_t pf : 1;  // 奇偶标志
        uint64_t af : 1;  // 辅助进位
        uint64_t zf : 1;  // 零标志
        uint64_t sf : 1;  // 符号标志
        uint64_t of : 1;  // 溢出标志
        // ... 其他标志位
    } rflags;

    // 内存（虚拟地址空间）
    uint8_t *memory;
    uint64_t mem_size;

    // 浮点和SIMD寄存器（SSE/AVX）
    union {
        float xmm_f[32][4];
        double xmm_d[32][2];
        uint8_t xmm_b[32][16];
    } xmm;

} X86_64_State;
