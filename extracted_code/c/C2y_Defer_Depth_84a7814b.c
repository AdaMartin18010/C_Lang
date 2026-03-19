/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 617
 * Language: c
 * Block ID: 84a7814b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void deferred_evaluation(void) {
    int x = 10;
    defer {
        printf("x = %d\n", x);  // 退出时x的值
    }

    x = 20;
    printf("After change: x = %d\n", x);  // 输出 20
}  // defer执行，输出 "x = 20"
