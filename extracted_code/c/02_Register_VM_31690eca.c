/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\02_Register_VM.md
 * Line: 138
 * Language: c
 * Block ID: 31690eca
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 三地址码指令格式 - 32位固定长度 */
/* | 操作码(8) | A(8) | B(8) | C(8) | - 三寄存器模式 */
/* | 操作码(8) | A(8) | Bx(16)      | - 寄存器+立即数 */
/* | 操作码(8) | Ax(24)             | - 跳转/常量索引 */

typedef uint32_t RVInstruction;

/* 指令字段提取宏 */
#define GET_OPCODE(i)   (((i) >> 24) & 0xFF)
#define GET_A(i)        (((i) >> 16) & 0xFF)
#define GET_B(i)        (((i) >>  8) & 0xFF)
#define GET_C(i)        (((i)      ) & 0xFF)
#define GET_Bx(i)       (((i)      ) & 0xFFFF)
#define GET_sBx(i)      ((int16_t)((i) & 0xFFFF))
#define GET_Ax(i)       (((i)      ) & 0xFFFFFF)

/* 指令构造宏 */
#define MK_OP_ABC(op, a, b, c) \
    (((op) << 24) | ((a) << 16) | ((b) << 8) | (c))
#define MK_OP_ABx(op, a, bx) \
    (((op) << 24) | ((a) << 16) | (bx))
