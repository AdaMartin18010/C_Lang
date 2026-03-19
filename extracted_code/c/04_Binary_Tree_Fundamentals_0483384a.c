/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 687
 * Language: c
 * Block ID: 0483384a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * AVL 删除
 */
TreeNode* avl_delete(TreeNode* root, int key) {
    // 1. 标准BST删除
    if (root == nullptr) return nullptr;

    if (key < root->data) {
        root->left = avl_delete(root->left, key);
    } else if (key > root->data) {
        root->right = avl_delete(root->right, key);
    } else {
        // 找到要删除的节点
        if (root->left == nullptr || root->right == nullptr) {
            TreeNode* temp = root->left ? root->left : root->right;
            if (temp == nullptr) {
                temp = root;
                root = nullptr;
            } else {
                *root = *temp;  // 复制内容
            }
            free(temp);
        } else {
            // 两个子节点: 找后继
            TreeNode* successor = bst_find_min(root->right);
            root->data = successor->data;
            root->right = avl_delete(root->right, successor->data);
        }
    }

    if (root == nullptr) return nullptr;

    // 2. 更新高度
    avl_update_height(root);

    // 3. 重新平衡 (类似插入)
    int balance = avl_balance_factor(root);

    // LL
    if (balance > 1 && avl_balance_factor(root->left) >= 0) {
        return avl_rotate_right(root);
    }

    // LR
    if (balance > 1 && avl_balance_factor(root->left) < 0) {
        root->left = avl_rotate_left(root->left);
        return avl_rotate_right(root);
    }

    // RR
    if (balance < -1 && avl_balance_factor(root->right) <= 0) {
        return avl_rotate_left(root);
    }

    // RL
    if (balance < -1 && avl_balance_factor(root->right) > 0) {
        root->right = avl_rotate_right(root->right);
        return avl_rotate_left(root);
    }

    return root;
}
