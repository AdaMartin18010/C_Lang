/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\04_Memory_Protection_Unit.md
 * Line: 617
 * Language: c
 * Block ID: e5ca8bdf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 安全关键系统中的MPU应用
 * 符合IEC 61508 / ISO 26262 要求
 */

/*
 * 安全分区架构
 */

// 定义安全分区
typedef enum {
    PARTITION_ASIL_D,   // 最高安全等级 (制动、转向)
    PARTITION_ASIL_B,   // 中等安全等级 (仪表)
    PARTITION_QM,       // 质量管理 (信息娱乐)
    PARTITION_NUM
} safety_partition_t;

/*
 * 每个分区的MPU配置
 */
typedef struct {
    uint32_t code_base;
    uint32_t code_size;
    uint32_t data_base;
    uint32_t data_size;
    uint32_t stack_base;
    uint32_t stack_size;
    uint32_t permissions;
} partition_mpu_config_t;

static const partition_mpu_config_t partition_config[PARTITION_NUM] = {
    [PARTITION_ASIL_D] = {
        .code_base = 0x08000000,
        .code_size = 256 * 1024,   // 256KB
        .data_base = 0x20000000,
        .data_size = 128 * 1024,   // 128KB
        .stack_base = 0x20020000,
        .stack_size = 32 * 1024,   // 32KB
        .permissions = MPU_PRIV_RO_USER_NO   // 严格隔离
    },
    [PARTITION_ASIL_B] = {
        .code_base = 0x08040000,
        .code_size = 128 * 1024,
        .data_base = 0x20040000,
        .data_size = 64 * 1024,
        .stack_base = 0x20050000,
        .stack_size = 16 * 1024,
        .permissions = MPU_PRIV_RW_USER_RO
    },
    [PARTITION_QM] = {
        .code_base = 0x08060000,
        .code_size = 512 * 1024,
        .data_base = 0x20060000,
        .data_size = 256 * 1024,
        .stack_base = 0x20090000,
        .stack_size = 64 * 1024,
        .permissions = MPU_PRIV_RW_USER_RW
    }
};

/*
 * 上下文切换时更新MPU
 * 不同安全等级的任务使用不同的MPU配置
 */
void mpu_switch_partition(safety_partition_t new_partition) {
    const partition_mpu_config_t *cfg = &partition_config[new_partition];

    // 禁用MPU进行重新配置
    MPU->CTRL = 0;

    // 配置代码区域
    MPU->RNR = 0;
    MPU->RBAR = cfg->code_base | (1 << 4);  // 区域有效
    MPU->RASR = ((31 - __CLZ(cfg->code_size)) << 1) |  // 大小
                (AP_RO << 24) |                          // 只读
                (1 << 28);                                // XN=0 (可执行)

    // 配置数据区域
    MPU->RNR = 1;
    MPU->RBAR = cfg->data_base | (1 << 4);
    MPU->RASR = ((31 - __CLZ(cfg->data_size)) << 1) |
                (cfg->permissions << 24) |
                (1 << 28);  // XN=1 (不可执行)

    // 配置栈区域
    MPU->RNR = 2;
    MPU->RBAR = cfg->stack_base | (1 << 4);
    MPU->RASR = ((31 - __CLZ(cfg->stack_size)) << 1) |
                (AP_RW << 24) |
                (1 << 28);

    // 启用MPU
    MPU->CTRL = MPU_CTRL_ENABLE | MPU_CTRL_PRIVDEFENA;

    __DSB();
    __ISB();
}

/*
 * 安全监控: 检测未授权访问尝试
 */
void safety_monitor_hook(void) {
    uint32_t cfsr = SCB->CFSR;

    if (cfsr & SCB_CFSR_MMARVALID_Msk) {
        uint32_t fault_addr = SCB->MMFAR;
        uint32_t task_id = get_current_task_id();

        // 记录安全事件
        log_safety_event(SEV_MEM_FAULT, task_id, fault_addr);

        // 根据安全策略处理
        switch (get_current_partition()) {
            case PARTITION_ASIL_D:
                // 最高安全分区出错，进入安全状态
                enter_safe_state();
                break;
            case PARTITION_ASIL_B:
                // 尝试恢复或重启分区
                restart_partition(PARTITION_ASIL_B);
                break;
            case PARTITION_QM:
                // 非安全分区出错，仅记录
                log_event("QM partition fault");
                break;
        }
    }
}
