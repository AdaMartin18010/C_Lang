/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\02_CFI_Stack_Reconstruction.md
 * Line: 640
 * Language: c
 * Block ID: 5d379808
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 栈帧表示
typedef struct {
    uint64_t pc;                    // 程序计数器
    uint64_t sp;                    // 栈指针
    uint64_t fp;                    // 帧指针（如果有）
    uint64_t cfa;                   // 规范帧地址
    uint64_t regs[REG_COUNT];       // 寄存器快照
} StackFrame;

// 执行单步栈展开
int unwind_frame(const StackFrame *current,
                 const GArray *fde_table,
                 StackFrame *next) {
    // 1. 找到当前PC对应的FDE
    FDE *fde = find_fde_for_pc(fde_table, current->pc);
    if (!fde) {
        fprintf(stderr, "No FDE found for PC 0x%lx\n", current->pc);
        return -1;
    }

    // 2. 构建或查找展开表
    GArray *unwind_table = build_unwind_table(fde);

    // 3. 找到当前PC对应的行
    UnwindRow *row = find_unwind_row(unwind_table, current->pc);
    if (!row) {
        fprintf(stderr, "No unwind row for PC 0x%lx\n", current->pc);
        return -1;
    }

    // 4. 计算CFA
    uint64_t cfa;
    if (compute_cfa(&row->cfa,
                    &(UnwindContext){ .registers = current->regs },
                    &cfa) != 0) {
        return -1;
    }

    // 5. 恢复所有寄存器
    memcpy(next->regs, current->regs, sizeof(next->regs));

    for (int i = 0; i < REG_COUNT; i++) {
        if (row->rules[i].type != RULE_UNDEFINED) {
            uint64_t val;
            if (restore_register(&row->rules[i], cfa,
                                 &(UnwindContext){ .registers = current->regs },
                                 &val) == 0) {
                next->regs[i] = val;
            }
        }
    }

    // 6. 设置下一帧的PC为返回地址
    // 返回地址通常在CFA - 8（x86-64）
    next->pc = next->regs[fde->cie->return_address_register];

    // 7. 更新SP为CFA
    next->sp = cfa;
    next->cfa = cfa;

    return 0;
}

// 完整栈回溯
int backtrace_stack(uint64_t *current_regs,
                    const GArray *fde_table,
                    StackFrame *frames,
                    int max_frames,
                    int *frame_count) {
    StackFrame current = {0};
    memcpy(current.regs, current_regs, sizeof(current.regs));
    current.pc = current_regs[RIP];  // x86-64指令指针
    current.sp = current_regs[RSP];  // x86-64栈指针
    current.fp = current_regs[RBP];  // x86-64帧指针

    *frame_count = 0;
    frames[(*frame_count)++] = current;

    while (*frame_count < max_frames) {
        StackFrame next = {0};

        if (unwind_frame(&current, fde_table, &next) != 0) {
            break;
        }

        // 检查是否到达栈底
        if (next.pc == 0) {
            break;
        }

        frames[(*frame_count)++] = next;
        current = next;
    }

    return 0;
}
