/*
 * Auto-generated from: 12_Practice_Exercises\03_Data_Structure_Projects.md
 * Line: 166
 * Language: c
 * Block ID: e3287a93
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct TreeNode {
    int key;
    void *value;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

TreeNode* bst_insert(TreeNode *root, int key, void *value);
TreeNode* bst_search(TreeNode *root, int key);
TreeNode* bst_remove(TreeNode *root, int key);
void bst_inorder(TreeNode *root, void (*visit)(TreeNode*));
void bst_destroy(TreeNode *root);
