/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\01_Rocket_Chip_Case_Study.md
 * Line: 1027
 * Language: c
 * Block ID: 15efd494
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 硬件调试接口
#define DEBUG_BASE 0x10003000

struct debug_regs {
    volatile uint32_t control;
    volatile uint32_t status;
    volatile uint64_t pc;
    volatile uint64_t gpr[32];
    volatile uint64_t csr[4096];
};

// 核心转储函数
void coredump(void) {
    struct debug_regs* dbg = (struct debug_regs*)DEBUG_BASE;

    printf("=== Core Dump ===\n");
    printf("PC: 0x%016lx\n", dbg->pc);
    printf("Status: 0x%08x\n", dbg->status);

    for (int i = 0; i < 32; i++) {
        printf("x%-2d: 0x%016lx\n", i, dbg->gpr[i]);
    }

    // 重要CSR
    printf("mstatus: 0x%016lx\n", dbg->csr[0x300]);
    printf("mepc:    0x%016lx\n", dbg->csr[0x341]);
    printf("mcause:  0x%016lx\n", dbg->csr[0x342]);
}

// 性能剖析器
#define SAMPLE_SIZE 10000
uint64_t sample_pc[SAMPLE_SIZE];
uint32_t sample_count = 0;

void sampling_handler(void) {
    if (sample_count < SAMPLE_SIZE) {
        uint64_t mepc;
        asm volatile("csrr %0, mepc" : "=r"(mepc));
        sample_pc[sample_count++] = mepc;
    }
}

void print_profile(void) {
    // 统计每个PC的出现次数
    qsort(sample_pc, sample_count, sizeof(uint64_t), compare_u64);

    uint64_t current = sample_pc[0];
    int count = 1;

    printf("=== Profile ===\n");
    for (int i = 1; i < sample_count; i++) {
        if (sample_pc[i] == current) {
            count++;
        } else {
            printf("0x%016lx: %.2f%% (%d samples)\n",
                   current, 100.0 * count / sample_count, count);
            current = sample_pc[i];
            count = 1;
        }
    }
}
