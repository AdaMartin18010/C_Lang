/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 253
 * Language: c
 * Block ID: 224d3a83
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 判断是否为空树
static inline bool tree_is_empty(const BinaryTree* tree) {
    return tree->root == nullptr;
}

// 判断是否为满二叉树
bool tree_is_full(const TreeNode* root) {
    if (root == nullptr) return true;
    if (root->left == nullptr && root->right == nullptr) return true;
    if (root->left != nullptr && root->right != nullptr) {
        return tree_is_full(root->left) && tree_is_full(root->right);
    }
    return false;
}

// 判断是否完全二叉树 (层序遍历法)
bool tree_is_complete(const TreeNode* root) {
    if (root == nullptr) return true;

    // 使用队列进行层序遍历
    TreeNode** queue = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    int front = 0, rear = 0;

    queue[rear++] = root;
    bool must_be_leaf = false;

    while (front < rear) {
        TreeNode* node = queue[front++];

        if (must_be_leaf && (node->left || node->right)) {
            free(queue);
            return false;
        }

        if (node->left == nullptr && node->right != nullptr) {
            free(queue);
            return false;
        }

        if (node->left) queue[rear++] = node->left;
        if (node->right) queue[rear++] = node->right;
        else must_be_leaf = true;
    }

    free(queue);
    return true;
}

// 判断是否为平衡二叉树
bool tree_is_balanced(const TreeNode* root) {
    if (root == nullptr) return true;

    int left_h = tree_height(root->left);
    int right_h = tree_height(root->right);

    if (abs(left_h - right_h) > 1) return false;

    return tree_is_balanced(root->left) && tree_is_balanced(root->right);
}
