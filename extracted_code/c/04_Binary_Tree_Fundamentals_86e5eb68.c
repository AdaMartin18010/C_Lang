/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 180
 * Language: c
 * Block ID: 86e5eb68
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 二叉树节点定义
 */
typedef struct TreeNode {
    int data;                    // 节点数据
    struct TreeNode* left;       // 左子树
    struct TreeNode* right;      // 右子树
    // 可选字段
    int height;                  // 节点高度 (AVL树需要)
    char color;                  // 节点颜色 (红黑树需要)
} TreeNode;

/**
 * 二叉树结构
 */
typedef struct {
    TreeNode* root;
    size_t size;
} BinaryTree;
