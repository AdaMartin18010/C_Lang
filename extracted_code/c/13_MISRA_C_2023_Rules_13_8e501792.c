/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\13_MISRA_C_2023_Rules_13.md
 * Line: 252
 * Language: c
 * Block ID: 8e501792
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 受控递归 - 有深度限制 */
#define MAX_RECURSION_DEPTH 100

static int recursive_helper(Node *node, int depth)
{
    if (node == NULL) {
        return 0;
    }

    if (depth > MAX_RECURSION_DEPTH) {
        return ERROR_DEPTH_EXCEEDED;
    }

    int left = recursive_helper(node->left, depth + 1);
    int right = recursive_helper(node->right, depth + 1);

    return left + right + node->value;
}

int process_tree(Node *root)
{
    return recursive_helper(root, 0);
}

/* ✅ 尾递归优化（编译器支持时）*/
int factorial(int n, int acc)
{
    if (n <= 1) {
        return acc;
    }
    return factorial(n - 1, n * acc);
}

/* ✅ 建议：迭代替代递归 */
int factorial_iterative(int n)
{
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}
