/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 165
 * Language: c
 * Block ID: 18908458
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// System V AMD64调用约定
/*
参数传递寄存器（按顺序）：
  1. %rdi
  2. %rsi
  3. %rdx
  4. %rcx
  5. %r8
  6. %r9
  7+  栈上

返回值：%rax (整数), %xmm0 (浮点)

调用者保存：%rax, %rcx, %rdx, %rsi, %rdi, %r8-%r11
被调用者保存：%rbx, %rbp, %r12-%r15
*/

// 示例函数
int64_t example_function(int64_t a, int64_t b, int64_t c,
                          int64_t d, int64_t e, int64_t f,
                          int64_t g, int64_t h);

// 汇编调用序列：
/*
mov $1, %rdi      # a
mov $2, %rsi      # b
mov $3, %rdx      # c
mov $4, %rcx      # d
mov $5, %r8       # e
mov $6, %r9       # f
push $8           # h (从右向左)
push $7           # g
call example_function
add $16, %rsp     # 清理参数
*/
