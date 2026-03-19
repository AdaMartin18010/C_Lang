/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\02_CFI_Stack_Reconstruction.md
 * Line: 841
 * Language: c
 * Block ID: 4d5b2d78
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：忽略code_alignment_factor
void wrong_advance_loc(const uint8_t *instr, CIE *cie) {
    uint8_t delta = *instr;
    ctx.pc += delta;  // ❌ 需要乘以code_align！
}

// 正确：应用对齐因子
void correct_advance_loc(const uint8_t *instr, CIE *cie) {
    uint8_t delta = *instr;
    ctx.pc += delta * cie->code_alignment_factor;
}

// 类似地，data_alignment_factor也需要用于offset
void correct_offset(uint64_t encoded_offset, CIE *cie) {
    int64_t actual_offset = encoded_offset * cie->data_alignment_factor;
    // data_align通常是负数（向低地址增长）
}
