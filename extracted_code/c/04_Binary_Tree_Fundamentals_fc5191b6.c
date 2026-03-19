/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 508
 * Language: c
 * Block ID: fc5191b6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * BST 范围查询 [low, high]
 * 时间复杂度: O(n) 最坏，O(log n + k) 平均，k为结果数
 */
void bst_range_query(TreeNode* root, int low, int high, int* result, int* count) {
    if (root == nullptr) return;

    // 剪枝: 如果当前值大于low，查左子树
    if (root->data > low) {
        bst_range_query(root->left, low, high, result, count);
    }

    // 当前值在范围内
    if (root->data >= low && root->data <= high) {
        result[(*count)++] = root->data;
    }

    // 剪枝: 如果当前值小于high，查右子树
    if (root->data < high) {
        bst_range_query(root->right, low, high, result, count);
    }
}
