/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\04_Memory_Protection_Unit.md
 * Line: 1115
 * Language: c
 * Block ID: 289211b8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * RISC-V PMP 配置示例
 */

#include <stdint.h>

// PMP CSR编号
#define CSR_PMPCFG0     0x3A0
#define CSR_PMPCFG1     0x3A1
#define CSR_PMPCFG2     0x3A2
#define CSR_PMPCFG3     0x3A3
#define CSR_PMPADDR0    0x3B0
#define CSR_PMPADDR1    0x3B1
// ... 到 CSR_PMPADDR63 0x3EF

/*
 * 读取/写入PMP配置CSR
 */
#define read_csr(csr) ({ \
    uint64_t __v; \
    __asm__ __volatile__ ("csrr %0, " #csr : "=r" (__v)); \
    __v; \
})

#define write_csr(csr, val) ({ \
    uint64_t __v = (uint64_t)(val); \
    __asm__ __volatile__ ("csrw " #csr ", %0" : : "rK" (__v)); \
})

#define set_csr(csr, val) ({ \
    uint64_t __v = (uint64_t)(val); \
    __asm__ __volatile__ ("csrs " #csr ", %0" : : "rK" (__v)); \
})

/*
 * PMP配置结构
 */
typedef struct {
    uint8_t r : 1;      // 读权限
    uint8_t w : 1;      // 写权限
    uint8_t x : 1;      // 执行权限
    uint8_t a : 2;      // 地址匹配模式 (0=OFF, 1=TOR, 2=NA4, 3=NAPOT)
    uint8_t reserved : 2;
    uint8_t l : 1;      // 锁定
} pmpcfg_t;

#define PMP_A_OFF       0
#define PMP_A_TOR       1
#define PMP_A_NA4       2
#define PMP_A_NAPOT     3

/*
 * 将地址转换为NAPOT格式的pmpaddr值
 */
uint64_t pmpaddr_napot_encode(uint64_t addr, uint64_t size) {
    // addr必须是size对齐的
    // size必须是2的幂次
    return (addr >> 2) | ((size >> 3) - 1);
}

/*
 * 配置单个PMP条目
 */
void pmp_config_entry(uint32_t entry_num,
                      uint64_t pmpaddr_val,
                      pmpcfg_t cfg) {
    // 写入pmpaddr
    write_csr(CSR_PMPADDR0 + entry_num, pmpaddr_val);

    // 计算pmpcfg中的位置
    uint32_t cfg_reg = entry_num / 4;
    uint32_t cfg_shift = (entry_num % 4) * 8;

    // 读取当前cfg值
    uint64_t cfg_val = read_csr(CSR_PMPCFG0 + cfg_reg);

    // 清除旧的配置，设置新的配置
    cfg_val &= ~(0xFFULL << cfg_shift);
    cfg_val |= ((uint64_t)(*(uint8_t *)&cfg)) << cfg_shift;

    // 写回
    write_csr(CSR_PMPCFG0 + cfg_reg, cfg_val);
}

/*
 * 典型的嵌入式系统PMP配置
 */
void pmp_setup_embedded(void) {
    pmpcfg_t cfg;

    /*
     * Entry 0: 闪存 (代码) - 只读执行
     * 范围: 0x20000000 - 0x20080000 (512KB)
     */
    cfg.r = 1; cfg.w = 0; cfg.x = 1;
    cfg.a = PMP_A_NAPOT; cfg.l = 0;
    pmp_config_entry(0,
        pmpaddr_napot_encode(0x20000000, 512 * 1024),
        cfg);

    /*
     * Entry 1: SRAM (数据) - 读写
     * 范围: 0x80000000 - 0x80040000 (256KB)
     */
    cfg.r = 1; cfg.w = 1; cfg.x = 0;
    cfg.a = PMP_A_NAPOT; cfg.l = 0;
    pmp_config_entry(1,
        pmpaddr_napot_encode(0x80000000, 256 * 1024),
        cfg);

    /*
     * Entry 2: 外设 - 读写 (不可执行)
     * 范围: 0x10000000 - 0x20000000 (256MB)
     */
    cfg.r = 1; cfg.w = 1; cfg.x = 0;
    cfg.a = PMP_A_NAPOT; cfg.l = 0;
    pmp_config_entry(2,
        pmpaddr_napot_encode(0x10000000, 256 * 1024 * 1024),
        cfg);

    /*
     * Entry 3: 栈保护 - 无访问 (作为guard page)
     * 范围: 0x8003F000 - 0x80040000 (4KB在SRAM末尾)
     */
    cfg.r = 0; cfg.w = 0; cfg.x = 0;
    cfg.a = PMP_A_NAPOT; cfg.l = 0;
    pmp_config_entry(3,
        pmpaddr_napot_encode(0x8003F000, 4 * 1024),
        cfg);

    /*
     * Entry 4: M模式代码 - 锁定
     * 确保即使后续代码也无法修改此配置
     */
    cfg.r = 1; cfg.w = 1; cfg.x = 1;
    cfg.a = PMP_A_NAPOT; cfg.l = 1;  // 锁定!
    pmp_config_entry(4,
        pmpaddr_napot_encode(0x20000000, 64 * 1024),
        cfg);
}

/*
 * TOR模式配置示例
 */
void pmp_setup_tor_example(void) {
    pmpcfg_t cfg;

    /*
     * 使用TOR模式创建连续区域
     *
     * Entry 0: 从0到8KB (保留，无访问)
     * Entry 1: 从8KB到32KB (代码，只读执行)
     * Entry 2: 从32KB到64KB (数据，读写)
     */

    // Entry 0: 设置上界 (不包含在区域中)
    // pmpaddr0 = 0x2000 (8KB >> 2)
    cfg.r = 0; cfg.w = 0; cfg.x = 0;
    cfg.a = PMP_A_OFF; cfg.l = 0;  // 先禁用
    pmp_config_entry(0, 0x2000 >> 2, cfg);

    // Entry 1: 8KB到32KB (代码)
    // pmpaddr1 = 0x8000 (32KB >> 2)
    cfg.r = 1; cfg.w = 0; cfg.x = 1;
    cfg.a = PMP_A_TOR; cfg.l = 0;
    pmp_config_entry(1, 0x8000 >> 2, cfg);

    // Entry 2: 32KB到64KB (数据)
    // pmpaddr2 = 0x10000 (64KB >> 2)
    cfg.r = 1; cfg.w = 1; cfg.x = 0;
    cfg.a = PMP_A_TOR; cfg.l = 0;
    pmp_config_entry(2, 0x10000 >> 2, cfg);
}

/*
 * PMP异常处理
 */
void handle_pmp_fault(void) {
    // 读取mcause确定异常类型
    uint64_t mcause;
    __asm__ volatile ("csrr %0, mcause" : "=r" (mcause));

    if ((mcause & 0xFF) == 1) {
        // Instruction access fault
        uint64_t mepc;
        __asm__ volatile ("csrr %0, mepc" : "=r" (mepc));
        printf("PMP Instruction fault at 0x%lx\n", mepc);
    }
    else if ((mcause & 0xFF) == 5) {
        // Load access fault
        uint64_t mtval;
        __asm__ volatile ("csrr %0, mtval" : "=r" (mtval));
        printf("PMP Load fault at 0x%lx\n", mtval);
    }
    else if ((mcause & 0xFF) == 7) {
        // Store/AMO access fault
        uint64_t mtval;
        __asm__ volatile ("csrr %0, mtval" : "=r" (mtval));
        printf("PMP Store fault at 0x%lx\n", mtval);
    }
}
