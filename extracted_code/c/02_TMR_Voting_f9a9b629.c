/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\02_TMR_Voting.md
 * Line: 354
 * Language: c
 * Block ID: f9a9b629
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// TMR故障恢复机制
// ============================================================================

// 故障日志
typedef struct {
    uint32_t timestamp;
    uint8_t failed_module;
    uint8_t error_type;
    uint32_t error_address;
} TMRFaultLog;

#define MAX_FAULT_LOG   16

typedef struct {
    TMRFaultLog logs[MAX_FAULT_LOG];
    uint8_t log_count;
    uint32_t total_faults;
} TMRDiagnostics;

// 记录故障
void tmr_log_fault(TMRDiagnostics *diag, uint8_t module,
                   uint8_t error_type, uint32_t address) {
    if (diag->log_count < MAX_FAULT_LOG) {
        TMRFaultLog *log = &diag->logs[diag->log_count++];
        log->timestamp = get_system_tick();
        log->failed_module = module;
        log->error_type = error_type;
        log->error_address = address;
    }
    diag->total_faults++;
}

// 尝试恢复故障模块
bool tmr_attempt_recovery(TMRProcessor *proc, uint8_t module) {
    if (module > 2) return false;

    // 1. 复位故障模块
    module_reset(module);

    // 2. 从健康模块复制状态
    int healthy = -1;
    for (int i = 0; i < 3; i++) {
        if (proc->module_status[i] == MODULE_HEALTHY) {
            healthy = i;
            break;
        }
    }

    if (healthy < 0) return false;  // 没有健康模块可恢复

    // 复制状态
    memcpy(proc->state_a + module * TMR_STATE_SIZE,
           proc->state_a + healthy * TMR_STATE_SIZE,
           TMR_STATE_SIZE * sizeof(uint32_t));

    // 3. 重新同步
    proc->module_status[module] = MODULE_HEALTHY;
    tmr_resynchronize(proc);

    return true;
}

// 降级到双模运行
void tmr_degrade_to_dual(TMRProcessor *proc) {
    // 找到两个健康模块
    int healthy[2];
    int h = 0;

    for (int i = 0; i < 3; i++) {
        if (proc->module_status[i] != MODULE_FAILED && h < 2) {
            healthy[h++] = i;
        }
    }

    if (h < 2) {
        // 只剩一个模块，进入安全模式
        enter_safe_mode();
        return;
    }

    // 使用双模比较
    // 不再能纠正错误，只能检测
    enable_dual_comparison(healthy[0], healthy[1]);
}
