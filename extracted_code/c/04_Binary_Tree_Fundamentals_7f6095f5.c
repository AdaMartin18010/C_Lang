/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 634
 * Language: c
 * Block ID: 7f6095f5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * AVL 插入
 * 插入后从插入点向上重新平衡
 */
TreeNode* avl_insert(TreeNode* node, int key) {
    // 1. 标准BST插入
    if (node == nullptr) return tree_node_create(key);

    if (key < node->data) {
        node->left = avl_insert(node->left, key);
    } else if (key > node->data) {
        node->right = avl_insert(node->right, key);
    } else {
        return node;  // 重复值
    }

    // 2. 更新高度
    avl_update_height(node);

    // 3. 获取平衡因子
    int balance = avl_balance_factor(node);

    // 4. 重新平衡

    // LL情况
    if (balance > 1 && key < node->left->data) {
        return avl_rotate_right(node);
    }

    // RR情况
    if (balance < -1 && key > node->right->data) {
        return avl_rotate_left(node);
    }

    // LR情况
    if (balance > 1 && key > node->left->data) {
        node->left = avl_rotate_left(node->left);
        return avl_rotate_right(node);
    }

    // RL情况
    if (balance < -1 && key < node->right->data) {
        node->right = avl_rotate_right(node->right);
        return avl_rotate_left(node);
    }

    return node;
}
