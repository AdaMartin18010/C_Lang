/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\01_ARM_Trusted_Firmware.md
 * Line: 112
 * Language: c
 * Block ID: 98b143ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ARM TF-A启动阶段

// BL1 - Boot ROM (EL3)
// • 芯片出厂固化在ROM中
// • 加载并验证BL2
// • 只读，不可更新

void bl1_entry(void) {
    // 1. 初始化基础硬件
    uart_init();
    console_init();

    // 2. 加载BL2到SRAM
    load_bl2_from_flash(BL2_BASE);

    // 3. 验证BL2签名
    if (!verify_image(BL2_BASE, BL2_SIZE, bl2_key)) {
        panic("BL2 verification failed");
    }

    // 4. 跳转到BL2
    jump_to_bl2(BL2_BASE);
}

// BL2 - Trusted Boot Firmware (EL3/S-EL1)
// • 负责加载后续启动阶段
// • 初始化DDR
// • 建立信任链

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint64_t image_id;
    uint64_t image_base;
    uint64_t image_size;
    uint8_t  signature[256];  // RSA-2048签名
    uint8_t  hash[32];        // SHA-256哈希
} ImageHeader;

void bl2_entry(void) {
    // 1. 初始化DDR
    ddr_init();

    // 2. 加载并验证BL31 (EL3 Runtime)
    load_image(BL31_BASE, "bl31.bin");
    verify_image(BL31_BASE, bl31_key);

    // 3. 加载并验证BL32 (Secure OS，可选)
    #ifdef LOAD_BL32
    load_image(BL32_BASE, "bl32.bin");
    verify_image(BL32_BASE, bl32_key);
    #endif

    // 4. 加载并验证BL33 (Normal World OS)
    load_image(BL33_BASE, "bl33.bin");
    verify_image(BL33_BASE, bl33_key);

    // 5. 传递控制权给BL31
    jump_to_bl31(BL31_BASE);
}

// BL31 - EL3 Runtime Firmware
// • 常驻EL3
// • 处理SMC调用
// • 电源管理 (PSCI)

void bl31_entry(void) {
    // 1. 初始化中断控制器
    gic_init();

    // 2. 初始化PSCI
    psci_init();

    // 3. 初始化Secure Monitor
    sm_init();

    // 4. 如果有BL32，启动它
    #ifdef LOAD_BL32
    start_bl32(BL32_BASE);
    #endif

    // 5. 切换到Non-secure世界，启动BL33
    switch_to_non_secure(BL33_BASE);
}
