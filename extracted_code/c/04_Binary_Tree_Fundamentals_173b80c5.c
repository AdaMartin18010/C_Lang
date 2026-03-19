/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 962
 * Language: c
 * Block ID: 173b80c5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 层序遍历 (Level-order)
 * 使用队列
 */
void level_order(TreeNode* root) {
    if (root == nullptr) return;

    TreeNode** queue = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    int front = 0, rear = 0;

    queue[rear++] = root;

    while (front < rear) {
        TreeNode* node = queue[front++];
        printf("%d ", node->data);

        if (node->left) queue[rear++] = node->left;
        if (node->right) queue[rear++] = node->right;
    }

    free(queue);
}

/**
 * 层序遍历 - 按层打印
 */
void level_order_by_level(TreeNode* root) {
    if (root == nullptr) return;

    TreeNode** queue = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    int front = 0, rear = 0;

    queue[rear++] = root;
    int level = 0;

    while (front < rear) {
        int level_size = rear - front;
        printf("Level %d: ", level++);

        for (int i = 0; i < level_size; i++) {
            TreeNode* node = queue[front++];
            printf("%d ", node->data);

            if (node->left) queue[rear++] = node->left;
            if (node->right) queue[rear++] = node->right;
        }
        printf("\n");
    }

    free(queue);
}
