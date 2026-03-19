/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 559
 * Language: c
 * Block ID: 4030f3ac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 获取节点高度
 */
static inline int avl_height(const TreeNode* node) {
    return node ? node->height : 0;
}

/**
 * 更新节点高度
 */
static inline void avl_update_height(TreeNode* node) {
    int left_h = avl_height(node->left);
    int right_h = avl_height(node->right);
    node->height = 1 + (left_h > right_h ? left_h : right_h);
}

/**
 * 获取平衡因子
 */
static inline int avl_balance_factor(const TreeNode* node) {
    return node ? avl_height(node->left) - avl_height(node->right) : 0;
}

/**
 * 右旋 (LL情况)
 *
 *     y              x
 *    / \            / \
 *   x   C   →     A   y
 *  / \               / \
 * A   B             B   C
 */
TreeNode* avl_rotate_right(TreeNode* y) {
    TreeNode* x = y->left;
    TreeNode* B = x->right;

    // 旋转
    x->right = y;
    y->left = B;

    // 更新高度
    avl_update_height(y);
    avl_update_height(x);

    return x;  // 新的根
}

/**
 * 左旋 (RR情况)
 *
 *   x                  y
 *  / \                / \
 * A   y      →      x   C
 *    / \           / \
 *   B   C         A   B
 */
TreeNode* avl_rotate_left(TreeNode* x) {
    TreeNode* y = x->right;
    TreeNode* B = y->left;

    // 旋转
    y->left = x;
    x->right = B;

    // 更新高度
    avl_update_height(x);
    avl_update_height(y);

    return y;  // 新的根
}
