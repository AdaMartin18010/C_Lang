/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 155
 * Language: c
 * Block ID: 72d5cc6f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 查找操作中的重复模式
Node* find_node(Tree* tree, Key key) {
    Node* result = search_tree(tree, key);  // 声明
    if (result != NULL) {                    // 检查
        return result;                       // 使用
    }
    // result 在后续代码中不再使用，但仍然可见

    // 另一种查找
    Node* alt_result = search_alternate(tree, key);  // 又一个声明
    if (alt_result != NULL) {                         // 又一个检查
        return alt_result;
    }
    // alt_result 也继续可见

    return NULL;
}
