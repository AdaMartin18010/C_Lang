/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 137
 * Language: c
 * Block ID: 3f1d7593
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 活动记录的结构化表示
typedef struct {
    struct ActivationRecord *prev;  // 保存的RBP (动态链)
    void *return_addr;               // 返回地址

    // 参数区（取决于调用约定）
    union {
        struct {
            int64_t rdi;
            int64_t rsi;
            int64_t rdx;
            int64_t rcx;
            int64_t r8;
            int64_t r9;
        } reg_params;
        int64_t stack_params[0];  // 变长
    } params;

    // 局部变量区（编译器确定偏移）
    char locals[0];  // 变长
} ActivationRecord;
