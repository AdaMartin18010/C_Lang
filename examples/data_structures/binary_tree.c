/*
 * 二叉搜索树示例（插入、查找、中序遍历、删除）
 * 编译: gcc -Wall -Wextra -o binary_tree binary_tree.c
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct TreeNode {
    int key;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

TreeNode *tree_insert(TreeNode *root, int key) {
    if (!root) {
        TreeNode *n = malloc(sizeof(TreeNode));
        n->key = key; n->left = n->right = NULL;
        return n;
    }
    if (key < root->key)
        root->left = tree_insert(root->left, key);
    else if (key > root->key)
        root->right = tree_insert(root->right, key);
    return root;
}

TreeNode *tree_find(TreeNode *root, int key) {
    while (root) {
        if (key == root->key) return root;
        root = (key < root->key) ? root->left : root->right;
    }
    return NULL;
}

TreeNode *tree_min(TreeNode *root) {
    while (root && root->left) root = root->left;
    return root;
}

TreeNode *tree_delete(TreeNode *root, int key) {
    if (!root) return NULL;
    if (key < root->key)
        root->left = tree_delete(root->left, key);
    else if (key > root->key)
        root->right = tree_delete(root->right, key);
    else {
        if (!root->left) { TreeNode *t = root->right; free(root); return t; }
        if (!root->right) { TreeNode *t = root->left; free(root); return t; }
        TreeNode *succ = tree_min(root->right);
        root->key = succ->key;
        root->right = tree_delete(root->right, succ->key);
    }
    return root;
}

void tree_inorder(const TreeNode *root) {
    if (!root) return;
    tree_inorder(root->left);
    printf("%d ", root->key);
    tree_inorder(root->right);
}

void tree_destroy(TreeNode *root) {
    if (!root) return;
    tree_destroy(root->left);
    tree_destroy(root->right);
    free(root);
}

int main(void) {
    TreeNode *root = NULL;
    int keys[] = {50, 30, 70, 20, 40, 60, 80};
    for (size_t i = 0; i < sizeof(keys)/sizeof(keys[0]); i++)
        root = tree_insert(root, keys[i]);
    
    printf("Inorder: "); tree_inorder(root); printf("\n");
    printf("Find 40: %s\n", tree_find(root, 40) ? "found" : "not found");
    printf("Find 99: %s\n", tree_find(root, 99) ? "found" : "not found");
    
    root = tree_delete(root, 50);
    printf("After delete 50: "); tree_inorder(root); printf("\n");
    
    tree_destroy(root);
    return 0;
}
