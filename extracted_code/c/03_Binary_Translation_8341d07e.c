/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\03_Binary_Translation.md
 * Line: 275
 * Language: c
 * Block ID: 8341d07e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ARM64后端代码生成示例
void tcg_out_add(TCGContext* ctx, TCGReg rd, TCGReg rn, TCGReg rm) {
    // 生成 ARM64 ADD 指令: ADD rd, rn, rm
    uint32_t insn = 0x0b000000;
    insn |= (rm & 0x1f) << 16;
    insn |= (rn & 0x1f) << 5;
    insn |= (rd & 0x1f);
    tcg_out32(ctx, insn);
}

void tcg_out_load(TCGContext* ctx, TCGReg rd, TCGReg rn, int offset) {
    // 生成 ARM64 LDR 指令
    if (is_valid_immediate_offset(offset)) {
        uint32_t insn = 0xb9400000;  // LDR (32-bit)
        insn |= ((offset >> 2) & 0xfff) << 10;
        insn |= (rn & 0x1f) << 5;
        insn |= (rd & 0x1f);
        tcg_out32(ctx, insn);
    } else {
        // 使用临时寄存器加载大偏移
        tcg_out_movi(ctx, TCG_TYPE_I64, TCG_REG_X16, offset);
        tcg_out_add(ctx, TCG_REG_X16, rn, TCG_REG_X16);
        tcg_out_load(ctx, rd, TCG_REG_X16, 0);
    }
}
