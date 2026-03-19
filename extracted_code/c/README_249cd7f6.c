/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\README.md
 * Line: 217
 * Language: c
 * Block ID: 249cd7f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 假设在 0x1000 处的代码需要引用 0x2000 处的变量

// R_X86_64_PC32 - 相对寻址重定位
// 计算: S + A - P
// S = 符号值 (0x2000), A = 加数, P = 重定位位置 (0x1005)
call printf      // 需要填充为 printf 地址 - 当前地址

// R_X86_64_32 - 绝对寻址重定位
// 计算: S + A
movl $global_var, %eax   // 需要填充为 global_var 的绝对地址

// R_X86_64_PLT32 - PLT 条目重定位 (用于动态链接)
call printf@PLT  // 跳转到过程链接表
