/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 204
 * Language: c
 * Block ID: 207c540a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 创建节点
[[nodiscard]]
TreeNode* tree_node_create(int data) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return nullptr;
    }
    node->data = data;
    node->left = nullptr;
    node->right = nullptr;
    node->height = 1;
    node->color = 'R';  // 红黑树: 新节点默认为红色
    return node;
}

// 销毁树
void tree_destroy(TreeNode* root) {
    if (root == nullptr) return;
    tree_destroy(root->left);
    tree_destroy(root->right);
    free(root);
}

// 获取树高度
int tree_height(const TreeNode* root) {
    if (root == nullptr) return 0;
    int left_h = tree_height(root->left);
    int right_h = tree_height(root->right);
    return 1 + (left_h > right_h ? left_h : right_h);
}

// 获取节点数
int tree_size(const TreeNode* root) {
    if (root == nullptr) return 0;
    return 1 + tree_size(root->left) + tree_size(root->right);
}

// 获取叶子节点数
int tree_leaf_count(const TreeNode* root) {
    if (root == nullptr) return 0;
    if (root->left == nullptr && root->right == nullptr) return 1;
    return tree_leaf_count(root->left) + tree_leaf_count(root->right);
}
