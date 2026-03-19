/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\02_Register_VM.md
 * Line: 357
 * Language: c
 * Block ID: cdc7ff26
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 基于使用计数的简单寄存器分配
 * 优先将高频使用的变量分配到寄存器
 */

typedef struct {
    uint16_t var_idx;     /* 变量索引 */
    uint16_t use_count;   /* 使用次数 */
    int last_use;         /* 最后使用位置 */
    int assigned_reg;     /* 分配的寄存器，-1表示栈 */
} VarInfo;

/* 线性扫描寄存器分配 */
void allocate_registers(RVProto *proto, int num_regs) {
    VarInfo vars[proto->num_locals];
    bool reg_free[num_regs];

    /* 初始化 */
    for (int i = 0; i < num_regs; i++) reg_free[i] = true;

    /* 按使用频率排序 */
    qsort(vars, proto->num_locals, sizeof(VarInfo), compare_use_count);

    /* 分配寄存器 */
    for (int i = 0; i < proto->num_locals; i++) {
        VarInfo *v = &vars[i];

        /* 查找空闲寄存器 */
        int reg = -1;
        for (int r = 0; r < num_regs; r++) {
            if (reg_free[r]) {
                reg = r;
                break;
            }
        }

        if (reg >= 0) {
            v->assigned_reg = reg;
            reg_free[reg] = false;

            /* 标记变量生命周期结束后的释放 */
            schedule_reg_release(v->last_use, reg);
        } else {
            /* 溢出到栈 */
            v->assigned_reg = -1;
        }
    }
}
