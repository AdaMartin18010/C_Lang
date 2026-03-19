/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\02_CFI_Stack_Reconstruction.md
 * Line: 818
 * Language: c
 * Block ID: 494e54c1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：在恢复寄存器前修改CFA相关寄存器
void wrong_unwind_order(UnwindContext *ctx) {
    // 如果CFA基于RSP定义，但先恢复了RSP...
    restore_register(&ctx->rules[RSP], ctx->cfa, ctx, &ctx->registers[RSP]);
    // 现在CFA计算会出错！
    compute_cfa(&ctx->cfa, ctx, &ctx->cfa);
}

// 正确：先计算CFA，再恢复寄存器
void correct_unwind_order(UnwindContext *ctx) {
    uint64_t saved_cfa;
    compute_cfa(&ctx->cfa, ctx, &saved_cfa);

    // 使用保存的CFA恢复所有寄存器
    for (int i = 0; i < REG_COUNT; i++) {
        restore_register(&ctx->rules[i], saved_cfa, ctx, &ctx->registers[i]);
    }
}
