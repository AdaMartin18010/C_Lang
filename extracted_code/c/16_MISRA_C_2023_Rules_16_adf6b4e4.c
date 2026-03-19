/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\16_MISRA_C_2023_Rules_16.md
 * Line: 189
 * Language: c
 * Block ID: adf6b4e4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 受控递归 - 有深度限制 */
#define MAX_RECURSION_DEPTH 100

static int recursive_helper(Node *node, int depth, int *result)
{
    if (node == NULL) {
        return 0;
    }
    
    if (depth > MAX_RECURSION_DEPTH) {
        return ERROR_DEPTH_EXCEEDED;
    }
    
    int left_result, right_result;
    int err;
    
    err = recursive_helper(node->left, depth + 1, &left_result);
    if (err != 0) return err;
    
    err = recursive_helper(node->right, depth + 1, &right_result);
    if (err != 0) return err;
    
    *result = combine(left_result, right_result, node->value);
    return 0;
}

int process_tree(Node *root, int *result)
{
    return recursive_helper(root, 0, result);
}

/* ✅ 建议 - 迭代替代递归 */
int factorial_iterative(int n)
{
    if (n < 0) return ERROR;
    
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}
