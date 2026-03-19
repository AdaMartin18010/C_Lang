/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 1333
 * Language: c
 * Block ID: 9960c2ad
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 二叉树的直径
 * 直径 = 任意两个节点之间的最长路径的边数
 */
int tree_diameter(TreeNode* root, int* max_diameter) {
    if (root == nullptr) return 0;

    int left_height = tree_diameter(root->left, max_diameter);
    int right_height = tree_diameter(root->right, max_diameter);

    // 经过当前节点的直径 = 左高度 + 右高度
    int diameter_through_root = left_height + right_height;
    if (diameter_through_root > *max_diameter) {
        *max_diameter = diameter_through_root;
    }

    // 返回当前节点的高度
    return 1 + (left_height > right_height ? left_height : right_height);
}

int get_tree_diameter(TreeNode* root) {
    int max_diameter = 0;
    tree_diameter(root, &max_diameter);
    return max_diameter;
}
