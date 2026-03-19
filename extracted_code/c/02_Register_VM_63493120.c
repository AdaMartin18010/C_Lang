/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\02_Register_VM.md
 * Line: 168
 * Language: c
 * Block ID: 63493120
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef enum {
    /* 数据移动 */
    OP_MOVE,      /* A = B */
    OP_LOADK,     /* A = Kst(Bx) */
    OP_LOADNIL,   /* A...A+B = nil */
    OP_LOADBOOL,  /* A = B; if C then PC++ */

    /* 算术运算 - A = B op C */
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_POW,
    OP_ADDI,      /* A = B + C (C为立即数) */

    /* 位运算 */
    OP_BAND, OP_BOR, OP_BXOR, OP_SHL, OP_SHR,

    /* 比较与跳转 */
    OP_EQ,        /* if (B == C) ~= A then PC++ */
    OP_LT,        /* if (B < C) ~= A then PC++ */
    OP_LE,        /* if (B <= C) ~= A then PC++ */
    OP_TEST,      /* if (R[A] <=> C) then PC++ */
    OP_JMP,       /* PC += sBx */

    /* 函数调用 */
    OP_CALL,      /* R[A..A+C-2] = R[A](R[A+1..A+B-1]) */
    OP_RETURN,    /* return R[A..A+B-2] */
    OP_CLOSURE,   /* R[A] = closure(KPROTO[Bx]) */

    /* 表操作 */
    OP_NEWTABLE,  /* R[A] = {} */
    OP_GETTABLE,  /* R[A] = R[B][RK(C)] */
    OP_SETTABLE,  /* R[A][RK(B)] = RK(C) */

    /* 元操作 */
    OP_SELF,      /* R[A+1] = R[B]; R[A] = R[B][RK(C)] */
    OP_LEN,       /* R[A] = #R[B] */
    OP_CONCAT,    /* R[A] = R[B]..R[B+1].. ... ..R[C] */

    OP_COUNT
} RVOpCode;
