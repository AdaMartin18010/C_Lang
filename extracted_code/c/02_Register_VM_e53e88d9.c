/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\02_Register_VM.md
 * Line: 211
 * Language: c
 * Block ID: e53e88d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 核心执行循环 - 使用线程化代码优化 */
#define VM_CASE(op)     case op:
#define VM_BREAK        break

void rv_execute(RVRegisterFile *rf, RVInstruction *code, RVValue *k) {
    static const void *dispatch_table[] = {
        &&L_OP_MOVE, &&L_OP_LOADK, &&L_OP_LOADNIL, &&L_OP_LOADBOOL,
        &&L_OP_ADD, &&L_OP_SUB, &&L_OP_MUL, &&L_OP_DIV,
        /* ... 其他操作码 ... */
    };

    #define DISPATCH() goto *dispatch_table[GET_OPCODE(code[rf->pc++])]

    RVValue *ra, *rb, *rc;
    RVInstruction i;

    DISPATCH();

L_OP_MOVE:
    i = code[rf->pc - 1];
    ra = rf->base + GET_A(i);
    rb = rf->base + GET_B(i);
    *ra = *rb;
    DISPATCH();

L_OP_ADD:
    i = code[rf->pc - 1];
    ra = rf->base + GET_A(i);
    rb = rf->base + GET_B(i);
    rc = rf->base + GET_C(i);

    if (rv_isint(rb) && rv_isint(rc)) {
        rv_setint(ra, rv_toint(rb) + rv_toint(rc));
    } else {
        rv_arith_add(rf, ra, rb, rc);
    }
    DISPATCH();

L_OP_LOADK:
    i = code[rf->pc - 1];
    ra = rf->base + GET_A(i);
    *ra = k[GET_Bx(i)];
    DISPATCH();

L_OP_JMP:
    i = code[rf->pc - 1];
    rf->pc += GET_sBx(i);
    DISPATCH();

L_OP_CALL:
    i = code[rf->pc - 1];
    int a = GET_A(i);
    int b = GET_B(i);
    int c = GET_C(i);

    rf->top = rf->base + a + (b > 0 ? b - 1 : 0);
    rv_call(rf, rf->base + a, c - 1);
    DISPATCH();

    #undef DISPATCH
}
