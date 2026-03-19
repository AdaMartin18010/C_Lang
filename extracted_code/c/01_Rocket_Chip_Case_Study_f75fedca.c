/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\01_Rocket_Chip_Case_Study.md
 * Line: 495
 * Language: c
 * Block ID: f75fedca
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// RoCC指令封装宏
#ifndef ROCC_INSTRUCTION
#define ROCC_INSTRUCTION(x)
#endif

// 向量加法RoCC指令定义
#define ROCC_VECTOR_ADD 0
#define ROCC_VECTOR_LOAD_A 1
#define ROCC_VECTOR_LOAD_B 2
#define ROCC_VECTOR_STORE 3

// 内联汇编封装
static inline void rocc_vector_load_a(void* addr) {
    asm volatile(".word 0x%0" :: "i"(
        (ROCC_VECTOR_LOAD_A << 25) |
        (1 << 20) |  // xs1 = 1
        (0x0B)       // custom-0 opcode
    ));
}

static inline void rocc_vector_load_b(void* addr) {
    asm volatile(".word 0x%0" :: "i"(
        (ROCC_VECTOR_LOAD_B << 25) |
        (1 << 20) |
        (0x0B)
    ));
}

static inline void rocc_vector_compute(void) {
    asm volatile(".word 0x%0" :: "i"(
        (ROCC_VECTOR_ADD << 25) |
        (0x0B)
    ));
}

static inline void rocc_vector_store(void* addr) {
    asm volatile(".word 0x%0" :: "i"(
        (ROCC_VECTOR_STORE << 25) |
        (1 << 20) |
        (0x0B)
    ));
}

// 高级封装函数
void vector_add_accel(uint64_t* a, uint64_t* b, uint64_t* c, size_t n) {
    const size_t vec_size = 16;  // 加速器向量长度

    for (size_t i = 0; i < n; i += vec_size) {
        // 加载数据到加速器
        rocc_vector_load_a(&a[i]);
        rocc_vector_load_b(&b[i]);

        // 执行计算
        rocc_vector_compute();

        // 存储结果
        rocc_vector_store(&c[i]);
    }
}
