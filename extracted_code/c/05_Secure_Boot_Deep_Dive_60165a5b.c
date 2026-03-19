/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 2123
 * Language: c
 * Block ID: 60165a5b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 硬件信任根实现
 * 基于ARM TrustZone和专用安全硬件
 */

/* 安全启动ROM（不可变） */
__attribute__((section(".bootrom"), used))
const uint8_t boot_rom_code[] = {
    /* 启动代码 - 烧录在芯片ROM中 */
    /* 功能：
     * 1. 初始化最小硬件
     * 2. 加载并验证BL1/BL2
     * 3. 永不更新
     */
};

/* PUF实现 */
typedef struct {
    volatile uint32_t *puf_base;
    uint32_t helper_data_size;
    bool enrolled;
} puf_controller_t;

#define PUF_REG_CONTROL     0x00
#define PUF_REG_STATUS      0x04
#define PUF_REG_KEY         0x08
#define PUF_REG_INDEX       0x0C

#define PUF_CTRL_ENROLL     0x01
#define PUF_CTRL_START      0x02
#define PUF_CTRL_GET_KEY    0x04

/* PUF初始化 */
int puf_init(puf_controller_t *puf, void *base_addr) {
    puf->puf_base = (volatile uint32_t *)base_addr;

    /* 检查PUF状态 */
    uint32_t status = puf->puf_base[PUF_REG_STATUS / 4];

    if (status & 0x01) {
        /* PUF已注册 */
        puf->enrolled = true;
    }

    return 0;
}

/* PUF注册 */
int puf_enroll(puf_controller_t *puf) {
    if (puf->enrolled) {
        return -1;  /* 已注册，不能重复 */
    }

    /* 启动注册 */
    puf->puf_base[PUF_REG_CONTROL / 4] = PUF_CTRL_ENROLL;

    /* 等待完成 */
    while (!(puf->puf_base[PUF_REG_STATUS / 4] & 0x02));

    puf->enrolled = true;
    return 0;
}

/* PUF获取密钥 */
int puf_get_key(puf_controller_t *puf,
                const uint8_t *helper_data,
                uint8_t *key_out,
                size_t key_len) {

    if (!puf->enrolled) {
        return -1;
    }

    /* 加载helper data到PUF */
    /* 硬件相关操作... */

    /* 启动密钥重构 */
    puf->puf_base[PUF_REG_CONTROL / 4] = PUF_CTRL_GET_KEY;

    /* 读取密钥 */
    for (size_t i = 0; i < key_len / 4; i++) {
        ((uint32_t *)key_out)[i] = puf->puf_base[PUF_REG_KEY / 4];
    }

    return 0;
}

/* eFuse控制器 */
typedef struct {
    volatile uint32_t *efuse_base;
    uint32_t block_size;
    uint32_t num_blocks;
} efuse_controller_t;

#define EFUSE_CMD_READ      0x01
#define EFUSE_CMD_WRITE     0x02
#define EFUSE_CMD_LOCK      0x04

/* 安全eFuse读取 */
int efuse_secure_read(efuse_controller_t *efuse,
                      uint32_t block,
                      uint8_t *data,
                      size_t len) {
    /* 验证访问权限 */
    if (!is_secure_world()) {
        return -EPERM;
    }

    /* 检查块是否锁定 */
    if (efuse->efuse_base[0x100 + block] & 0x01) {
        /* 已锁定，只能读取 */
    }

    /* 执行读取 */
    for (size_t i = 0; i < len / 4; i++) {
        efuse->efuse_base[0x200 / 4] = EFUSE_CMD_READ | (block << 8);
        while (efuse->efuse_base[0x204 / 4] & 0x01);  /* 等待完成 */
        ((uint32_t *)data)[i] = efuse->efuse_base[0x208 / 4];
    }

    return 0;
}

/* 安全eFuse写入（仅限工厂阶段） */
int efuse_secure_write(efuse_controller_t *efuse,
                       uint32_t block,
                       const uint8_t *data,
                       size_t len) {
    /* 严格的安全检查 */
    if (!is_secure_world()) {
        return -EPERM;
    }

    if (!is_factory_mode()) {
        return -EPERM;  /* 非工厂模式禁止写入 */
    }

    if (efuse->efuse_base[0x100 + block] & 0x01) {
        return -EEXIST;  /* 已锁定 */
    }

    /* 执行写入 */
    for (size_t i = 0; i < len / 4; i++) {
        efuse->efuse_base[0x210 / 4] = ((uint32_t *)data)[i];
        efuse->efuse_base[0x200 / 4] = EFUSE_CMD_WRITE | (block << 8);
        while (efuse->efuse_base[0x204 / 4] & 0x01);
    }

    /* 锁定块（可选） */
    efuse->efuse_base[0x200 / 4] = EFUSE_CMD_LOCK | (block << 8);

    return 0;
}
