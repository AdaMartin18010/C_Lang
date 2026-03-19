/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 1391
 * Language: c
 * Block ID: 264e3754
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 计算由1到n组成的不同的BST数量
 * 卡塔兰数: C(n) = (2n)! / ((n+1)! * n!)
 * 递推: C(n) = Σ C(i-1) * C(n-i) for i in 1..n
 */
int num_trees(int n) {
    if (n <= 1) return 1;

    // dp[i] = 由i个节点组成的BST数量
    int* dp = (int*)calloc(n + 1, sizeof(int));
    dp[0] = 1;
    dp[1] = 1;

    for (int nodes = 2; nodes <= n; nodes++) {
        for (int root = 1; root <= nodes; root++) {
            int left_trees = dp[root - 1];
            int right_trees = dp[nodes - root];
            dp[nodes] += left_trees * right_trees;
        }
    }

    int result = dp[n];
    free(dp);
    return result;
}

// 生成所有不同的BST
typedef struct TreeNodeList {
    TreeNode* node;
    struct TreeNodeList* next;
} TreeNodeList;

TreeNodeList* generate_trees(int start, int end) {
    TreeNodeList* result = (TreeNodeList*)malloc(sizeof(TreeNodeList));
    result->node = nullptr;
    result->next = nullptr;
    TreeNodeList* tail = result;

    if (start > end) {
        return result;
    }

    for (int i = start; i <= end; i++) {
        TreeNodeList* left_trees = generate_trees(start, i - 1);
        TreeNodeList* right_trees = generate_trees(i + 1, end);

        for (TreeNodeList* left = left_trees; left != nullptr; left = left->next) {
            for (TreeNodeList* right = right_trees; right != nullptr; right = right->next) {
                TreeNode* root = tree_node_create(i);
                root->left = left->node;
                root->right = right->node;

                tail->next = (TreeNodeList*)malloc(sizeof(TreeNodeList));
                tail = tail->next;
                tail->node = root;
                tail->next = nullptr;
            }
        }
    }

    TreeNodeList* temp = result;
    result = result->next;
    free(temp);

    return result;
}
