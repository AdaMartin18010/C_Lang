/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 392
 * Language: c
 * Block ID: 6a318de7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * BST 插入
 * 时间复杂度: O(h)
 */
TreeNode* bst_insert(TreeNode* root, int key) {
    if (root == nullptr) {
        return tree_node_create(key);
    }

    if (key < root->data) {
        root->left = bst_insert(root->left, key);
    } else if (key > root->data) {
        root->right = bst_insert(root->right, key);
    }
    // key == root->data: 重复值，不插入

    return root;
}
