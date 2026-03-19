/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\02_CFI_Stack_Reconstruction.md
 * Line: 476
 * Language: c
 * Block ID: 3367212d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 从FDE构建完整展开表
GArray *build_unwind_table(const FDE *fde) {
    GArray *table = g_array_new(FALSE, FALSE, sizeof(UnwindRow));

    // 初始状态来自CIE
    UnwindContext ctx = {
        .pc = fde->initial_location,
        .cfa = fde->cie->initial_cfa,
    };
    memcpy(ctx.rules, fde->cie->initial_rules, sizeof(ctx.rules));

    // 记录初始行
    UnwindRow initial_row = {
        .pc = ctx.pc,
        .cfa = ctx.cfa,
    };
    memcpy(initial_row.rules, ctx.rules, sizeof(initial_row.rules));
    g_array_append_val(table, initial_row);

    // 执行CIE初始指令
    const uint8_t *cie_p = fde->cie->initial_instructions;
    const uint8_t *cie_end = cie_p + fde->cie->initial_instructions_length;

    while (cie_p < cie_end) {
        uint8_t opcode = *cie_p++;
        execute_cfi_instruction(opcode, &cie_p, fde->cie, &ctx, table);

        // 记录状态变更
        UnwindRow row = {
            .pc = ctx.pc,
            .cfa = ctx.cfa,
        };
        memcpy(row.rules, ctx.rules, sizeof(row.rules));
        g_array_append_val(table, row);
    }

    // 执行FDE指令
    const uint8_t *fde_p = fde->instructions;
    const uint8_t *fde_end = fde_p + fde->instructions_length;

    while (fde_p < fde_end) {
        uint8_t opcode = *fde_p++;
        execute_cfi_instruction(opcode, &fde_p, fde->cie, &ctx, table);

        // 记录状态变更
        UnwindRow row = {
            .pc = ctx.pc,
            .cfa = ctx.cfa,
        };
        memcpy(row.rules, ctx.rules, sizeof(row.rules));
        g_array_append_val(table, row);
    }

    return table;
}
