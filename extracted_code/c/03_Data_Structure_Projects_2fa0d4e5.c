/*
 * Auto-generated from: 12_Practice_Exercises\03_Data_Structure_Projects.md
 * Line: 183
 * Language: c
 * Block ID: 2fa0d4e5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct AVLNode {
    int key;
    void *value;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;

AVLNode* avl_insert(AVLNode *node, int key, void *value);
AVLNode* avl_remove(AVLNode *node, int key);
AVLNode* avl_search(AVLNode *node, int key);
int avl_height(AVLNode *node);
int avl_balance_factor(AVLNode *node);

// 旋转操作
AVLNode* avl_right_rotate(AVLNode *y);
AVLNode* avl_left_rotate(AVLNode *x);
