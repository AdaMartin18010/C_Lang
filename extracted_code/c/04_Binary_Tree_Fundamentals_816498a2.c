/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 1290
 * Language: c
 * Block ID: 816498a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 二叉树的最近公共祖先
 *
 * 性质:
 * - 如果p和q分别在root的左右子树，则root是LCA
 * - 如果都在左子树，则LCA在左子树
 * - 如果都在右子树，则LCA在右子树
 */
TreeNode* lowest_common_ancestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (root == nullptr || root == p || root == q) {
        return root;
    }

    TreeNode* left = lowest_common_ancestor(root->left, p, q);
    TreeNode* right = lowest_common_ancestor(root->right, p, q);

    // 左右子树各找到一个，当前节点就是LCA
    if (left != nullptr && right != nullptr) {
        return root;
    }

    // 只在一边找到
    return left != nullptr ? left : right;
}

// BST的LCA (利用BST性质)
TreeNode* bst_lowest_common_ancestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    while (root != nullptr) {
        if (p->data < root->data && q->data < root->data) {
            root = root->left;
        } else if (p->data > root->data && q->data > root->data) {
            root = root->right;
        } else {
            return root;
        }
    }
    return nullptr;
}
