/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\02_CFI_Stack_Reconstruction.md
 * Line: 220
 * Language: c
 * Block ID: fc5cc0ad
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 执行CFI程序，构建展开表
typedef struct {
    uint64_t pc;
    CFADefinition cfa;
    RegisterRule rules[REG_COUNT];
} UnwindRow;

// 执行CFI指令，更新展开上下文
int execute_cfi_instruction(
    uint8_t opcode,
    const uint8_t **operands,
    const CIE *cie,
    UnwindContext *ctx,
    GArray *unwind_table
) {
    switch (opcode) {
        case CFA_nop:
            // 无操作
            break;

        case CFA_advance_loc: {
            // 短格式: 操作数在opcode中
            uint8_t delta = opcode & 0x3F;
            ctx->pc += delta * cie->code_alignment_factor;
            break;
        }

        case CFA_advance_loc1: {
            uint8_t delta = *(*operands)++;
            ctx->pc += delta * cie->code_alignment_factor;
            break;
        }

        case CFA_advance_loc2: {
            uint16_t delta = read_uint16(*operands);
            *operands += 2;
            ctx->pc += delta * cie->code_alignment_factor;
            break;
        }

        case CFA_advance_loc4: {
            uint32_t delta = read_uint32(*operands);
            *operands += 4;
            ctx->pc += delta * cie->code_alignment_factor;
            break;
        }

        case CFA_set_loc: {
            uint64_t addr = read_uint64(*operands);
            *operands += 8;
            ctx->pc = addr;
            break;
        }

        case CFA_offset:
        case CFA_offset_extended: {
            size_t bytes;
            uint64_t reg = decode_uleb128(*operands, &bytes);
            *operands += bytes;
            uint64_t offset = decode_uleb128(*operands, &bytes);
            *operands += bytes;

            ctx->rules[reg].type = RULE_OFFSET;
            ctx->rules[reg].offset = offset * cie->data_alignment_factor;
            break;
        }

        case CFA_offset_extended_sf: {
            size_t bytes;
            uint64_t reg = decode_uleb128(*operands, &bytes);
            *operands += bytes;
            int64_t offset = decode_sleb128(*operands, &bytes);
            *operands += bytes;

            ctx->rules[reg].type = RULE_OFFSET;
            ctx->rules[reg].offset = offset * cie->data_alignment_factor;
            break;
        }

        case CFA_restore:
        case CFA_restore_extended: {
            size_t bytes;
            uint64_t reg = decode_uleb128(*operands, &bytes);
            *operands += bytes;

            // 恢复到CIE初始规则
            ctx->rules[reg] = cie->initial_rules[reg];
            break;
        }

        case CFA_undefined: {
            size_t bytes;
            uint64_t reg = decode_uleb128(*operands, &bytes);
            *operands += bytes;

            ctx->rules[reg].type = RULE_UNDEFINED;
            break;
        }

        case CFA_same_value: {
            size_t bytes;
            uint64_t reg = decode_uleb128(*operands, &bytes);
            *operands += bytes;

            ctx->rules[reg].type = RULE_SAME_VALUE;
            break;
        }

        case CFA_register: {
            size_t bytes;
            uint64_t reg1 = decode_uleb128(*operands, &bytes);
            *operands += bytes;
            uint64_t reg2 = decode_uleb128(*operands, &bytes);
            *operands += bytes;

            ctx->rules[reg1].type = RULE_REGISTER;
            ctx->rules[reg1].reg_num = reg2;
            break;
        }

        case CFA_def_cfa: {
            size_t bytes;
            uint64_t reg = decode_uleb128(*operands, &bytes);
            *operands += bytes;
            uint64_t offset = decode_uleb128(*operands, &bytes);
            *operands += bytes;

            ctx->cfa.type = CFA_REGISTER;
            ctx->cfa.reg.reg_num = reg;
            ctx->cfa.reg.offset = offset;
            break;
        }

        case CFA_def_cfa_sf: {
            size_t bytes;
            uint64_t reg = decode_uleb128(*operands, &bytes);
            *operands += bytes;
            int64_t offset = decode_sleb128(*operands, &bytes);
            *operands += bytes;

            ctx->cfa.type = CFA_REGISTER;
            ctx->cfa.reg.reg_num = reg;
            ctx->cfa.reg.offset = offset;
            break;
        }

        case CFA_def_cfa_register: {
            size_t bytes;
            uint64_t reg = decode_uleb128(*operands, &bytes);
            *operands += bytes;

            ctx->cfa.reg.reg_num = reg;
            break;
        }

        case CFA_def_cfa_offset: {
            size_t bytes;
            uint64_t offset = decode_uleb128(*operands, &bytes);
            *operands += bytes;

            ctx->cfa.reg.offset = offset;
            break;
        }

        case CFA_def_cfa_offset_sf: {
            size_t bytes;
            int64_t offset = decode_sleb128(*operands, &bytes);
            *operands += bytes;

            ctx->cfa.reg.offset = offset;
            break;
        }

        case CFA_def_cfa_expression: {
            size_t bytes;
            uint64_t len = decode_uleb128(*operands, &bytes);
            *operands += bytes;

            ctx->cfa.type = CFA_EXPRESSION;
            ctx->cfa.expr.expr = malloc(len);
            memcpy(ctx->cfa.expr.expr, *operands, len);
            ctx->cfa.expr.len = len;
            *operands += len;
            break;
        }

        case CFA_expression: {
            size_t bytes;
            uint64_t reg = decode_uleb128(*operands, &bytes);
            *operands += bytes;
            uint64_t len = decode_uleb128(*operands, &bytes);
            *operands += bytes;

            ctx->rules[reg].type = RULE_EXPRESSION;
            ctx->rules[reg].expression.expr = malloc(len);
            memcpy(ctx->rules[reg].expression.expr, *operands, len);
            ctx->rules[reg].expression.len = len;
            *operands += len;
            break;
        }

        case CFA_val_expression: {
            size_t bytes;
            uint64_t reg = decode_uleb128(*operands, &bytes);
            *operands += bytes;
            uint64_t len = decode_uleb128(*operands, &bytes);
            *operands += bytes;

            ctx->rules[reg].type = RULE_VAL_EXPRESSION;
            ctx->rules[reg].expression.expr = malloc(len);
            memcpy(ctx->rules[reg].expression.expr, *operands, len);
            ctx->rules[reg].expression.len = len;
            *operands += len;
            break;
        }

        case CFA_remember_state:
            // 压入状态栈（简化实现）
            break;

        case CFA_restore_state:
            // 弹出状态栈（简化实现）
            break;

        default:
            if ((opcode & 0xC0) == CFA_advance_loc) {
                // 短格式advance_loc
                uint8_t delta = opcode & 0x3F;
                ctx->pc += delta * cie->code_alignment_factor;
            } else if ((opcode & 0xF0) == CFA_offset) {
                // 短格式offset
                uint64_t reg = opcode & 0x0F;
                size_t bytes;
                uint64_t offset = decode_uleb128(*operands, &bytes);
                *operands += bytes;

                ctx->rules[reg].type = RULE_OFFSET;
                ctx->rules[reg].offset = offset * cie->data_alignment_factor;
            } else if ((opcode & 0xF0) == CFA_restore) {
                // 短格式restore
                uint64_t reg = opcode & 0x0F;
                ctx->rules[reg] = cie->initial_rules[reg];
            } else {
                fprintf(stderr, "Unknown CFI opcode: 0x%02x\n", opcode);
                return -1;
            }
            break;
    }

    return 0;
}
