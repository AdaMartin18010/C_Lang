/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 281
 * Language: c
 * Block ID: 42bbd884
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * BL1: Boot ROM实现
 * 位于芯片只读存储器，不可更新
 * 负责初始化最基本硬件并加载BL2
 */

/* BL1入口点 - 汇编实现 */
__attribute__((section(".text.bl1_entry")))
void bl1_entry(void) {
    /*
     * 汇编代码（概念性）：
     * 1. 设置异常向量表
     * 2. 配置MMU（如果支持）
     * 3. 初始化基本时钟和电源
     * 4. 跳转到C代码
     */
    bl1_main();
}

/* BL1主函数 */
void bl1_main(void) {
    /* 1. 架构初始化 */
    init_cpu_arch();

    /* 2. 平台早期初始化 */
    plat_early_init();

    /* 3. 验证启动原因 */
    uint32_t boot_reason = get_boot_reason();
    if (boot_reason == BOOT_REASON_WATCHDOG) {
        handle_watchdog_reset();
    }

    /* 4. 配置安全状态 */
    configure_security_state();

    /* 5. 加载BL2到安全RAM */
    image_info_t bl2_info = {0};
    int ret = load_bl2_image(&bl2_info);
    if (ret != 0) {
        panic("BL2 load failed");
    }

    /* 6. 验证BL2签名 */
    ret = authenticate_image(&bl2_info, ROTPK_BL2);
    if (ret != SECURE_BOOT_OK) {
        log_security_event(EVENT_BL2_AUTH_FAIL, ret);
        panic("BL2 authentication failed");
    }

    /* 7. 测量BL2（如果支持测量启动） */
    if (is_measured_boot_enabled()) {
        extend_pcr(PCR_BL2, bl2_info.hash, bl2_info.hash_len);
    }

    /* 8. 移交控制权给BL2 */
    bl1_plat_prepare_bl2_entry(&bl2_info);
    bl2_entry(bl2_info.entry_point);
}

/* 加载BL2镜像 */
int load_bl2_image(image_info_t *info) {
    /* 从启动设备读取BL2（NOR Flash、eMMC、SD等） */
    const boot_device_t *boot_dev = get_boot_device();

    /* 读取镜像头部 */
    image_header_t header;
    boot_dev->read(boot_dev, BL2_OFFSET, sizeof(header), &header);

    /* 验证头部 */
    if (header.magic != IMAGE_MAGIC) {
        return ERR_INVALID_MAGIC;
    }

    /* 分配安全RAM */
    void *load_addr = bl1_alloc_secure_memory(header.image_size);
    if (!load_addr) {
        return -ENOMEM;
    }

    /* 读取完整镜像 */
    boot_dev->read(boot_dev,
                   BL2_OFFSET + header.image_offset,
                   header.image_size,
                   load_addr);

    /* 计算哈希 */
    hash_memory(header.hash_alg, load_addr, header.image_size,
                info->hash, &info->hash_len);

    info->entry_point = (entry_point_t)load_addr;
    info->image_base = load_addr;
    info->image_size = header.image_size;

    return 0;
}

/* 镜像认证 */
int authenticate_image(const image_info_t *info, const uint8_t *rotpk) {
    /* 1. 获取镜像签名 */
    image_header_t *header = (image_header_t *)info->image_base;
    uint8_t *signature = (uint8_t *)info->image_base + header->signature_offset;

    /* 2. 验证签名 */
    switch (header->auth_type) {
        case AUTH_RSA_PSS:
            return verify_rsa_pss(rotpk, info->hash, info->hash_len,
                                  signature, header->signature_size);
        case AUTH_ECDSA:
            return verify_ecdsa(rotpk, info->hash, info->hash_len,
                               signature, header->signature_size);
        default:
            return ERR_SIGNATURE_INVALID;
    }
}
