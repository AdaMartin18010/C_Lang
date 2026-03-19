/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\README.md
 * Line: 437
 * Language: c
 * Block ID: 55b42632
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DWARF 表达式操作码
enum DwarfExpressionOpcode {
    // 堆栈操作
    DW_OP_dup      = 0x12,  // 复制栈顶
    DW_OP_drop     = 0x13,  // 弹出栈顶
    DW_OP_pick     = 0x15,  // 选取栈中元素
    DW_OP_swap     = 0x16,  // 交换栈顶两个元素
    DW_OP_rot      = 0x17,  // 旋转栈顶三个元素

    // 算术运算
    DW_OP_abs      = 0x19,
    DW_OP_and      = 0x1a,
    DW_OP_div      = 0x1b,
    DW_OP_minus    = 0x1c,
    DW_OP_mod      = 0x1d,
    DW_OP_mul      = 0x1e,
    DW_OP_neg      = 0x1f,
    DW_OP_not      = 0x20,
    DW_OP_or       = 0x21,
    DW_OP_plus     = 0x22,
    DW_OP_shl      = 0x24,
    DW_OP_shr      = 0x25,
    DW_OP_shra     = 0x26,
    DW_OP_xor      = 0x27,

    // 内存访问
    DW_OP_deref    = 0x06,  // 解引用
    DW_OP_deref_size = 0x94,// 带大小限制的解引用
    DW_OP_xderef   = 0x18,  // 跨地址空间解引用

    // 寄存器访问
    DW_OP_reg0     = 0x50,  // 寄存器 0-31 的直接编码
    // ... DW_OP_reg31 = 0x6f
    DW_OP_regx     = 0x90,  // 任意寄存器编号
    DW_OP_breg0    = 0x70,  // 基址寄存器 + SLEB128 偏移
    // ... DW_OP_breg31 = 0x8f
    DW_OP_bregx    = 0x92,  // 任意基址寄存器 + 偏移

    // 字面量
    DW_OP_lit0     = 0x30,  // 压入无符号字面量 0-31
    // ... DW_OP_lit31 = 0x4f
    DW_OP_addr     = 0x03,  // 压入绝对地址
    DW_OP_const1u  = 0x08,  // 压入 1 字节无符号常量
    DW_OP_const1s  = 0x09,  // 压入 1 字节有符号常量
    DW_OP_const2u  = 0x0a,
    DW_OP_const2s  = 0x0b,
    DW_OP_const4u  = 0x0c,
    DW_OP_const4s  = 0x0d,
    DW_OP_const8u  = 0x0e,
    DW_OP_const8s  = 0x0f,
    DW_OP_constu   = 0x10,  // ULEB128 常量
    DW_OP_consts   = 0x11,  // SLEB128 常量

    // 特殊
    DW_OP_fbreg    = 0x91,  // 基于帧寄存器的偏移
    DW_OP_piece    = 0x93,  // 值的一部分（分散在多个位置）
    DW_OP_bit_piece = 0x9d, // 位的子集
    DW_OP_implicit_value = 0x9e, // 隐式值（常量）
    DW_OP_stack_value = 0x9f,    // 值在栈上（不在内存）
};
