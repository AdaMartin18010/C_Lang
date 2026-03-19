/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 603
 * Language: c
 * Block ID: 75dbc62e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void evaluation_timing(void) {
    int x = 10;
    defer printf("x = %d\n", x);  // x=10被保存

    x = 20;
    printf("After change: x = %d\n", x);  // 输出 20
}  // defer执行，输出 "x = 10"
