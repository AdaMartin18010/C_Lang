/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\02_CFI_Stack_Reconstruction.md
 * Line: 172
 * Language: c
 * Block ID: d82153ba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DWARF CFI指令操作码
typedef enum {
    // 高2位=操作码，低6位=操作数（对于部分指令）

    // 0x40-0x4F: DW_CFA_advance_loc (delta = opcode & 0x3F)
    CFA_advance_loc = 0x40,         // 位置 += delta * code_align

    // 0x50-0x5F: DW_CFA_offset (reg = opcode & 0x0F)
    CFA_offset = 0x50,              // 规则[reg] = OFFSET, factored_offset

    // 0x60-0x6F: DW_CFA_restore (reg = opcode & 0x0F)
    CFA_restore = 0x60,             // 规则[reg] = CIE初始规则

    // 扩展操作码（0x00-0x0F）
    CFA_nop = 0x00,
    CFA_set_loc = 0x01,             // 设置绝对位置
    CFA_advance_loc1 = 0x02,        // 1字节delta
    CFA_advance_loc2 = 0x03,        // 2字节delta
    CFA_advance_loc4 = 0x04,        // 4字节delta
    CFA_offset_extended = 0x05,     // ULEB128寄存器, ULEB128偏移
    CFA_restore_extended = 0x06,    // ULEB128寄存器
    CFA_undefined = 0x07,           // ULEB128寄存器 → 规则=UNDEFINED
    CFA_same_value = 0x08,          // ULEB128寄存器 → 规则=SAME_VALUE
    CFA_register = 0x09,            // ULEB128 reg1, ULEB128 reg2
    CFA_remember_state = 0x0A,      // 压入状态栈
    CFA_restore_state = 0x0B,       // 弹出状态栈
    CFA_def_cfa = 0x0C,             // ULEB128寄存器, ULEB128偏移
    CFA_def_cfa_register = 0x0D,    // ULEB128寄存器
    CFA_def_cfa_offset = 0x0E,      // ULEB128偏移
    CFA_def_cfa_expression = 0x0F,  // BLOCK (DWARF表达式)

    CFA_expression = 0x10,          // ULEB128寄存器, BLOCK
    CFA_offset_extended_sf = 0x11,  // SLEB128偏移（有符号）
    CFA_def_cfa_sf = 0x12,          // ULEB128寄存器, SLEB128偏移
    CFA_def_cfa_offset_sf = 0x13,   // SLEB128偏移
    CFA_val_offset = 0x14,
    CFA_val_offset_sf = 0x15,
    CFA_val_expression = 0x16,      // ULEB128寄存器, BLOCK
    CFA_GNU_args_size = 0x2E,       // GNU扩展
    CFA_GNU_negative_offset_extended = 0x2F,
} CFI_Opcode;
