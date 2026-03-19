/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\README.md
 * Line: 504
 * Language: c
 * Block ID: a9d82151
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 位置列表条目
struct LocationListEntry {
    uint64_t start_addr;   // 起始地址（或 base address selection）
    uint64_t end_addr;     // 结束地址
    uint8_t *expression;   // DWARF 表达式
    size_t expr_len;
};

// 解析变量位置
int evaluate_location_expression(const uint8_t *expr, size_t len,
                                  Dwarf_RegState *regs,
                                  uint64_t *result) {
    uint64_t stack[64];
    int sp = 0;
    const uint8_t *p = expr;

    while (p < expr + len) {
        uint8_t op = *p++;

        switch (op) {
            case DW_OP_reg0 ... DW_OP_reg31:
                stack[sp++] = regs->r[op - DW_OP_reg0];
                break;

            case DW_OP_breg0 ... DW_OP_breg31: {
                int64_t offset = decode_sleb128(&p);
                stack[sp++] = regs->r[op - DW_OP_breg0] + offset;
                break;
            }

            case DW_OP_addr:
                stack[sp++] = read_target_addr(&p);
                break;

            case DW_OP_constu:
                stack[sp++] = decode_uleb128(&p);
                break;

            case DW_OP_consts:
                stack[sp++] = decode_sleb128(&p);
                break;

            case DW_OP_plus:
                sp--;
                stack[sp-1] += stack[sp];
                break;

            case DW_OP_minus:
                sp--;
                stack[sp-1] -= stack[sp];
                break;

            case DW_OP_deref:
                // 从计算出的地址读取值
                // 需要目标进程的内存访问
                stack[sp-1] = read_memory(stack[sp-1]);
                break;

            case DW_OP_fbreg: {
                int64_t offset = decode_sleb128(&p);
                // 帧基址寄存器 + 偏移
                stack[sp++] = regs->frame_base + offset;
                break;
            }

            case DW_OP_stack_value:
                // 值本身就在栈上，不在内存中
                // 常用于寄存器变量
                break;
        }
    }

    *result = stack[0];
    return 0;
}
