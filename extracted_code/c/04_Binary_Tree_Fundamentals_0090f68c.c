/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 454
 * Language: c
 * Block ID: 0090f68c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 查找后继 (中序遍历的下一个)
TreeNode* bst_successor(TreeNode* root, int key) {
    TreeNode* current = bst_search(root, key);
    if (current == nullptr) return nullptr;

    // 有右子树: 右子树最小值
    if (current->right != nullptr) {
        return bst_find_min(current->right);
    }

    // 无右子树: 向上找第一个比key大的祖先
    TreeNode* successor = nullptr;
    TreeNode* ancestor = root;

    while (ancestor != current) {
        if (key < ancestor->data) {
            successor = ancestor;
            ancestor = ancestor->left;
        } else {
            ancestor = ancestor->right;
        }
    }

    return successor;
}

// 查找前驱 (中序遍历的前一个)
TreeNode* bst_predecessor(TreeNode* root, int key) {
    TreeNode* current = bst_search(root, key);
    if (current == nullptr) return nullptr;

    if (current->left != nullptr) {
        return bst_find_max(current->left);
    }

    TreeNode* predecessor = nullptr;
    TreeNode* ancestor = root;

    while (ancestor != current) {
        if (key > ancestor->data) {
            predecessor = ancestor;
            ancestor = ancestor->right;
        } else {
            ancestor = ancestor->left;
        }
    }

    return predecessor;
}
