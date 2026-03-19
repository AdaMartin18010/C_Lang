/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 326
 * Language: c
 * Block ID: 17ec4e08
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 访问栈上的参数
typedef struct {
    int64_t *return_addr;
    int64_t param7;  // 第一个栈参数
    int64_t param8;
    // ...
} StackParams;

int64_t access_stack_param(int64_t p1, int64_t p2, int64_t p3,
                            int64_t p4, int64_t p5, int64_t p6,
                            int64_t p7, int64_t p8) {
    // 获取栈参数（依赖于编译器实现）
    int64_t *rsp;
    __asm__ volatile("lea 8(%%rbp), %0" : "=r"(rsp));

    printf("第7参数: %ld\n", rsp[0]);
    printf("第8参数: %ld\n", rsp[1]);

    return p7 + p8;
}
