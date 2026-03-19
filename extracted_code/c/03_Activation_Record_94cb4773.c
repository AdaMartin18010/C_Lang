/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 440
 * Language: c
 * Block ID: 94cb4773
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 栈溢出保护（Stack Canary）
void vulnerable_function(char *input) {
    char buffer[64];

    // 编译器插入：
    // mov rax, QWORD PTR fs:40    ; 读取金丝雀值
    // mov QWORD PTR [rbp-8], rax  ; 保存在栈上

    strcpy(buffer, input);  // 潜在的缓冲区溢出

    // 函数返回前检查：
    // mov rax, QWORD PTR [rbp-8]
    // xor rax, QWORD PTR fs:40
    // je .L_no_overflow
    // call __stack_chk_fail       ; 检测到溢出！
    // .L_no_overflow:
}
