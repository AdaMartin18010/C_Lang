/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 457
 * Language: c
 * Block ID: 79800659
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 硬件信任根边界保护机制
 */

/* 保护机制类型 */
typedef enum {
    PROTECTION_PHYSICAL = 0,    /* 物理保护 */
    PROTECTION_LOGICAL,         /* 逻辑保护 */
    PROTECTION_CRYPTOGRAPHIC,   /* 密码学保护 */
    PROTECTION_TEMPORAL,        /* 时序保护 */
} protection_type_t;

/* 物理保护 */
typedef struct {
    bool tamper_detection;      /* 篡改检测 */
    bool tamper_response;       /* 篡改响应 */
    bool side_channel_shielding; /* 侧信道屏蔽 */
    bool active_shield;         /* 主动屏蔽 */
    uint32_t tamper_response_delay_us; /* 响应延迟 */
} physical_protection_t;

/* 逻辑保护 */
typedef struct {
    bool mpu_enabled;           /* 内存保护单元 */
    bool trustzone_enabled;     /* TrustZone */
    bool pmp_enabled;           /* 物理内存保护(RISC-V) */
    uint32_t secure_region_start;
    uint32_t secure_region_size;
} logical_protection_t;

/* 密码学保护 */
typedef struct {
    bool secure_boot_verification;  /* 安全启动验证 */
    bool measurement_boot;          /* 测量启动 */
    bool authenticated_encryption;  /* 认证加密 */
    uint32_t key_hierarchy_levels;  /* 密钥层次 */
} crypto_protection_t;

/* 配置边界保护 */
int configure_boundary_protection(physical_protection_t *phys,
                                   logical_protection_t *logic,
                                   crypto_protection_t *crypto) {
    /* 1. 配置物理保护 */
    if (phys->tamper_detection) {
        /* 启用篡改检测 */
        enable_tamper_sensors();

        /* 配置篡改响应 */
        if (phys->tamper_response) {
            configure_tamper_response(phys->tamper_response_delay_us);
        }
    }

    /* 启用侧信道防护 */
    if (phys->side_channel_shielding) {
        enable_power_analysis_countermeasures();
        enable_em_shielding();
    }

    /* 2. 配置逻辑保护 */
    if (logic->trustzone_enabled) {
        /* 配置TrustZone */
        configure_trustzone(logic->secure_region_start,
                           logic->secure_region_size);
    }

    if (logic->mpu_enabled) {
        /* 配置MPU */
        setup_mpu_regions();
    }

    if (logic->pmp_enabled) {
        /* 配置RISC-V PMP */
        setup_pmp_regions();
    }

    /* 3. 配置密码学保护 */
    if (crypto->secure_boot_verification) {
        /* 加载并验证固件 */
        load_verified_firmware();
    }

    if (crypto->measurement_boot) {
        /* 测量系统组件 */
        measure_system_components();
    }

    return 0;
}

/* 篡改检测处理 */
void tamper_detection_handler(void) {
    /* 1. 识别篡改源 */
    uint32_t tamper_source = read_tamper_source();

    /* 2. 记录篡改事件 */
    log_tamper_event(tamper_source);

    /* 3. 执行响应 */
    switch (tamper_source) {
        case TAMPER_VOLTAGE_GLITCH:
            /* 重置系统 */
            system_reset();
            break;

        case TAMPER_TEMPERATURE:
            /* 进入安全模式 */
            enter_secure_mode();
            break;

        case TAMPER_PHYSICAL_BREACH:
            /* 清除敏感数据 */
            secure_erase_all_keys();
            system_reset();
            break;

        case TAMPER_CLOCK_GLITCH:
            /* 切换到内部时钟 */
            switch_to_internal_clock();
            break;
    }
}
