/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_New_Proposals_2026.md
 * Line: 306
 * Language: c
 * Block ID: 1a50e2ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C2y 提案 (N3745)

// 类似于C++的依赖类型
struct Node<T> {
    T value;
    struct Node<T>* next;  // 依赖类型参数
};

// 使用
struct Node<int> int_list;
struct Node<double> double_list;
