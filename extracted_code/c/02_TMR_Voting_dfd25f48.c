/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\02_TMR_Voting.md
 * Line: 263
 * Language: c
 * Block ID: dfd25f48
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// TMR处理器状态管理
// ============================================================================

#define TMR_STATE_SIZE  256     // 状态字数

// TMR处理器上下文
typedef struct {
    uint32_t state_a[TMR_STATE_SIZE];
    uint32_t state_b[TMR_STATE_SIZE];
    uint32_t state_c[TMR_STATE_SIZE];
    uint32_t voted_state[TMR_STATE_SIZE];

    TMRChannel32 pc;            // 程序计数器TMR
    TMRChannel32 flags;         // 标志寄存器TMR

    ModuleStatus module_status[3];
    uint32_t sync_counter;
    bool synchronized;
} TMRProcessor;

// 同步检查
bool tmr_check_sync(const TMRProcessor *proc) {
    // 检查PC是否一致
    bool pc_match = (proc->pc.value_a == proc->pc.value_b) &&
                    (proc->pc.value_b == proc->pc.value_c);

    // 检查关键状态是否一致
    bool state_match = true;
    for (int i = 0; i < 16; i++) {  // 检查前16个关键寄存器
        if (proc->state_a[i] != proc->state_b[i] ||
            proc->state_b[i] != proc->state_c[i]) {
            state_match = false;
            break;
        }
    }

    return pc_match && state_match;
}

// 表决整个状态
void tmr_vote_state(TMRProcessor *proc) {
    for (int i = 0; i < TMR_STATE_SIZE; i++) {
        TMRChannel32 channel;
        tmr_channel_init(&channel);

        channel.value_a = proc->state_a[i];
        channel.value_b = proc->state_b[i];
        channel.value_c = proc->state_c[i];

        majority_vote_32(&channel);
        proc->voted_state[i] = channel.voted_value;

        // 纠正偏离的模块
        if (channel.status_a == MODULE_FAILED) {
            proc->state_a[i] = channel.voted_value;
        }
        if (channel.status_b == MODULE_FAILED) {
            proc->state_b[i] = channel.voted_value;
        }
        if (channel.status_c == MODULE_FAILED) {
            proc->state_c[i] = channel.voted_value;
        }
    }
}

// 状态同步 (恢复偏离的模块)
void tmr_resynchronize(TMRProcessor *proc) {
    // 选择参考模块 (最健康的)
    int ref = 0;
    if (proc->module_status[0] != MODULE_HEALTHY) {
        ref = (proc->module_status[1] == MODULE_HEALTHY) ? 1 : 2;
    }

    // 将其他模块同步到参考模块
    for (int i = 0; i < 3; i++) {
        if (i != ref && proc->module_status[i] != MODULE_FAILED) {
            memcpy(proc->state_a + i * TMR_STATE_SIZE,
                   proc->state_a + ref * TMR_STATE_SIZE,
                   TMR_STATE_SIZE * sizeof(uint32_t));
            proc->module_status[i] = MODULE_HEALTHY;
        }
    }

    proc->synchronized = true;
}
