/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\01_Language_Extensions.md
 * Line: 305
 * Language: c
 * Block ID: 495467e8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误的约束
__asm__("movl %0, %%eax" : : "r" (var));  // 忘记输出约束

// ✅ 正确的输入输出描述
__asm__("movl %[in], %[out]"
    : [out] "=r" (output)
    : [in] "r" (input)
    : "eax"
);
