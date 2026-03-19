/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_Coq_Examples\01_Coq_Verification_Cases.md
 * Line: 4099
 * Language: c
 * Block ID: 917642b9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* File: c_impl/binary_search_tree.c */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

/* ============================================================ */
/* 1. BST节点定义 */
/* ============================================================ */

typedef struct BSTNode {
    int key;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

/* ============================================================ */
/* 2. 基本操作 */
/* ============================================================ */

/* 创建新节点 */
BSTNode* bst_create_node(int key) {
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* 查找 */
/* 
 * 规范: {BST(t)}
 *       bst_search(t, key)
 *       {ret = true <-> key \\in tree_keys(t)}
 */
bool bst_search(BSTNode* root, int key) {
    while (root != NULL) {
        if (key < root->key) {
            root = root->left;
        } else if (key > root->key) {
            root = root->right;
        } else {
            return true;
        }
    }
    return false;
}

/* 插入 */
/*
 * 规范: {BST(t)}
 *       bst_insert(t, key)
 *       {BST(ret) /\\ tree_keys(ret) = tree_keys(t) \\union {key}}
 */
BSTNode* bst_insert(BSTNode* root, int key) {
    if (root == NULL) {
        return bst_create_node(key);
    }
    
    if (key < root->key) {
        root->left = bst_insert(root->left, key);
    } else if (key > root->key) {
        root->right = bst_insert(root->right, key);
    }
    /* 如果key已存在，不重复插入 */
    
    return root;
}

/* 找最小值 */
/*
 * 规范: {BST(t) /\\ t \\neq null}
 *       bst_find_min(t)
 *       {ret = min(tree_keys(t))}
 */
BSTNode* bst_find_min(BSTNode* root) {
    assert(root != NULL);
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

/* 找最大值 */
/*
 * 规范: {BST(t) /\\ t \\neq null}
 *       bst_find_max(t)
 *       {ret = max(tree_keys(t))}
 */
BSTNode* bst_find_max(BSTNode* root) {
    assert(root != NULL);
    while (root->right != NULL) {
        root = root->right;
    }
    return root;
}

/* 删除 */
/*
 * 规范: {BST(t)}
 *       bst_delete(t, key)
 *       {BST(ret) /\\ tree_keys(ret) = tree_keys(t) \\setminus {key}}
 */
BSTNode* bst_delete(BSTNode* root, int key) {
    if (root == NULL) return NULL;
    
    if (key < root->key) {
        root->left = bst_delete(root->left, key);
    } else if (key > root->key) {
        root->right = bst_delete(root->right, key);
    } else {
        /* 找到要删除的节点 */
        if (root->left == NULL) {
            BSTNode* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            BSTNode* temp = root->left;
            free(root);
            return temp;
        } else {
            /* 有两个子节点：用右子树的最小值替换 */
            BSTNode* temp = bst_find_min(root->right);
            root->key = temp->key;
            root->right = bst_delete(root->right, temp->key);
        }
    }
    return root;
}

/* 中序遍历（打印排序后的结果） */
/*
 * 规范: {BST(t)}
 *       bst_inorder(t)
 *       {输出按升序排列的keys}
 */
void bst_inorder(BSTNode* root) {
    if (root != NULL) {
        bst_inorder(root->left);
        printf("%d ", root->key);
        bst_inorder(root->right);
    }
}

/* 释放树 */
/*
 * 规范: {tree(t)}
 *       bst_free(t)
 *       {emp}
 */
void bst_free(BSTNode* root) {
    if (root != NULL) {
        bst_free(root->left);
        bst_free(root->right);
        free(root);
    }
}

/* 树高度 */
/*
 * 规范: {tree(t)}
 *       bst_height(t)
 *       {ret = height(t)}
 */
int bst_height(BSTNode* root) {
    if (root == NULL) return -1;
    
    int left_height = bst_height(root->left);
    int right_height = bst_height(root->right);
    
    return 1 + (left_height > right_height ? left_height : right_height);
}

/* 节点数量 */
int bst_size(BSTNode* root) {
    if (root == NULL) return 0;
    return 1 + bst_size(root->left) + bst_size(root->right);
}

/* 验证BST性质（用于测试） */
/*
 * 规范: {tree(t)}
 *       is_valid_bst(t)
 *       {ret = true <-> BST(t)}
 */
bool is_valid_bst_helper(BSTNode* root, int min_val, int max_val) {
    if (root == NULL) return true;
    
    if (root->key <= min_val || root->key >= max_val) {
        return false;
    }
    
    return is_valid_bst_helper(root->left, min_val, root->key) &&
           is_valid_bst_helper(root->right, root->key, max_val);
}

bool is_valid_bst(BSTNode* root) {
    return is_valid_bst_helper(root, INT_MIN, INT_MAX);
}

/* ============================================================ */
/* 3. 测试函数 */
/* ============================================================ */

void test_bst_operations() {
    BSTNode* root = NULL;
    
    /* 测试插入 */
    root = bst_insert(root, 50);
    root = bst_insert(root, 30);
    root = bst_insert(root, 70);
    root = bst_insert(root, 20);
    root = bst_insert(root, 40);
    root = bst_insert(root, 60);
    root = bst_insert(root, 80);
    
    assert(is_valid_bst(root));
    assert(bst_size(root) == 7);
    
    /* 测试查找 */
    assert(bst_search(root, 40) == true);
    assert(bst_search(root, 100) == false);
    
    /* 测试最值 */
    assert(bst_find_min(root)->key == 20);
    assert(bst_find_max(root)->key == 80);
    
    /* 测试删除（叶子节点） */
    root = bst_delete(root, 20);
    assert(is_valid_bst(root));
    assert(bst_search(root, 20) == false);
    
    /* 测试删除（有一个子节点） */
    root = bst_delete(root, 30);
    assert(is_valid_bst(root));
    assert(bst_search(root, 30) == false);
    
    /* 测试删除（有两个子节点） */
    root = bst_delete(root, 50);
    assert(is_valid_bst(root));
    assert(bst_search(root, 50) == false);
    
    printf("Inorder traversal: ");
    bst_inorder(root);
    printf("\n");
    
    bst_free(root);
    printf("BST tests passed!\n");
}

/* 压力测试：大量随机操作 */
void test_bst_stress() {
    BSTNode* root = NULL;
    int keys[1000];
    int n = 0;
    
    /* 随机插入 */
    for (int i = 0; i < 1000; i++) {
        int key = rand() % 10000;
        if (!bst_search(root, key)) {
            root = bst_insert(root, key);
            keys[n++] = key;
            assert(is_valid_bst(root));
        }
    }
    
    /* 随机删除 */
    for (int i = 0; i < n / 2; i++) {
        int idx = rand() % n;
        if (keys[idx] != -1) {
            root = bst_delete(root, keys[idx]);
            keys[idx] = -1;
            assert(is_valid_bst(root));
        }
    }
    
    bst_free(root);
    printf("BST stress test passed!\n");
}
