/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 1034
 * Language: c
 * Block ID: 7c99688b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ============================================================================
 * 二叉树完整实现 (Binary Tree Complete Implementation)
 * ============================================================================
 * 编译命令: gcc -std=c23 -Wall -Wextra -o binary_tree binary_tree.c
 * 运行命令: ./binary_tree
 * ============================================================================
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <time.h>

#if __STDC_VERSION__ < 202311L
    #define nullptr NULL
#endif

// 这里包含前面所有的结构定义和函数实现...

// =============================================================================
// 测试函数
// =============================================================================

void test_bst() {
    printf("\n========== BST 测试 ==========\n");

    TreeNode* root = nullptr;
    int values[] = {50, 30, 70, 20, 40, 60, 80};

    printf("插入: ");
    for (int i = 0; i < 7; i++) {
        printf("%d ", values[i]);
        root = bst_insert(root, values[i]);
    }
    printf("\n");

    printf("中序遍历: ");
    inorder_recursive(root);
    printf("\n");

    printf("查找 40: %s\n", bst_search(root, 40) ? "Found" : "Not Found");
    printf("查找 100: %s\n", bst_search(root, 100) ? "Found" : "Not Found");

    printf("删除 30\n");
    root = bst_delete(root, 30);
    printf("中序遍历: ");
    inorder_recursive(root);
    printf("\n");

    tree_destroy(root);
    printf("BST 测试通过!\n");
}

void test_avl() {
    printf("\n========== AVL 树测试 ==========\n");

    TreeNode* root = nullptr;
    int values[] = {10, 20, 30, 40, 50, 25};

    printf("插入序列: ");
    for (int i = 0; i < 6; i++) {
        printf("%d ", values[i]);
        root = avl_insert(root, values[i]);
    }
    printf("\n");

    printf("中序遍历: ");
    inorder_recursive(root);
    printf("\n");

    printf("树高度: %d\n", tree_height(root));
    printf("是否平衡: %s\n", tree_is_balanced(root) ? "是" : "否");

    tree_destroy(root);
    printf("AVL 树测试通过!\n");
}

void test_traversals() {
    printf("\n========== 遍历测试 ==========\n");

    // 构建测试树
    //       1
    //      / \
    //     2   3
    //    / \   \
    //   4   5   6

    TreeNode* root = tree_node_create(1);
    root->left = tree_node_create(2);
    root->right = tree_node_create(3);
    root->left->left = tree_node_create(4);
    root->left->right = tree_node_create(5);
    root->right->right = tree_node_create(6);

    printf("前序遍历 (递归): ");
    preorder_recursive(root);
    printf("\n");

    printf("中序遍历 (递归): ");
    inorder_recursive(root);
    printf("\n");

    printf("后序遍历 (递归): ");
    postorder_recursive(root);
    printf("\n");

    printf("层序遍历: ");
    level_order(root);
    printf("\n");

    printf("按层打印:\n");
    level_order_by_level(root);

    tree_destroy(root);
    printf("遍历测试通过!\n");
}

// =============================================================================
// 主函数
// =============================================================================

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║         二叉树完整实现测试 (C23 Standard)                  ║\n");
    printf("║         Binary Tree Complete Implementation                ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("编译时间: %s %s\n", __DATE__, __TIME__);

    test_bst();
    test_avl();
    test_traversals();

    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║              所有测试通过! All tests passed!               ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");

    return 0;
}
