/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 812
 * Language: c
 * Block ID: 214125ad
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 前序遍历 (Pre-order): 根 -> 左 -> 右
 */
void preorder_recursive(TreeNode* root) {
    if (root == nullptr) return;
    printf("%d ", root->data);
    preorder_recursive(root->left);
    preorder_recursive(root->right);
}

/**
 * 中序遍历 (In-order): 左 -> 根 -> 右
 * BST中序遍历结果是有序的
 */
void inorder_recursive(TreeNode* root) {
    if (root == nullptr) return;
    inorder_recursive(root->left);
    printf("%d ", root->data);
    inorder_recursive(root->right);
}

/**
 * 后序遍历 (Post-order): 左 -> 右 -> 根
 */
void postorder_recursive(TreeNode* root) {
    if (root == nullptr) return;
    postorder_recursive(root->left);
    postorder_recursive(root->right);
    printf("%d ", root->data);
}
