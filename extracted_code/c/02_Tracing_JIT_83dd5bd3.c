/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\02_Tracing_JIT.md
 * Line: 231
 * Language: c
 * Block ID: 83dd5bd3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * SSA(静态单赋值)形式的中间表示
 * 每个变量只被赋值一次，便于优化
 */

typedef enum {
    IR_NOP,
    IR_CONST,      // 常量加载
    IR_LOAD,       // 从栈/变量加载
    IR_STORE,      // 存储到变量
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_LT,         // 小于比较
    IR_GUARD,      // 类型/值守卫（guard）
    IR_PHI,        // SSA合并
    IR_RET
} IROpcode;

typedef uint16_t IRRef;
#define IRREF_NONE 0xFFFF

typedef struct {
    IROpcode op;
    IRRef op1;      // 操作数1
    IRRef op2;      // 操作数2
    int32_t const_val;  // 常量值
    uint8_t type;   // 值类型
} IRInstruction;

#define MAX_IR 512
static IRInstruction ir_buffer[MAX_IR];
static IRRef ir_count = 0;

// IR常量折叠优化
IRRef ir_emit_const(int32_t val) {
    // 查找已存在的常量
    for (IRRef i = 0; i < ir_count; i++) {
        if (ir_buffer[i].op == IR_CONST && ir_buffer[i].const_val == val) {
            return i;
        }
    }

    IRRef ref = ir_count++;
    ir_buffer[ref] = (IRInstruction){IR_CONST, IRREF_NONE, IRREF_NONE, val, 0};
    return ref;
}

// 发射算术运算（带常量折叠）
IRRef ir_emit_arith(IROpcode op, IRRef a, IRRef b) {
    // 常量折叠: c1 op c2 -> const(c1 op c2)
    if (ir_buffer[a].op == IR_CONST && ir_buffer[b].op == IR_CONST) {
        int32_t va = ir_buffer[a].const_val;
        int32_t vb = ir_buffer[b].const_val;
        int32_t result;
        switch (op) {
            case IR_ADD: result = va + vb; break;
            case IR_SUB: result = va - vb; break;
            case IR_MUL: result = va * vb; break;
            default: goto no_fold;
        }
        printf("[IR优化] 常量折叠: %d %s %d = %d\n",
               va, op == IR_ADD ? "+" : op == IR_SUB ? "-" : "*", vb, result);
        return ir_emit_const(result);
    }

no_fold:
    IRRef ref = ir_count++;
    ir_buffer[ref] = (IRInstruction){op, a, b, 0, 0};
    return ref;
}

// 发射守卫（guard）
IRRef ir_emit_guard(IRRef cond, bool expected) {
    IRRef ref = ir_count++;
    ir_buffer[ref] = (IRInstruction){IR_GUARD, cond, IRREF_NONE, expected, 0};
    printf("[IR] 守卫: r%u == %s\n", cond, expected ? "true" : "false");
    return ref;
}
