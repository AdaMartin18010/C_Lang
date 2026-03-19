/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 847
 * Language: c
 * Block ID: b2c0ea64
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 前序遍历 - 迭代 (使用栈)
 */
void preorder_iterative(TreeNode* root) {
    if (root == nullptr) return;

    TreeNode** stack = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    int top = -1;
    stack[++top] = root;

    while (top >= 0) {
        TreeNode* node = stack[top--];
        printf("%d ", node->data);

        // 先右后左 (栈是LIFO)
        if (node->right) stack[++top] = node->right;
        if (node->left) stack[++top] = node->left;
    }

    free(stack);
}

/**
 * 中序遍历 - 迭代 (使用栈)
 */
void inorder_iterative(TreeNode* root) {
    TreeNode** stack = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    int top = -1;
    TreeNode* current = root;

    while (current != nullptr || top >= 0) {
        // 走到最左
        while (current != nullptr) {
            stack[++top] = current;
            current = current->left;
        }

        current = stack[top--];
        printf("%d ", current->data);
        current = current->right;
    }

    free(stack);
}

/**
 * 后序遍历 - 迭代 (使用栈)
 * 技巧: 根右左遍历，然后反转结果
 */
void postorder_iterative(TreeNode* root) {
    if (root == nullptr) return;

    TreeNode** stack1 = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    TreeNode** stack2 = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    int top1 = -1, top2 = -1;

    stack1[++top1] = root;

    while (top1 >= 0) {
        TreeNode* node = stack1[top1--];
        stack2[++top2] = node;

        if (node->left) stack1[++top1] = node->left;
        if (node->right) stack1[++top1] = node->right;
    }

    while (top2 >= 0) {
        printf("%d ", stack2[top2--]->data);
    }

    free(stack1);
    free(stack2);
}
