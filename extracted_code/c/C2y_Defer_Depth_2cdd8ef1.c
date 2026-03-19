/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2087
 * Language: c
 * Block ID: 2cdd8ef1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译器在调用longjmp前插入defer执行代码
void modified_longjmp(jmp_buf env, int val) {
    // 1. 执行当前作用域的defer
    defer_execute_up_to(env);

    // 2. 执行longjmp
    real_longjmp(env, val);
}
