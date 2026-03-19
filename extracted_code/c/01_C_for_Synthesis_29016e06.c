/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 157
 * Language: c
 * Block ID: 29016e06
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 不可综合：递归函数
int factorial_recursive(int n) {
    if (n <= 1) return 1;
    return n * factorial_recursive(n - 1);  // 递归调用
}

// ✅ 可综合：迭代实现
int factorial_iterative(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        #pragma HLS PIPELINE
        result *= i;
    }
    return result;
}

// ✅ 对于树形递归，展开为迭代或使用显式栈
#define MAX_DEPTH 32
int tree_sum_iterative(node_t *root) {
    node_t *stack[MAX_DEPTH];
    int sp = 0;  // 栈指针
    int sum = 0;

    stack[sp++] = root;
    while (sp > 0) {
        #pragma HLS PIPELINE
        node_t *node = stack[--sp];
        sum += node->value;
        if (node->left) stack[sp++] = node->left;
        if (node->right) stack[sp++] = node->right;
    }
    return sum;
}
