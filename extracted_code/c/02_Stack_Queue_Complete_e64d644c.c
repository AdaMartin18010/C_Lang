/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 525
 * Language: c
 * Block ID: e64d644c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 使用栈模拟递归 - 以阶乘为例
 * 展示如何将递归算法转换为迭代实现
 */

// 递归版本
long long factorial_recursive(int n) {
    if (n <= 1) return 1;
    return n * factorial_recursive(n - 1);
}

// 迭代版本 (使用显式栈)
long long factorial_iterative(int n) {
    if (n <= 1) return 1;

    ArrayStack stack;
    array_stack_init(&stack);

    // 压入所有数字
    for (int i = n; i >= 1; i--) {
        array_stack_push(&stack, i);
    }

    long long result = 1;
    int value;
    while (!array_stack_is_empty(&stack)) {
        array_stack_pop(&stack, &value);
        result *= value;
    }

    array_stack_destroy(&stack);
    return result;
}

/**
 * 更复杂的例子: 树的先序遍历
 */
typedef struct TreeNode {
    int val;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// 递归先序遍历
void preorder_recursive(TreeNode* root) {
    if (root == nullptr) return;
    printf("%d ", root->val);
    preorder_recursive(root->left);
    preorder_recursive(root->right);
}

// 迭代先序遍历 (使用栈)
void preorder_iterative(TreeNode* root) {
    if (root == nullptr) return;

    LinkedStack stack;
    linked_stack_init(&stack);
    linked_stack_push(&stack, (int)(intptr_t)root);

    while (!linked_stack_is_empty(&stack)) {
        int node_ptr;
        linked_stack_pop(&stack, &node_ptr);
        TreeNode* node = (TreeNode*)(intptr_t)node_ptr;

        printf("%d ", node->val);

        // 先右后左 (栈是LIFO)
        if (node->right != nullptr) {
            linked_stack_push(&stack, (int)(intptr_t)node->right);
        }
        if (node->left != nullptr) {
            linked_stack_push(&stack, (int)(intptr_t)node->left);
        }
    }

    linked_stack_destroy(&stack);
}
