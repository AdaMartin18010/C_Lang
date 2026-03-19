/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\04_Control_Flow.md
 * Line: 186
 * Language: c
 * Block ID: 15654168
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译器通常优化switch为跳转表或二叉搜索
// 密集case值：跳转表 O(1)
switch (opcode) {
    case 0: add(); break;
    case 1: sub(); break;
    case 2: mul(); break;
    // ... 连续值
}

// 稀疏case值：二叉搜索 O(log n)
switch (error_code) {
    case 0x1000: ...
    case 0x8000: ...
    case 0xF000: ...
}
