/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 736
 * Language: c
 * Block ID: de5bda90
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * BL33: Non-Secure World Bootloader
 * 典型的U-Boot或UEFI实现
 */

/* 设备树配置（用于传递安全信息给内核） */
void bl33_prepare_dt(void *dtb) {
    /* 添加安全启动相关信息到设备树 */

    /* 1. 添加TPM节点 */
    fdt_add_tpm_node(dtb);

    /* 2. 添加保留内存区域（安全内存） */
    fdt_add_reserved_memory(dtb, "optee_core",
                            OPTEE_RAM_START, OPTEE_RAM_SIZE);

    /* 3. 添加安全启动状态 */
    fdt_setprop_string(dtb, "/chosen", "security-model", "trustzone");

    /* 4. 添加测量启动日志（如果存在） */
    if (has_event_log()) {
        void *log_addr = get_event_log_address();
        size_t log_size = get_event_log_size();
        fdt_setprop_u64(dtb, "/chosen", "linux,tpm-event-log",
                        (uint64_t)log_addr);
        fdt_setprop_u32(dtb, "/chosen", "linux,tpm-event-log-size",
                        log_size);
    }
}

/* 加载操作系统内核 */
int load_kernel(const char *kernel_path, boot_params_t *params) {
    /* 1. 从存储设备读取内核 */
    size_t kernel_size;
    void *kernel_buf = load_file(kernel_path, &kernel_size);
    if (!kernel_buf) {
        return -ENOENT;
    }

    /* 2. 验证内核签名（如果启用安全启动） */
    if (is_secure_boot_enabled()) {
        if (verify_kernel_signature(kernel_buf, kernel_size) != 0) {
            printf("ERROR: Kernel signature verification failed!\n");
            return -EPERM;
        }
        printf("Kernel signature verified OK\n");
    }

    /* 3. 测量内核 */
    if (is_measured_boot_enabled()) {
        uint8_t kernel_hash[32];
        sha256(kernel_buf, kernel_size, kernel_hash);
        tpm_extend(PCR_KERNEL, kernel_hash, 32);
    }

    /* 4. 解压内核（如果是压缩格式） */
    void *decompressed = decompress_kernel(kernel_buf, kernel_size);

    /* 5. 加载设备树 */
    void *dtb = load_dtb();
    bl33_prepare_dt(dtb);

    /* 6. 设置启动参数 */
    params->kernel_addr = decompressed;
    params->dtb_addr = dtb;
    params->cmdline = build_kernel_cmdline();

    return 0;
}

/* 跳转到内核 */
void boot_kernel(boot_params_t *params) {
    /* 刷新缓存 */
    clean_dcache_range(params->kernel_addr, KERNEL_MAX_SIZE);
    clean_dcache_range(params->dtb_addr, DTB_MAX_SIZE);

    /* 禁用MMU和缓存 */
    disable_mmu();

    /* 设置寄存器 */
    __asm__ volatile (
        "mov x0, #0\n"
        "mov x1, %0\n"      /* 机器类型或设备树 */
        "mov x2, %1\n"      /* 内核参数 */
        "mov x3, #0\n"
        "br  %2\n"          /* 跳转到内核 */
        :
        : "r" (params->dtb_addr),
          "r" (params->cmdline),
          "r" (params->kernel_addr)
        : "x0", "x1", "x2", "x3"
    );

    /* 不应该到达这里 */
    while (1);
}
