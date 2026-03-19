/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 342
 * Language: c
 * Block ID: c90c59ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * BST 查找
 * 时间复杂度: O(h)，h为树高，平衡时O(log n)
 */
TreeNode* bst_search(TreeNode* root, int key) {
    if (root == nullptr || root->data == key) {
        return root;
    }

    if (key < root->data) {
        return bst_search(root->left, key);
    } else {
        return bst_search(root->right, key);
    }
}

// 迭代版本
TreeNode* bst_search_iter(TreeNode* root, int key) {
    while (root != nullptr && root->data != key) {
        if (key < root->data) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    return root;
}

// 查找最小值
TreeNode* bst_find_min(TreeNode* root) {
    if (root == nullptr) return nullptr;
    while (root->left != nullptr) {
        root = root->left;
    }
    return root;
}

// 查找最大值
TreeNode* bst_find_max(TreeNode* root) {
    if (root == nullptr) return nullptr;
    while (root->right != nullptr) {
        root = root->right;
    }
    return root;
}
