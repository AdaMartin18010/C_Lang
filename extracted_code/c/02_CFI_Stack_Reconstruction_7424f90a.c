/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\02_CFI_Stack_Reconstruction.md
 * Line: 863
 * Language: c
 * Block ID: 7424f90a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 信号处理程序栈帧需要特殊处理
bool is_signal_frame(FDE *fde) {
    // 检查CIE扩展（GNU特殊扩展）
    return strstr(fde->cie->augmentation, "S") != NULL;
}

int unwind_signal_frame(const StackFrame *current, StackFrame *next) {
    if (is_signal_frame(current_fde)) {
        // 信号帧保存了完整的sigcontext
        // 需要从ucontext中恢复所有寄存器
        struct ucontext *uc = (struct ucontext *)current->cfa;
        next->regs[RIP] = uc->uc_mcontext.gregs[REG_RIP];
        next->regs[RSP] = uc->uc_mcontext.gregs[REG_RSP];
        // ... 恢复其他寄存器
        return 0;
    }
    return unwind_normal_frame(current, next);
}
