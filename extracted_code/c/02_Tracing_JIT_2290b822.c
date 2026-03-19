/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\02_Tracing_JIT.md
 * Line: 556
 * Language: c
 * Block ID: 2290b822
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 生成字节码: 计算 1+2+...+10
 *
 * 伪代码:
 *   sum = 0
 *   i = 1
 * loop:
 *   if i > 10 goto end
 *   sum = sum + i
 *   i = i + 1
 *   goto loop
 * end:
 *   print sum
 */
void generate_test_program(void) {
    BCIndex loop_header = 3;

    int idx = 0;
    // sum = 0
    bytecode[idx++] = BC_OP(OP_LOAD_CONST, 0);
    bytecode[idx++] = BC_OP(OP_STORE_VAR, 0);  // var[0] = sum

    // i = 1
    bytecode[idx++] = BC_OP(OP_LOAD_CONST, 1);
    bytecode[idx++] = BC_OP(OP_STORE_VAR, 1);  // var[1] = i

    // loop: if i > 10 goto end
    bytecode[idx++] = BC_OP(OP_LOAD_VAR, 1);
    bytecode[idx++] = BC_OP(OP_LOAD_CONST, 10);
    bytecode[idx++] = BC_OP(OP_LT, 0);         // 10 < i 即 i > 10
    bytecode[idx++] = BC_OP(OP_JMP_IF_FALSE, 10); // 若为假(不大于)继续
    bytecode[idx++] = BC_OP(OP_JMP, 13);       // 否则跳转到 end

    // sum = sum + i
    bytecode[idx++] = BC_OP(OP_LOAD_VAR, 0);
    bytecode[idx++] = BC_OP(OP_LOAD_VAR, 1);
    bytecode[idx++] = BC_OP(OP_ADD, 0);
    bytecode[idx++] = BC_OP(OP_STORE_VAR, 0);

    // i = i + 1
    bytecode[idx++] = BC_OP(OP_LOAD_VAR, 1);
    bytecode[idx++] = BC_OP(OP_LOAD_CONST, 1);
    bytecode[idx++] = BC_OP(OP_ADD, 0);
    bytecode[idx++] = BC_OP(OP_STORE_VAR, 1);

    // goto loop
    bytecode[idx++] = BC_OP(OP_LOOP, loop_header);

    // end: print sum
    bytecode[idx++] = BC_OP(OP_LOAD_VAR, 0);
    bytecode[idx++] = BC_OP(OP_PRINT, 0);
    bytecode[idx++] = BC_OP(OP_RET, 0);

    bc_len = idx;
}

int main(void) {
    printf("=== 简化版追踪JIT演示 ===\n\n");

    generate_test_program();
    printf("程序: 计算 1+2+...+10\n\n");

    printf("=== 解释执行 ===\n");
    int32_t result = interpret(0);
    printf("结果: %d\n", result);

    printf("\n=== 热点统计 ===\n");
    for (int i = 0; i < loop_count; i++) {
        printf("循环 @%u: 执行 %u 次, %s\n",
               loop_counters[i].loop_header,
               loop_counters[i].counter,
               loop_counters[i].is_hot ? "已JIT" : "未触发");
    }

    return 0;
}
