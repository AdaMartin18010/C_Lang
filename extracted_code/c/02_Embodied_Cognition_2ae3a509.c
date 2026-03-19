/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\02_Embodied_Cognition.md
 * Line: 497
 * Language: c
 * Block ID: 2ae3a509
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 完整的调试支持基础设施

// 断点管理
typedef struct {
    void *address;
    uint8_t saved_byte;  // 保存的原始指令
    bool enabled;
} Breakpoint;

// 设置断点（INT 3指令，0xCC）
bool set_breakpoint(Breakpoint *bp) {
    uint8_t int3 = 0xCC;
    // 保存原字节
    bp->saved_byte = *(uint8_t *)bp->address;
    // 写入INT 3
    memcpy(bp->address, &int3, 1);
    bp->enabled = true;
    return true;
}

// 移除断点
bool remove_breakpoint(Breakpoint *bp) {
    if (!bp->enabled) return false;
    // 恢复原字节
    memcpy(bp->address, &bp->saved_byte, 1);
    bp->enabled = false;
    return true;
}

// 单步执行实现（x86 EFLAGS.TF）
void enable_single_step(void) {
    __asm__ volatile(
        "pushf\n\t"
        "orl $0x100, (%rsp)\n\t"  // 设置陷阱标志
        "popf\n\t"
    );
}
