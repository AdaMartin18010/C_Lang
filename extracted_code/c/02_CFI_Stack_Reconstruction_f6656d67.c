/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\02_CFI_Stack_Reconstruction.md
 * Line: 538
 * Language: c
 * Block ID: f6656d67
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 计算CFA（Canonical Frame Address）
int compute_cfa(const CFADefinition *cfa_def,
                const UnwindContext *ctx,
                uint64_t *cfa_out) {
    switch (cfa_def->type) {
        case CFA_REGISTER: {
            uint64_t reg_val = ctx->registers[cfa_def->reg.reg_num];
            *cfa_out = reg_val + cfa_def->reg.offset;
            return 0;
        }

        case CFA_EXPRESSION: {
            // 执行DWARF表达式计算CFA
            uint64_t result;
            int ret = evaluate_dwarf_expression(
                cfa_def->expr.expr,
                cfa_def->expr.len,
                ctx->registers,
                0,  // CFA未定义（正在计算）
                &result
            );
            if (ret == 0) {
                *cfa_out = result;
            }
            return ret;
        }

        default:
            return -1;
    }
}
