/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\README.md
 * Line: 117
 * Language: c
 * Block ID: f49e846b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例: 编译器无法重排, 但硬件可以乱序执行
void ooo_example(int* arr, int n) {
    int sum = 0;
    int prod = 1;

    // 循环内指令可以乱序执行
    for (int i = 0; i < n; i++) {
        sum += arr[i];       // 可能需要等待内存
        prod *= arr[i];      // 如果 sum 等待, prod 可以先执行!
    }
}

// 寄存器重命名消除 WAR/WAW 冒险
void register_renaming() {
    int a = 1;   // 物理寄存器 R1 ← 1
    a = 2;       // 物理寄存器 R2 ← 2 (不是 R1!)
    int b = a;   // 读取 R2
    // 通过重命名, 消除了 WAW 依赖
}
