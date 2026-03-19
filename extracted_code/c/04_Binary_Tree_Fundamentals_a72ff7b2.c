/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 1363
 * Language: c
 * Block ID: a72ff7b2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 二叉树中的最大路径和
 * 路径至少包含一个节点
 */
int max_path_sum(TreeNode* root, int* max_sum) {
    if (root == nullptr) return 0;

    // 只取正值的贡献
    int left_gain = max_path_sum(root->left, max_sum);
    int right_gain = max_path_sum(root->right, max_sum);

    left_gain = left_gain > 0 ? left_gain : 0;
    right_gain = right_gain > 0 ? right_gain : 0;

    // 经过当前节点的路径和
    int price_newpath = root->data + left_gain + right_gain;
    if (price_newpath > *max_sum) {
        *max_sum = price_newpath;
    }

    // 返回以当前节点为端点的最大路径和
    return root->data + (left_gain > right_gain ? left_gain : right_gain);
}
