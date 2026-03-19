/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\08_RISCV_CPU_Complete\04_RISCV_Simulator_in_C.md
 * Line: 806
 * Language: c
 * Block ID: 7eb528cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

//////////////////////////////////////////////////////////////////////////////
// main.c - RISC-V模拟器主程序
//////////////////////////////////////////////////////////////////////////////

#include "riscv_sim.h"
#include <time.h>

// 创建CPU
RISCV_CPU* cpu_create(u32 mem_size) {
    RISCV_CPU *cpu = (RISCV_CPU*)calloc(1, sizeof(RISCV_CPU));
    if (!cpu) return NULL;

    cpu->mem.data = (u8*)calloc(1, mem_size);
    if (!cpu->mem.data) {
        free(cpu);
        return NULL;
    }

    cpu->mem.size = mem_size;
    cpu->mem.base_addr = 0;
    cpu->running = false;
    cpu->exception = false;

    cpu_reset(cpu);
    return cpu;
}

// 销毁CPU
void cpu_destroy(RISCV_CPU *cpu) {
    if (cpu) {
        free(cpu->mem.data);
        free(cpu);
    }
}

// 复位CPU
void cpu_reset(RISCV_CPU *cpu) {
    memset(&cpu->regs, 0, sizeof(cpu->regs));
    memset(&cpu->csr, 0, sizeof(cpu->csr));
    cpu->regs.pc = 0;
    cpu->running = true;
    cpu->exception = false;
}

// 取指
u32 cpu_fetch(RISCV_CPU *cpu) {
    return cpu_read_word(cpu, cpu->regs.pc);
}

// 单步执行
void cpu_step(RISCV_CPU *cpu) {
    // 取指
    u32 inst_raw = cpu_fetch(cpu);

    // 译码
    Instruction inst;
    cpu_decode(inst_raw, &inst);

    // 执行
    cpu_execute(cpu, &inst);
}

// 运行多个周期
void cpu_run(RISCV_CPU *cpu, u32 max_cycles) {
    clock_t start = clock();
    u32 cycles = 0;

    while (cpu->running && !cpu->exception && cycles < max_cycles) {
        cpu_step(cpu);
        cycles++;

        // 调试: 每1000条指令打印状态
        if (cycles % 1000 == 0) {
            // cpu_dump_regs(cpu);
        }
    }

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\n========================================\n");
    printf("Execution completed:\n");
    printf("  Cycles executed: %u\n", cycles);
    printf("  Time elapsed: %.3f seconds\n", elapsed);
    printf("  Simulation speed: %.0f instructions/second\n", cycles / elapsed);
    printf("========================================\n");
}

// 寄存器访问
u32 cpu_read_reg(RISCV_CPU *cpu, u32 reg) {
    if (reg == 0) return 0;  // x0恒为0
    return cpu->regs.x[reg];
}

void cpu_write_reg(RISCV_CPU *cpu, u32 reg, u32 value) {
    if (reg != 0) {  // x0不能写入
        cpu->regs.x[reg] = value;
    }
}

// 打印寄存器
void cpu_dump_regs(RISCV_CPU *cpu) {
    const char *reg_names[] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
    };

    printf("\n===== Register File =====\n");
    printf("PC = 0x%08X\n", cpu->regs.pc);

    for (int i = 0; i < NUM_REGS; i += 4) {
        for (int j = 0; j < 4 && (i + j) < NUM_REGS; j++) {
            int reg = i + j;
            printf("x%-2d(%s) = 0x%08X  ", reg, reg_names[reg], cpu->regs.x[reg]);
        }
        printf("\n");
    }
}

// 打印内存
void cpu_dump_memory(RISCV_CPU *cpu, u32 addr, u32 len) {
    printf("\n===== Memory Dump [0x%08X - 0x%08X] =====\n", addr, addr + len);

    for (u32 i = 0; i < len; i += 16) {
        printf("%08X: ", addr + i);

        // 打印hex
        for (u32 j = 0; j < 16; j++) {
            if (i + j < len) {
                printf("%02X ", cpu_read_byte(cpu, addr + i + j));
            } else {
                printf("   ");
            }
        }

        printf(" |");

        // 打印ASCII
        for (u32 j = 0; j < 16 && (i + j) < len; j++) {
            u8 c = cpu_read_byte(cpu, addr + i + j);
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }

        printf("|\n");
    }
}

// 测试程序 - 计算斐波那契数列
const u32 fibonacci_program[] = {
    // fib(n):
    // x10 = n, x11 = result
    0x00a00513,  // addi x10, x0, 10   # n = 10
    0x00100593,  // addi x11, x0, 1    # a = 1
    0x00100613,  // addi x12, x0, 1    # b = 1
    0x00000693,  // addi x13, x0, 0    # i = 0
    0x00a6f863,  // bleu x10, x13, done # if n <= i, goto done
    0x00b006b3,  // add  x13, x11, x0  # temp = a
    0x00c585b3,  // add  x11, x11, x12 # a = a + b
    0x00d00633,  // add  x12, x13, x0  # b = temp
    0x00168693,  // addi x13, x13, 1   # i++
    0xfe0006e3,  // beq  x0, x0, loop  # goto loop
    0x00000073,  // ecall              # system call
    0x0000006f,  // jal  x0, 0         # infinite loop
};

int main(int argc, char *argv[]) {
    printf("====================================\n");
    printf("   RISC-V RV32I Simulator (C)\n");
    printf("====================================\n\n");

    // 创建CPU (64KB内存)
    RISCV_CPU *cpu = cpu_create(64 * 1024);
    if (!cpu) {
        printf("Error: Failed to create CPU\n");
        return 1;
    }

    // 加载测试程序
    if (argc > 1) {
        // 从文件加载
        if (cpu_load_program(cpu, argv[1], 0) != 0) {
            printf("Using built-in test program\n");
            for (int i = 0; i < sizeof(fibonacci_program)/4; i++) {
                cpu_write_word(cpu, i * 4, fibonacci_program[i]);
            }
        }
    } else {
        printf("No input file provided, using built-in test program\n");
        for (int i = 0; i < sizeof(fibonacci_program)/4; i++) {
            cpu_write_word(cpu, i * 4, fibonacci_program[i]);
        }
    }

    printf("\nInitial state:\n");
    cpu_dump_regs(cpu);

    printf("\n----- Starting execution -----\n");

    // 运行程序
    cpu_run(cpu, 1000000);

    printf("\nFinal state:\n");
    cpu_dump_regs(cpu);

    // 显示结果
    printf("\nResult: fib(10) = %d\n", cpu_read_reg(cpu, REG_A0));

    // 清理
    cpu_destroy(cpu);

    return 0;
}
