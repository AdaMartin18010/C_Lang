/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 237
 * Language: c
 * Block ID: cbc0ffae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. 基本变量声明
if (int x = compute_value(); x > 0) {
    // x 在这里可用
}

// 2. 多变量声明
if (int a = get_a(), b = get_b(); a + b > 0) {
    // a 和 b 都可用
}

// 3. 带初始化的指针声明
if (Node* node = find_node(tree, key); node != NULL) {
    // node 可用
}

// 4. 复合类型声明
if (struct Data data = load_data(); data.valid) {
    // data 可用
}

// 5. 类型定义声明（限制作用域）
if (typedef int MyInt; true) {
    MyInt x = 10;  // MyInt 在此作用域内
}
// MyInt 在这里不可用
