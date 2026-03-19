/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 415
 * Language: c
 * Block ID: 1a8a3145
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * BST 删除 (最复杂的操作)
 * 三种情况:
 * 1. 叶子节点: 直接删除
 * 2. 只有一个子节点: 用子节点替换
 * 3. 有两个子节点: 用后继(或前驱)替换，然后删除后继
 */
TreeNode* bst_delete(TreeNode* root, int key) {
    if (root == nullptr) return nullptr;

    if (key < root->data) {
        root->left = bst_delete(root->left, key);
    } else if (key > root->data) {
        root->right = bst_delete(root->right, key);
    } else {
        // 找到要删除的节点
        if (root->left == nullptr) {
            TreeNode* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == nullptr) {
            TreeNode* temp = root->left;
            free(root);
            return temp;
        }

        // 有两个子节点: 找后继(右子树最小)
        TreeNode* successor = bst_find_min(root->right);
        root->data = successor->data;
        root->right = bst_delete(root->right, successor->data);
    }

    return root;
}
