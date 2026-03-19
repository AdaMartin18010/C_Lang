/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 2353
 * Language: c
 * Block ID: ca2d16e9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * RISC-V MultiZone安全启动实现
 * 基于RISC-V特权架构和PMP（物理内存保护）
 */

/* RISC-V CSR定义 */
#define CSR_MSTATUS     0x300
#define CSR_MISA        0x301
#define CSR_MEDELEG     0x302
#define CSR_MIDELEG     0x303
#define CSR_MIE         0x304
#define CSR_MTVEC       0x305
#define CSR_MEPC        0x341
#define CSR_MCAUSE      0x342
#define CSR_PMPADDR0    0x3B0
#define CSR_PMPCFG0     0x3A0

/* PMP配置 */
typedef enum {
    PMP_OFF     = 0,
    PMP_TOR     = 1,    /* Top of Range */
    PMP_NA4     = 2,    /* Naturally aligned 4-byte */
    PMP_NAPOT   = 3,    /* Naturally aligned power-of-2 */
} pmp_a_field_t;

#define PMP_R       (1 << 0)
#define PMP_W       (1 << 1)
#define PMP_X       (1 << 2)
#define PMP_L       (1 << 7)    /* 锁定 */

/* Zone配置 */
typedef struct {
    uint32_t zone_id;
    uintptr_t start_addr;
    size_t size;
    uint32_t pmp_flags;
    void *entry_point;
    uint8_t signature[64];
} zone_config_t;

/* MultiZone配置 */
typedef struct {
    uint32_t magic;             /* "MZON" */
    uint32_t version;
    uint32_t num_zones;
    zone_config_t zones[4];
    uint8_t manifest_signature[64];
} multizone_manifest_t;

#define MZ_MAGIC    0x4E4F5A4D  /* "MZON" */

/* 配置PMP */
void configure_pmp(const zone_config_t *zones, int num_zones) {
    /* PMP条目0-3：区域配置 */
    for (int i = 0; i < num_zones && i < 4; i++) {
        uint32_t pmpcfg = zones[i].pmp_flags | (PMP_NAPOT << 3);

        /* 计算NAPOT地址 */
        uint32_t pmpaddr = ((zones[i].start_addr >> 2) |
                           ((zones[i].size - 1) >> 3));

        write_csr(CSR_PMPADDR0 + i, pmpaddr);

        /* 配置寄存器 */
        uint32_t cfg_shift = (i % 4) * 8;
        uint32_t cfg_mask = 0xFF << cfg_shift;
        uint32_t cfg_reg = read_csr(CSR_PMPCFG0 + (i / 4));
        cfg_reg = (cfg_reg & ~cfg_mask) | ((pmpcfg << cfg_shift) & cfg_mask);
        write_csr(CSR_PMPCFG0 + (i / 4), cfg_reg);
    }

    /* PMP条目15：默认拒绝 */
    write_csr(CSR_PMPADDR0 + 15, 0xFFFFFFFF);
    uint32_t cfg15 = PMP_L;  /* 锁定，无权限 */
    uint32_t cfg_reg = read_csr(CSR_PMPCFG0 + 3);
    cfg_reg = (cfg_reg & ~(0xFF << 24)) | (cfg15 << 24);
    write_csr(CSR_PMPCFG0 + 3, cfg_reg);
}

/* 验证Zone镜像 */
int verify_zone_image(const zone_config_t *zone) {
    /* 计算镜像哈希 */
    uint8_t hash[32];
    sha256((void *)zone->start_addr, zone->size, hash);

    /* 验证签名 */
    if (ed25519_verify(zone->signature, hash, 32, root_pubkey) != 0) {
        return -1;
    }

    return 0;
}

/* MultiZone启动 */
void multizone_boot(void) {
    /* 1. 读取manifest */
    multizone_manifest_t *manifest = (multizone_manifest_t *)MANIFEST_ADDR;

    if (manifest->magic != MZ_MAGIC) {
        panic("Invalid MultiZone manifest");
    }

    /* 2. 验证manifest签名 */
    if (ed25519_verify(manifest->manifest_signature,
                       (uint8_t *)manifest,
                       offsetof(multizone_manifest_t, manifest_signature),
                       root_pubkey) != 0) {
        panic("Manifest signature verification failed");
    }

    /* 3. 验证每个zone */
    for (int i = 0; i < manifest->num_zones; i++) {
        if (verify_zone_image(&manifest->zones[i]) != 0) {
            panic("Zone %d verification failed", i);
        }
    }

    /* 4. 配置PMP */
    configure_pmp(manifest->zones, manifest->num_zones);

    /* 5. 启动Zone 0（主zone） */
    void (*zone0_entry)(void) = (void (*)(void))manifest->zones[0].entry_point;
    zone0_entry();
}

/* Zone间通信 */
int zone_send_message(uint32_t target_zone,
                      const void *data,
                      size_t len) {
    /* 通过共享内存和信号量 */
    /* 验证目标zone存在 */
    /* 执行权限检查 */

    shared_mem_t *shm = get_zone_shm(target_zone);

    /* 等待可用 */
    while (shm->status != SHM_EMPTY);

    /* 复制数据 */
    if (len > SHM_MAX_SIZE) {
        return -EMSGSIZE;
    }

    memcpy(shm->data, data, len);
    shm->size = len;
    shm->status = SHM_READY;

    /* 触发中断通知目标zone */
    trigger_software_interrupt(target_zone);

    return 0;
}
