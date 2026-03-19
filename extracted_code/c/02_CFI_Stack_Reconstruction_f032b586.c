/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\02_CFI_Stack_Reconstruction.md
 * Line: 574
 * Language: c
 * Block ID: f032b586
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 根据规则恢复单个寄存器值
int restore_register(const RegisterRule *rule,
                     uint64_t cfa,
                     const UnwindContext *ctx,
                     uint64_t *value_out) {
    switch (rule->type) {
        case RULE_UNDEFINED:
            // 值未定义
            return -1;

        case RULE_SAME_VALUE:
            // 值与调用时相同（已在ctx中）
            *value_out = ctx->registers[/*当前寄存器*/];
            return 0;

        case RULE_OFFSET:
            // 值保存在 CFA + offset
            *value_out = *(uint64_t *)(cfa + rule->offset);
            return 0;

        case RULE_VAL_OFFSET:
            // 值是 CFA + offset
            *value_out = cfa + rule->offset;
            return 0;

        case RULE_REGISTER:
            // 从另一个寄存器获取
            *value_out = ctx->registers[rule->reg_num];
            return 0;

        case RULE_EXPRESSION: {
            // 执行DWARF表达式得到地址，然后解引用
            uint64_t addr;
            int ret = evaluate_dwarf_expression(
                rule->expression.expr,
                rule->expression.len,
                ctx->registers,
                cfa,
                &addr
            );
            if (ret == 0) {
                *value_out = *(uint64_t *)addr;
            }
            return ret;
        }

        case RULE_VAL_EXPRESSION: {
            // 执行DWARF表达式直接得到值
            return evaluate_dwarf_expression(
                rule->expression.expr,
                rule->expression.len,
                ctx->registers,
                cfa,
                value_out
            );
        }

        default:
            return -1;
    }
}
