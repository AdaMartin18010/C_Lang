/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
 * Line: 1012
 * Language: c
 * Block ID: 092cf8f3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: 栈帧示例

void stack_frame_example(int x, int y) {
    // x: %edi, y: %esi
    int local1 = x + y;     // 栈上的局部变量
    int local2 = x - y;

    // 调用其他函数
    printf("%d %d\n", local1, local2);
}
