/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\02_Tracing_JIT.md
 * Line: 315
 * Language: c
 * Block ID: 1afc0198
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 将记录的trace翻译为IR
 */

typedef struct {
    int32_t value;
    bool is_const;
} StackSlot;

static StackSlot stack[16];
static int stack_top = 0;
static IRRef var_map[16];  // 变量到IR引用的映射

void push_ir(IRRef ref) {
    stack[stack_top++].value = ref;
}

IRRef pop_ir(void) {
    return stack[--stack_top].value;
}

// 将trace编译为IR
void compile_trace_to_ir(const Trace *trace) {
    printf("\n=== IR 生成 ===\n");

    ir_count = 0;
    stack_top = 0;

    for (int i = 0; i < trace->length; i++) {
        const Instruction *instr = &trace->ops[i];

        switch (instr->op) {
            case OP_LOAD_CONST: {
                IRRef ref = ir_emit_const(instr->operand);
                push_ir(ref);
                break;
            }

            case OP_LOAD_VAR: {
                push_ir(var_map[instr->operand]);
                break;
            }

            case OP_STORE_VAR: {
                var_map[instr->operand] = pop_ir();
                break;
            }

            case OP_ADD: {
                IRRef b = pop_ir();
                IRRef a = pop_ir();
                push_ir(ir_emit_arith(IR_ADD, a, b));
                break;
            }

            case OP_SUB: {
                IRRef b = pop_ir();
                IRRef a = pop_ir();
                push_ir(ir_emit_arith(IR_SUB, a, b));
                break;
            }

            case OP_MUL: {
                IRRef b = pop_ir();
                IRRef a = pop_ir();
                push_ir(ir_emit_arith(IR_MUL, a, b));
                break;
            }

            case OP_LT: {
                IRRef b = pop_ir();
                IRRef a = pop_ir();
                IRRef ref = ir_count++;
                ir_buffer[ref] = (IRInstruction){IR_LT, a, b, 0, 0};
                push_ir(ref);
                break;
            }

            case OP_JMP_IF_FALSE: {
                // 守卫：条件必须为真
                IRRef cond = pop_ir();
                ir_emit_guard(cond, true);
                break;
            }

            default:
                break;
        }
    }

    printf("共生成 %u 条 IR 指令\n\n", ir_count);
}
