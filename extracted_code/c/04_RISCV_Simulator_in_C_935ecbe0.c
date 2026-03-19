/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\08_RISCV_CPU_Complete\04_RISCV_Simulator_in_C.md
 * Line: 653
 * Language: c
 * Block ID: 935ecbe0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

//////////////////////////////////////////////////////////////////////////////
// memory.c - 内存管理实现
//////////////////////////////////////////////////////////////////////////////

#include "riscv_sim.h"

// 创建内存
Memory* memory_create(u32 size) {
    Memory *mem = (Memory*)malloc(sizeof(Memory));
    if (!mem) return NULL;

    mem->data = (u8*)calloc(1, size);
    if (!mem->data) {
        free(mem);
        return NULL;
    }

    mem->size = size;
    mem->base_addr = 0;
    return mem;
}

// 销毁内存
void memory_destroy(Memory *mem) {
    if (mem) {
        free(mem->data);
        free(mem);
    }
}

// 地址检查
static bool check_address(RISCV_CPU *cpu, u32 addr, u32 len) {
    if (addr < cpu->mem.base_addr ||
        addr + len > cpu->mem.base_addr + cpu->mem.size) {
        printf("Memory access error: address 0x%08X out of bounds\n", addr);
        cpu->exception = true;
        return false;
    }
    return true;
}

// 读字节
u8 cpu_read_byte(RISCV_CPU *cpu, u32 addr) {
    if (!check_address(cpu, addr, 1)) return 0;
    return cpu->mem.data[addr - cpu->mem.base_addr];
}

// 读半字
u16 cpu_read_half(RISCV_CPU *cpu, u32 addr) {
    if (!check_address(cpu, addr, 2)) return 0;
    u32 offset = addr - cpu->mem.base_addr;
    return (u16)cpu->mem.data[offset] |
           ((u16)cpu->mem.data[offset + 1] << 8);
}

// 读字
u32 cpu_read_word(RISCV_CPU *cpu, u32 addr) {
    if (!check_address(cpu, addr, 4)) return 0;
    u32 offset = addr - cpu->mem.base_addr;
    return (u32)cpu->mem.data[offset] |
           ((u32)cpu->mem.data[offset + 1] << 8) |
           ((u32)cpu->mem.data[offset + 2] << 16) |
           ((u32)cpu->mem.data[offset + 3] << 24);
}

// 写字节
void cpu_write_byte(RISCV_CPU *cpu, u32 addr, u8 value) {
    if (!check_address(cpu, addr, 1)) return;
    cpu->mem.data[addr - cpu->mem.base_addr] = value;
}

// 写半字
void cpu_write_half(RISCV_CPU *cpu, u32 addr, u16 value) {
    if (!check_address(cpu, addr, 2)) return;
    u32 offset = addr - cpu->mem.base_addr;
    cpu->mem.data[offset] = value & 0xFF;
    cpu->mem.data[offset + 1] = (value >> 8) & 0xFF;
}

// 写字
void cpu_write_word(RISCV_CPU *cpu, u32 addr, u32 value) {
    if (!check_address(cpu, addr, 4)) return;
    u32 offset = addr - cpu->mem.base_addr;
    cpu->mem.data[offset] = value & 0xFF;
    cpu->mem.data[offset + 1] = (value >> 8) & 0xFF;
    cpu->mem.data[offset + 2] = (value >> 16) & 0xFF;
    cpu->mem.data[offset + 3] = (value >> 24) & 0xFF;
}

// 从文件加载程序
int cpu_load_program(RISCV_CPU *cpu, const char *filename, u32 addr) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("Error: Cannot open file %s\n", filename);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (addr + size > cpu->mem.base_addr + cpu->mem.size) {
        printf("Error: Program too large for memory\n");
        fclose(fp);
        return -1;
    }

    fread(cpu->mem.data + (addr - cpu->mem.base_addr), 1, size, fp);
    fclose(fp);

    printf("Loaded %ld bytes to address 0x%08X\n", size, addr);
    return 0;
}

// 加载hex文件 (格式: @AAAAAAAADDDDDDDD)
int cpu_load_hex(RISCV_CPU *cpu, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: Cannot open file %s\n", filename);
        return -1;
    }

    char line[256];
    u32 addr = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '@') {
            // 地址行
            sscanf(line + 1, "%x", &addr);
        } else {
            // 数据行
            u32 data;
            char *ptr = line;
            while (sscanf(ptr, "%x", &data) == 1) {
                cpu_write_word(cpu, addr, data);
                addr += 4;
                ptr += 9;  // 跳过"XXXXXXXX "
            }
        }
    }

    fclose(fp);
    return 0;
}
