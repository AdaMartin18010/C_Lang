/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 205
 * Language: c
 * Block ID: ba0b7570
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Windows x64调用约定
/*
参数传递寄存器（按顺序）：
  1. %rcx (整数) / %xmm0 (浮点)
  2. %rdx (整数) / %xmm1 (浮点)
  3. %r8  (整数) / %xmm2 (浮点)
  4. %r9  (整数) / %xmm3 (浮点)
  5+  栈上（32字节shadow space预留）

返回值：%rax (整数), %xmm0 (浮点)

调用者分配：32字节 "shadow space"（即使参数<4个）
栈对齐：16字节
*/

// Windows ABI要求被调用函数可以使用shadow space
void windows_function(int a, int b, int c, int d) {
    // 编译器保证在进入时RSP+8到RSP+40是可用空间
    // 函数可以在这里保存参数寄存器
}
