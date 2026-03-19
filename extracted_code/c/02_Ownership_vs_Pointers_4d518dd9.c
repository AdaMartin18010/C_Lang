/*
 * Auto-generated from: 15_Memory_Safe_Languages\01_Rust_vs_C\02_Ownership_vs_Pointers.md
 * Line: 516
 * Language: c
 * Block ID: 4d518dd9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C树实现：父子双向引用导致复杂性
typedef struct TreeNode {
    int value;
    struct TreeNode* parent;    // 双向引用
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// 删除节点的复杂性：需要处理多个指针
void tree_remove(TreeNode* root, int value) {
    // 实现复杂：需要处理parent、left、right的多种组合
    // 容易遗漏更新parent指针，导致树结构损坏
}

// 内存管理：需要遍历整棵树才能安全释放
void tree_free(TreeNode* node) {
    if (node) {
        tree_free(node->left);
        tree_free(node->right);
        free(node);
    }
}
