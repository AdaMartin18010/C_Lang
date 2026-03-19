/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 2199
 * Language: c
 * Block ID: c666e36f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * RISC-V硬件信任根实现
 * 基于PMP（物理内存保护）
 */

/* PMP配置 */
#define PMP_ENTRIES             16
#define PMP_ADDR_SHIFT          2

/* PMP配置寄存器 */
typedef enum {
    PMP_OFF = 0,
    PMP_TOR = 1,
    PMP_NA4 = 2,
    PMP_NAPOT = 3,
} pmp_a_field_t;

#define PMP_R       (1 << 0)
#define PMP_W       (1 << 1)
#define PMP_X       (1 << 2)
#define PMP_L       (1 << 7)    /* 锁定 */

/* 安全区域定义 */
typedef struct {
    uintptr_t start;
    size_t size;
    uint8_t permissions;    /* R/W/X */
    bool locked;
} pmp_region_t;

/* 初始化PMP */
int pmp_init(void) {
    /* 禁用所有PMP条目 */
    for (int i = 0; i < PMP_ENTRIES; i++) {
        write_csr(CSR_PMPCFG0 + i, 0);
    }

    /* 配置安全区域 */

    /* 条目0: 安全ROM（只读，锁定）*/
    pmp_configure_region(0, 0x00000000, 0x10000, PMP_R | PMP_L, PMP_NAPOT);

    /* 条目1: 安全SRAM（读写，锁定）*/
    pmp_configure_region(1, 0x80000000, 0x40000, PMP_R | PMP_W | PMP_L, PMP_NAPOT);

    /* 条目2: 安全代码（执行，锁定）*/
    pmp_configure_region(2, 0x80100000, 0x10000, PMP_R | PMP_X | PMP_L, PMP_NAPOT);

    /* 条目15: 默认拒绝（锁定）*/
    pmp_configure_region(15, 0xFFFFFFFF, 0, PMP_L, PMP_NAPOT);

    return 0;
}

/* 配置PMP区域 */
void pmp_configure_region(int entry,
                          uintptr_t addr,
                          size_t size,
                          uint8_t permissions,
                          pmp_a_field_t a_field) {
    /* 计算PMP地址寄存器值 */
    uintptr_t pmpaddr;

    if (a_field == PMP_NAPOT) {
        /* NAPOT: 地址 | (size-1) >> 3 */
        pmpaddr = (addr >> PMP_ADDR_SHIFT) | ((size - 1) >> 3);
    } else if (a_field == PMP_TOR) {
        /* TOR: 地址 >> 2 */
        pmpaddr = addr >> PMP_ADDR_SHIFT;
    } else {
        pmpaddr = addr >> PMP_ADDR_SHIFT;
    }

    /* 写入PMP地址寄存器 */
    write_csr(CSR_PMPADDR0 + entry, pmpaddr);

    /* 写入PMP配置寄存器 */
    uint8_t cfg = permissions | (a_field << 3);
    uint32_t cfg_reg = read_csr(CSR_PMPCFG0 + (entry / 4));
    cfg_reg &= ~(0xFF << ((entry % 4) * 8));
    cfg_reg |= (cfg << ((entry % 4) * 8));
    write_csr(CSR_PMPCFG0 + (entry / 4), cfg_reg);
}

/* RISC-V MultiZone实现 */
int multizone_init(void) {
    /* 配置多个安全区域（MultiZone）*/

    /* Zone 0: 信任根 */
    pmp_configure_region(0, 0x00000000, 0x20000, PMP_R | PMP_X | PMP_L, PMP_NAPOT);

    /* Zone 1: 可信应用 */
    pmp_configure_region(1, 0x20000000, 0x10000, PMP_R | PMP_W | PMP_X, PMP_NAPOT);

    /* Zone 2: 不可信应用 */
    pmp_configure_region(2, 0x30000000, 0x10000, PMP_R | PMP_W | PMP_X, PMP_NAPOT);

    /* 设置中断委托 */
    uint32_t mideleg = 0;
    write_csr(CSR_MIDELEG, mideleg);  /* 所有中断到M模式 */

    return 0;
}

/* 处理ecall（环境调用）*/
void handle_ecall(void) {
    uint32_t a0, a1, a2, a3, a7;

    /* 读取参数 */
    __asm volatile ("mv %0, a0" : "=r" (a0));
    __asm volatile ("mv %0, a1" : "=r" (a1));
    __asm volatile ("mv %0, a2" : "=r" (a2));
    __asm volatile ("mv %0, a3" : "=r" (a3));
    __asm volatile ("mv %0, a7" : "=r" (a7));

    switch (a7) {
        case SBI_EXT_PUF_GET_KEY:
            a0 = sbi_puf_get_key(a0, a1, a2);
            break;

        case SBI_EXT_SECURE_STORAGE:
            a0 = sbi_secure_storage_op(a0, a1, a2, a3);
            break;

        case SBI_EXT_ATTESTATION:
            a0 = sbi_attestation(a0, a1, a2);
            break;

        default:
            a0 = SBI_ERR_NOT_SUPPORTED;
    }

    /* 返回结果 */
    __asm volatile ("mv a0, %0" : : "r" (a0));

    /* 返回 */
    __asm volatile ("mret");
}
