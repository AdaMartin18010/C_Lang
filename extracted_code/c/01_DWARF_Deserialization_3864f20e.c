/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\01_DWARF_Deserialization.md
 * Line: 556
 * Language: c
 * Block ID: 3864f20e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 执行行号程序
LineNumberEntry *execute_line_program(
    const uint8_t *program,
    size_t program_length,
    const LineNumberProgramHeader *header,
    int *num_entries
) {
    LineNumberState state = {
        .address = 0,
        .file = 1,
        .line = 1,
        .column = 0,
        .is_stmt = header->default_is_stmt,
        .basic_block = false,
        .end_sequence = false,
        .prologue_end = false,
        .epilogue_begin = false,
        .isa = 0,
        .discriminator = 0,
    };

    GArray *matrix = g_array_new(FALSE, FALSE, sizeof(LineNumberEntry));
    size_t pc = 0;

    while (pc < program_length) {
        uint8_t opcode = program[pc++];

        if (opcode == 0) {
            // 扩展操作码
            size_t len = decode_uleb128(program + pc, &pc);
            uint8_t ext_opcode = program[pc++];

            switch (ext_opcode) {
                case DW_LNE_end_sequence:
                    state.end_sequence = true;
                    g_array_append_val(matrix,
                                       ((LineNumberEntry){ state.address,
                                                           state.file,
                                                           state.line,
                                                           state.column,
                                                           state.is_stmt,
                                                           state.basic_block,
                                                           state.end_sequence,
                                                           state.prologue_end,
                                                           state.epilogue_begin,
                                                           state.isa,
                                                           state.discriminator }));
                    // 重置状态机
                    memset(&state, 0, sizeof(state));
                    state.file = 1;
                    state.line = 1;
                    state.is_stmt = header->default_is_stmt;
                    break;

                case DW_LNE_set_address:
                    state.address = read_uint64(program + pc);
                    pc += 8;
                    break;

                case DW_LNE_define_file: {
                    // 解析文件名定义
                    char *name = (char *)(program + pc);
                    pc += strlen(name) + 1;
                    size_t dir_idx = decode_uleb128(program + pc, &pc);
                    size_t mtime = decode_uleb128(program + pc, &pc);
                    size_t flen = decode_uleb128(program + pc, &pc);
                    // 添加到文件列表...
                    break;
                }

                case DW_LNE_set_discriminator:
                    state.discriminator = decode_uleb128(program + pc, &pc);
                    break;
            }
        } else if (opcode < header->opcode_base) {
            // 标准操作码
            switch (opcode) {
                case DW_LNS_copy:
                    g_array_append_val(matrix,
                                       ((LineNumberEntry){ state.address,
                                                           state.file,
                                                           state.line,
                                                           state.column,
                                                           state.is_stmt,
                                                           state.basic_block,
                                                           state.end_sequence,
                                                           state.prologue_end,
                                                           state.epilogue_begin,
                                                           state.isa,
                                                           state.discriminator }));
                    state.basic_block = false;
                    state.prologue_end = false;
                    state.epilogue_begin = false;
                    state.discriminator = 0;
                    break;

                case DW_LNS_advance_pc: {
                    size_t bytes;
                    uint64_t delta = decode_uleb128(program + pc, &bytes);
                    pc += bytes;
                    state.address += delta * header->minimum_instruction_length;
                    break;
                }

                case DW_LNS_advance_line: {
                    size_t bytes;
                    int64_t delta = decode_sleb128(program + pc, &bytes);
                    pc += bytes;
                    state.line += delta;
                    break;
                }

                case DW_LNS_set_file: {
                    size_t bytes;
                    state.file = decode_uleb128(program + pc, &bytes);
                    pc += bytes;
                    break;
                }

                case DW_LNS_set_column: {
                    size_t bytes;
                    state.column = decode_uleb128(program + pc, &bytes);
                    pc += bytes;
                    break;
                }

                case DW_LNS_negate_stmt:
                    state.is_stmt = !state.is_stmt;
                    break;

                case DW_LNS_set_basic_block:
                    state.basic_block = true;
                    break;

                case DW_LNS_const_add_pc:
                    state.address += ((255 - header->opcode_base) /
                                     header->line_range) *
                                     header->minimum_instruction_length;
                    break;

                case DW_LNS_fixed_advance_pc:
                    state.address += read_uint16(program + pc);
                    pc += 2;
                    break;

                case DW_LNS_set_prologue_end:
                    state.prologue_end = true;
                    break;

                case DW_LNS_set_epilogue_begin:
                    state.epilogue_begin = true;
                    break;

                case DW_LNS_set_isa: {
                    size_t bytes;
                    state.isa = decode_uleb128(program + pc, &bytes);
                    pc += bytes;
                    break;
                }
            }
        } else {
            // 特殊操作码
            uint8_t adjusted_opcode = opcode - header->opcode_base;
            int8_t line_inc = header->line_base +
                             (adjusted_opcode % header->line_range);
            uint64_t addr_inc = (adjusted_opcode / header->line_range) *
                                header->minimum_instruction_length;

            state.line += line_inc;
            state.address += addr_inc;

            g_array_append_val(matrix,
                               ((LineNumberEntry){ state.address,
                                                   state.file,
                                                   state.line,
                                                   state.column,
                                                   state.is_stmt,
                                                   true,  // basic_block
                                                   state.end_sequence,
                                                   state.prologue_end,
                                                   state.epilogue_begin,
                                                   state.isa,
                                                   state.discriminator }));

            state.basic_block = false;
            state.prologue_end = false;
            state.epilogue_begin = false;
            state.discriminator = 0;
        }
    }

    *num_entries = matrix->len;
    return (LineNumberEntry *)g_array_free(matrix, FALSE);
}
