/*
 * Auto-generated from: 12_Practice_Exercises\03_Data_Structure_Projects.md
 * Line: 369
 * Language: c
 * Block ID: 71bf3e6b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define B_TREE_MIN_DEGREE 3  // t
#define B_TREE_MAX_KEYS (2 * B_TREE_MIN_DEGREE - 1)
#define B_TREE_MAX_CHILDREN (2 * B_TREE_MIN_DEGREE)

typedef struct BTreeNode {
    int keys[B_TREE_MAX_KEYS];
    struct BTreeNode *children[B_TREE_MAX_CHILDREN];
    int num_keys;
    bool is_leaf;
} BTreeNode;

typedef struct {
    BTreeNode *root;
    int t;  // 最小度数
} BTree;

BTree* btree_create(int t);
void btree_destroy(BTree *tree);

void btree_insert(BTree *tree, int key);
void btree_remove(BTree *tree, int key);
BTreeNode* btree_search(BTreeNode *node, int key, int *index);
void btree_traverse(BTreeNode *node, void (*visit)(int));
