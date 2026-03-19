/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\README.md
 * Line: 345
 * Language: c
 * Block ID: 9a3d45a6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简化的行号程序解析器
int parse_line_number_program(const uint8_t *program, size_t len,
                              LineNumberState *initial_state,
                              void (*callback)(const LineNumberState *)) {
    LineNumberState state = *initial_state;
    const uint8_t *p = program;

    while (p < program + len) {
        uint8_t opcode = *p++;

        if (opcode == 0) {
            // 扩展操作码
            uint64_t length = decode_uleb128(&p);
            const uint8_t *end = p + length;
            uint8_t ext_opcode = *p++;

            switch (ext_opcode) {
                case DW_LNE_end_sequence:
                    state.end_sequence = 1;
                    callback(&state);
                    // 重置状态
                    state = *initial_state;
                    break;
                case DW_LNE_set_address:
                    state.address = read_target_addr(&p);
                    break;
                case DW_LNE_define_file:
                    // 添加文件到文件表
                    break;
            }
            p = end;
        }
        else if (opcode < opcode_base) {
            // 标准操作码
            switch (opcode) {
                case DW_LNS_copy:
                    callback(&state);
                    state.basic_block = 0;
                    state.prologue_end = 0;
                    state.epilogue_begin = 0;
                    state.discriminator = 0;
                    break;
                case DW_LNS_advance_pc:
                    state.address += decode_uleb128(&p) * minimum_instruction_length;
                    break;
                case DW_LNS_advance_line:
                    state.line += decode_sleb128(&p);
                    break;
                case DW_LNS_set_file:
                    state.file = decode_uleb128(&p);
                    break;
                case DW_LNS_set_column:
                    state.column = decode_uleb128(&p);
                    break;
                case DW_LNS_negate_stmt:
                    state.is_stmt = !state.is_stmt;
                    break;
                case DW_LNS_const_add_pc:
                    state.address += ((255 - opcode_base) / line_range)
                                     * minimum_instruction_length;
                    break;
            }
        }
        else {
            // 特殊操作码
            int adjusted = opcode - opcode_base;
            int line_adv = line_base + (adjusted % line_range);
            int addr_adv = adjusted / line_range;

            state.line += line_adv;
            state.address += addr_adv * minimum_instruction_length;
            callback(&state);

            state.basic_block = 0;
            state.prologue_end = 0;
            state.epilogue_begin = 0;
            state.discriminator = 0;
        }
    }
    return 0;
}
