/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\01_DWARF_Deserialization.md
 * Line: 755
 * Language: c
 * Block ID: b12da21f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DWARF表达式操作码
typedef enum {
    DW_OP_addr = 0x03,              // 常量地址
    DW_OP_deref = 0x06,             // 解引用
    DW_OP_const1u = 0x08,           // 1字节无符号常量
    DW_OP_const1s = 0x09,           // 1字节有符号常量
    DW_OP_const2u = 0x0a,
    DW_OP_const2s = 0x0b,
    DW_OP_const4u = 0x0c,
    DW_OP_const4s = 0x0d,
    DW_OP_const8u = 0x0e,
    DW_OP_const8s = 0x0f,
    DW_OP_constu = 0x10,            // ULEB128常量
    DW_OP_consts = 0x11,            // SLEB128常量
    DW_OP_dup = 0x12,               // 复制栈顶
    DW_OP_drop = 0x13,              // 丢弃栈顶
    DW_OP_over = 0x14,              // 复制次栈顶
    DW_OP_pick = 0x15,              // 复制栈中元素
    DW_OP_swap = 0x16,              // 交换栈顶两个元素
    DW_OP_rot = 0x17,               // 旋转栈顶三个元素
    DW_OP_xderef = 0x18,            // 扩展解引用
    DW_OP_abs = 0x19,               // 绝对值
    DW_OP_and = 0x1a,               // 按位与
    DW_OP_div = 0x1b,               // 除法
    DW_OP_minus = 0x1c,             // 减法
    DW_OP_mod = 0x1d,               // 取模
    DW_OP_mul = 0x1e,               // 乘法
    DW_OP_neg = 0x1f,               // 取负
    DW_OP_not = 0x20,               // 按位非
    DW_OP_or = 0x21,                // 按位或
    DW_OP_plus = 0x22,              // 加法
    DW_OP_plus_uconst = 0x23,       // 加无符号常量
    DW_OP_shl = 0x24,               // 左移
    DW_OP_shr = 0x25,               // 逻辑右移
    DW_OP_shra = 0x26,              // 算术右移
    DW_OP_xor = 0x27,               // 按位异或
    DW_OP_bra = 0x28,               // 分支
    DW_OP_eq = 0x29,                // 等于
    DW_OP_ge = 0x2a,                // 大于等于
    DW_OP_gt = 0x2b,                // 大于
    DW_OP_le = 0x2c,                // 小于等于
    DW_OP_lt = 0x2d,                // 小于
    DW_OP_ne = 0x2e,                // 不等于
    DW_OP_skip = 0x2f,              // 跳过
    DW_OP_lit0 = 0x30,              // 压入字面量0
    // ... DW_OP_lit1 to DW_OP_lit31
    DW_OP_reg0 = 0x50,              // 读取寄存器0
    // ... DW_OP_reg1 to DW_OP_reg31
    DW_OP_breg0 = 0x70,             // 基址寄存器0 + SLEB128偏移
    // ... DW_OP_breg1 to DW_OP_breg31
    DW_OP_regx = 0x90,              // 读取寄存器（ULEB128编号）
    DW_OP_fbreg = 0x91,             // 基于帧寄存器
    DW_OP_bregx = 0x92,             // 基址寄存器 + 偏移
    DW_OP_piece = 0x93,             // 片段
    DW_OP_deref_size = 0x94,        // 指定大小解引用
    DW_OP_xderef_size = 0x95,
    DW_OP_nop = 0x96,
    // DWARFv3+
    DW_OP_push_object_address = 0x97,
    DW_OP_call2 = 0x98,
    DW_OP_call4 = 0x99,
    DW_OP_call_ref = 0x9a,
    DW_OP_form_tls_address = 0x9b,
    DW_OP_call_frame_cfa = 0x9c,
    DW_OP_bit_piece = 0x9d,
    // DWARFv4+
    DW_OP_implicit_value = 0x9e,
    DW_OP_stack_value = 0x9f,
    // DWARFv5
    DW_OP_implicit_pointer = 0xa0,
    DW_OP_addrx = 0xa1,
    DW_OP_constx = 0xa2,
    DW_OP_entry_value = 0xa3,
    DW_OP_const_type = 0xa4,
    DW_OP_regval_type = 0xa5,
    DW_OP_deref_type = 0xa6,
    DW_OP_xderef_type = 0xa7,
    DW_OP_convert = 0xa8,
    DW_OP_reinterpret = 0xa9,
} Dwarf_Op;

// 执行DWARF表达式
int64_t evaluate_dwarf_expression(
    const uint8_t *expr,
    size_t expr_len,
    uint64_t *registers,    // 寄存器值数组
    uint64_t cfa,           // 规范帧地址
    uint64_t *result
) {
    // 基于栈的虚拟机
    uint64_t stack[64];
    int sp = 0;  // 栈指针

    #define PUSH(v) do { if (sp >= 64) return -1; stack[sp++] = (v); } while(0)
    #define POP() (sp > 0 ? stack[--sp] : 0)
    #define PEEK() (sp > 0 ? stack[sp-1] : 0)
    #define PICK(n) (sp > (n) ? stack[sp-1-(n)] : 0)

    size_t pc = 0;
    while (pc < expr_len) {
        uint8_t opcode = expr[pc++];

        switch (opcode) {
            case DW_OP_addr:
                PUSH(read_uint64(expr + pc));
                pc += 8;
                break;

            case DW_OP_const1u:
                PUSH(expr[pc++]);
                break;

            case DW_OP_const1s:
                PUSH((int8_t)expr[pc++]);
                break;

            case DW_OP_constu: {
                size_t bytes;
                PUSH(decode_uleb128(expr + pc, &bytes));
                pc += bytes;
                break;
            }

            case DW_OP_consts: {
                size_t bytes;
                PUSH(decode_sleb128(expr + pc, &bytes));
                pc += bytes;
                break;
            }

            case DW_OP_dup:
                PUSH(PEEK());
                break;

            case DW_OP_drop:
                POP();
                break;

            case DW_OP_pick: {
                uint8_t n = expr[pc++];
                PUSH(PICK(n));
                break;
            }

            case DW_OP_swap: {
                uint64_t top = POP();
                uint64_t second = POP();
                PUSH(top);
                PUSH(second);
                break;
            }

            case DW_OP_rot: {
                uint64_t a = POP();
                uint64_t b = POP();
                uint64_t c = POP();
                PUSH(b);
                PUSH(a);
                PUSH(c);
                break;
            }

            case DW_OP_deref: {
                uint64_t addr = POP();
                PUSH(*(uint64_t *)addr);  // 危险：需要安全读取
                break;
            }

            case DW_OP_plus: {
                uint64_t b = POP();
                uint64_t a = POP();
                PUSH(a + b);
                break;
            }

            case DW_OP_minus: {
                uint64_t b = POP();
                uint64_t a = POP();
                PUSH(a - b);
                break;
            }

            case DW_OP_mul: {
                uint64_t b = POP();
                uint64_t a = POP();
                PUSH(a * b);
                break;
            }

            case DW_OP_and: {
                uint64_t b = POP();
                uint64_t a = POP();
                PUSH(a & b);
                break;
            }

            case DW_OP_or: {
                uint64_t b = POP();
                uint64_t a = POP();
                PUSH(a | b);
                break;
            }

            case DW_OP_xor: {
                uint64_t b = POP();
                uint64_t a = POP();
                PUSH(a ^ b);
                break;
            }

            case DW_OP_shl: {
                uint64_t b = POP();
                uint64_t a = POP();
                PUSH(a << b);
                break;
            }

            case DW_OP_shr: {
                uint64_t b = POP();
                uint64_t a = POP();
                PUSH(a >> b);
                break;
            }

            case DW_OP_eq: {
                uint64_t b = POP();
                uint64_t a = POP();
                PUSH(a == b ? 1 : 0);
                break;
            }

            case DW_OP_lt: {
                uint64_t b = POP();
                uint64_t a = POP();
                PUSH(a < b ? 1 : 0);
                break;
            }

            case DW_OP_gt: {
                uint64_t b = POP();
                uint64_t a = POP();
                PUSH(a > b ? 1 : 0);
                break;
            }

            case DW_OP_plus_uconst: {
                size_t bytes;
                uint64_t val = decode_uleb128(expr + pc, &bytes);
                pc += bytes;
                PUSH(POP() + val);
                break;
            }

            case DW_OP_call_frame_cfa:
                PUSH(cfa);
                break;

            case DW_OP_fbreg: {
                size_t bytes;
                int64_t offset = decode_sleb128(expr + pc, &bytes);
                pc += bytes;
                PUSH(cfa + offset);
                break;
            }

            default:
                if (opcode >= DW_OP_lit0 && opcode <= DW_OP_lit0 + 31) {
                    // DW_OP_litN 压入字面量 N
                    PUSH(opcode - DW_OP_lit0);
                } else if (opcode >= DW_OP_reg0 && opcode <= DW_OP_reg0 + 31) {
                    // DW_OP_regN 读取寄存器N的值
                    PUSH(registers[opcode - DW_OP_reg0]);
                } else if (opcode >= DW_OP_breg0 && opcode <= DW_OP_breg0 + 31) {
                    // DW_OP_bregN: 读取寄存器N + SLEB128偏移
                    size_t bytes;
                    int64_t offset = decode_sleb128(expr + pc, &bytes);
                    pc += bytes;
                    PUSH(registers[opcode - DW_OP_breg0] + offset);
                } else {
                    fprintf(stderr, "Unknown DWARF opcode: 0x%02x\n", opcode);
                    return -1;
                }
                break;
        }
    }

    #undef PUSH
    #undef POP
    #undef PEEK
    #undef PICK

    if (sp > 0) {
        *result = stack[sp - 1];
        return 0;
    }
    return -1;
}
