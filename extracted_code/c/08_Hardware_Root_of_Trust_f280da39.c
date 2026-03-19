/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 1312
 * Language: c
 * Block ID: f280da39
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 安全启动ROM实现
 * 基于ARM Cortex-M架构
 */

/* 链接脚本定义 */
__attribute__((section(".bootrom")))
__attribute__((aligned(4)))

/* ROM入口点 */
__attribute__((naked))
__attribute__((section(".bootrom.entry")))
void rom_entry(void) {
    __asm volatile (
        /* 禁用中断 */
        "cpsid i\n"

        /* 设置初始栈指针 */
        "ldr r0, =_stack_top\n"
        "msr msp, r0\n"

        /* 清除通用寄存器 */
        "mov r0, #0\n"
        "mov r1, #0\n"
        "mov r2, #0\n"
        "mov r3, #0\n"
        "mov r4, #0\n"
        "mov r5, #0\n"
        "mov r6, #0\n"
        "mov r7, #0\n"

        /* 跳转到C代码 */
        "bl rom_main\n"

        /* 不应该到达这里 */
        "b .\n"
    );
}

/* ROM主函数 */
__attribute__((section(".bootrom.text")))
void rom_main(void) {
    /* 1. 验证启动原因 */
    uint32_t reset_reason = get_reset_reason();

    switch (reset_reason) {
        case RESET_POWER_ON:
            /* 正常启动 */
            break;

        case RESET_WATCHDOG:
            /* 看门狗复位，可能检测到故障 */
            handle_watchdog_reset();
            break;

        case RESET_SOFTWARE:
            /* 软件复位 */
            break;

        case RESET_SECURITY:
            /* 安全违规导致的复位 */
            handle_security_violation();
            break;

        default:
            /* 未知原因，安全起见停止 */
            enter_secure_failure_mode();
    }

    /* 2. 初始化基本硬件 */
    init_minimal_hardware();

    /* 3. 验证eFuse配置 */
    if (verify_efuse_config() != 0) {
        enter_secure_failure_mode();
    }

    /* 4. 检查调试状态 */
    if (is_debug_enabled() && !is_debug_allowed()) {
        /* 调试被禁用但尝试启用 */
        enter_secure_failure_mode();
    }

    /* 5. 加载并验证下一阶段（BL1）*/
    if (load_and_verify_bl1() != 0) {
        /* 验证失败，尝试恢复 */
        if (attempt_recovery() != 0) {
            enter_secure_failure_mode();
        }
    }

    /* 6. 移交控制权 */
    jump_to_bl1();

    /* 不应该到达这里 */
    while (1);
}

/* 加载并验证BL1 */
__attribute__((section(".bootrom.text")))
int load_and_verify_bl1(void) {
    /* 1. 定位BL1镜像 */
    const uint8_t *bl1_flash_addr = (const uint8_t *)BL1_FLASH_OFFSET;

    /* 2. 读取镜像头部 */
    image_header_t header;
    memcpy(&header, bl1_flash_addr, sizeof(header));

    /* 3. 验证头部魔数 */
    if (header.magic != IMAGE_MAGIC) {
        return -EINVALID_IMAGE;
    }

    /* 4. 验证版本（反滚动）*/
    uint32_t current_version = get_current_security_version();
    if (header.version < current_version) {
        return -EROLLBACK_ATTEMPT;
    }

    /* 5. 计算哈希 */
    uint8_t computed_hash[32];
    sha256_rom(bl1_flash_addr + header.header_size,
               header.image_size,
               computed_hash);

    /* 6. 验证签名 */
    const uint8_t *pk = get_rot_public_key();
    if (ed25519_verify(header.signature, computed_hash, 32, pk) != 0) {
        /* 添加随机延迟防止时序攻击 */
        random_delay();
        return -ESIGNATURE_INVALID;
    }

    /* 7. 复制到SRAM */
    memcpy((void *)BL1_SRAM_ADDR,
           bl1_flash_addr + header.header_size,
           header.image_size);

    /* 8. 验证复制完整性 */
    uint8_t sram_hash[32];
    sha256_rom((const uint8_t *)BL1_SRAM_ADDR, header.image_size, sram_hash);

    if (memcmp(computed_hash, sram_hash, 32) != 0) {
        return -EMEMORY_CORRUPTION;
    }

    /* 9. 更新安全版本 */
    set_security_version(header.version);

    /* 10. 锁定BL1内存区域 */
    lock_bl1_memory_region();

    return 0;
}

/* 进入安全故障模式 */
__attribute__((section(".bootrom.text")))
void enter_secure_failure_mode(void) {
    /* 1. 禁用所有外部接口 */
    disable_all_peripherals();

    /* 2. 清除敏感数据 */
    clear_sensitive_registers();

    /* 3. 记录故障原因 */
    log_failure_reason();

    /* 4. 通知外部（如果安全）*/
    signal_failure_external();

    /* 5. 无限循环或安全复位 */
    while (1) {
        /* 等待看门狗复位 */
        __WFI();
    }
}

/* 恢复模式 */
__attribute__((section(".bootrom.text")))
int attempt_recovery(void) {
    /* 检查是否有恢复镜像 */
    if (!recovery_image_exists()) {
        return -ENORECOVERY;
    }

    /* 验证恢复镜像 */
    if (verify_recovery_image() != 0) {
        return -EINVALID_RECOVERY;
    }

    /* 加载恢复镜像 */
    load_recovery_image();

    /* 进入恢复模式 */
    jump_to_recovery();

    return 0;  /* 不应该到达 */
}
