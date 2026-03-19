/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 925
 * Language: c
 * Block ID: c16351f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * Morris中序遍历
 * 利用叶子节点的空指针作为临时线索
 * 时间: O(n)，空间: O(1)
 */
void morris_inorder(TreeNode* root) {
    TreeNode* current = root;

    while (current != nullptr) {
        if (current->left == nullptr) {
            printf("%d ", current->data);
            current = current->right;
        } else {
            // 找前驱
            TreeNode* predecessor = current->left;
            while (predecessor->right != nullptr && predecessor->right != current) {
                predecessor = predecessor->right;
            }

            if (predecessor->right == nullptr) {
                // 建立线索
                predecessor->right = current;
                current = current->left;
            } else {
                // 线索已存在，删除线索
                predecessor->right = nullptr;
                printf("%d ", current->data);
                current = current->right;
            }
        }
    }
}
