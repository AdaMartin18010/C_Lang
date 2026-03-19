/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\06_Coroutine_Fiber.md
 * Line: 170
 * Language: c
 * Block ID: 7578c564
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* x86_64 汇编上下文切换 - 更快 */

typedef struct {
    void* rsp;  /* 栈指针 */
    void* rbp;
    void* rip;  /* 指令指针 */
    void* rbx;
    void* r12;
    void* r13;
    void* r14;
    void* r15;
} context_t;

/* 保存当前上下文到from，恢复to上下文 */
__attribute__((naked)) void context_switch(context_t* from, context_t* to) {
    __asm__ volatile (
        /* 保存当前寄存器 */
        "movq %rsp, 0(%rdi)\n\t"
        "movq %rbp, 8(%rdi)\n\t"
        "movq (%rsp), %rax\n\t"  /* 返回地址 */
        "movq %rax, 16(%rdi)\n\t"
        "movq %rbx, 24(%rdi)\n\t"
        "movq %r12, 32(%rdi)\n\t"
        "movq %r13, 40(%rdi)\n\t"
        "movq %r14, 48(%rdi)\n\t"
        "movq %r15, 56(%rdi)\n\t"

        /* 恢复目标寄存器 */
        "movq 56(%rsi), %r15\n\t"
        "movq 48(%rsi), %r14\n\t"
        "movq 40(%rsi), %r13\n\t"
        "movq 32(%rsi), %r12\n\t"
        "movq 24(%rsi), %rbx\n\t"
        "movq 16(%rsi), %rax\n\t"
        "movq %rax, (%rsp)\n\t"   /* 恢复返回地址 */
        "movq 8(%rsi), %rbp\n\t"
        "movq 0(%rsi), %rsp\n\t"

        "ret\n\t"
    );
}
