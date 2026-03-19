/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 2057
 * Language: c
 * Block ID: 3f0890a1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ARM TrustZone硬件信任根实现
 */

/* TrustZone配置 */
#define TZASC_REGIONS_MAX       8

/* TZASC（TrustZone地址空间控制器）配置 */
typedef struct {
    uint32_t base_addr;
    uint32_t size;
    bool secure_only;
    bool non_secure_access;
    uint32_t permissions;
} tzasc_region_t;

/* TrustZone初始化 */
int trustzone_init(void) {
    /* 1. 配置安全世界中断向量 */
    __asm volatile (
        "ldr r0, =_secure_vector_table\n"
        "mcr p15, 0, r0, c12, c0, 0\n"  /* VBAR */
    );

    /* 2. 配置SCR（安全配置寄存器）*/
    uint32_t scr;
    __asm volatile ("mrc p15, 0, %0, c1, c1, 0" : "=r" (scr));
    scr |= (1 << 0);   /* NS = 1: 非安全位 */
    scr |= (1 << 1);   /* IRQ = 1: FIQ路由到安全世界 */
    scr |= (1 << 2);   /* FIQ = 1: FIQ路由到安全世界 */
    scr |= (1 << 8);   /* HCE = 1: 启用HVC指令 */
    __asm volatile ("mcr p15, 0, %0, c1, c1, 0" : : "r" (scr));

    /* 3. 配置TZASC */
    configure_tzasc();

    /* 4. 配置TZPC（TrustZone保护控制器）*/
    configure_tzpc();

    return 0;
}

/* 配置TZASC */
void configure_tzasc(void) {
    /* 区域0: 安全ROM */
    tzasc_region_t secure_rom = {
        .base_addr = 0x00000000,
        .size = 0x00010000,  /* 64KB */
        .secure_only = true,
        .non_secure_access = false,
        .permissions = 0,  /* 只读 */
    };
    tzasc_configure_region(0, &secure_rom);

    /* 区域1: 安全SRAM */
    tzasc_region_t secure_sram = {
        .base_addr = 0x10000000,
        .size = 0x00080000,  /* 512KB */
        .secure_only = true,
        .non_secure_access = false,
        .permissions = 3,  /* 读写 */
    };
    tzasc_configure_region(1, &secure_sram);

    /* 区域2: 共享内存 */
    tzasc_region_t shared_mem = {
        .base_addr = 0x20000000,
        .size = 0x00040000,  /* 256KB */
        .secure_only = false,
        .non_secure_access = true,
        .permissions = 3,
    };
    tzasc_configure_region(2, &shared_mem);

    /* 区域7: 默认（非安全）*/
    tzasc_region_t default_region = {
        .base_addr = 0x00000000,
        .size = 0xFFFFFFFF,
        .secure_only = false,
        .non_secure_access = true,
        .permissions = 3,
    };
    tzasc_configure_region(7, &default_region);
}

/* 安全监控调用处理 */
uint32_t smc_handler(uint32_t smc_fid,
                     uint32_t x1, uint32_t x2, uint32_t x3,
                     uint32_t x4, void *handle) {
    switch (smc_fid) {
        case SMC_PUF_GET_KEY:
            return smc_puf_get_key(x1, x2, x3);

        case SMC_SECURE_STORAGE_READ:
            return smc_secure_storage_read(x1, x2, x3);

        case SMC_SECURE_STORAGE_WRITE:
            return smc_secure_storage_write(x1, x2, x3);

        case SMC_ATTESTATION_GET_TOKEN:
            return smc_attestation_get_token(x1, x2, x3);

        case SMC_CRYPTO_OPERATION:
            return smc_crypto_operation(x1, x2, x3, x4);

        default:
            return SMC_UNK;
    }
}

/* 从安全世界访问PUF */
uint32_t smc_puf_get_key(uint32_t key_type,
                         uint32_t key_buf_addr,
                         uint32_t key_len) {
    /* 验证非安全世界缓冲区 */
    if (!is_non_secure_memory(key_buf_addr, key_len)) {
        return -EPERM;
    }

    /* 映射非安全缓冲区到安全世界 */
    void *secure_buf = map_ns_buffer(key_buf_addr, key_len);

    /* 派生密钥 */
    uint8_t key[64];
    puf_derive_key(key_type, NULL, 0, 0, key, key_len);

    /* 复制到非安全缓冲区 */
    memcpy(secure_buf, key, key_len);

    /* 清除密钥 */
    secure_memzero(key, sizeof(key));

    /* 解除映射 */
    unmap_ns_buffer(secure_buf, key_len);

    return 0;
}
