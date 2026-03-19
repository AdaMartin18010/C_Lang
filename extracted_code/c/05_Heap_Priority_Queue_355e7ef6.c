/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 1422
 * Language: c
 * Block ID: 355e7ef6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 使用稀疏表(Sparse Table)实现RMQ
 * 预处理O(n log n)，查询O(1)
 */

typedef struct {
    int** st;       // 稀疏表
    int* log_table;
    int n;
    int k;          // 最大幂次
} SparseTableRMQ;

void rmq_init(SparseTableRMQ* rmq, const int* arr, int n) {
    rmq->n = n;
    rmq->k = 0;
    while ((1 << rmq->k) <= n) rmq->k++;

    // 预计算log值
    rmq->log_table = (int*)malloc((n + 1) * sizeof(int));
    rmq->log_table[1] = 0;
    for (int i = 2; i <= n; i++) {
        rmq->log_table[i] = rmq->log_table[i / 2] + 1;
    }

    // 构建稀疏表
    rmq->st = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        rmq->st[i] = (int*)malloc(rmq->k * sizeof(int));
        rmq->st[i][0] = arr[i];
    }

    for (int j = 1; j < rmq->k; j++) {
        for (int i = 0; i + (1 << j) <= n; i++) {
            rmq->st[i][j] = (rmq->st[i][j-1] < rmq->st[i + (1 << (j-1))][j-1]) ?
                            rmq->st[i][j-1] : rmq->st[i + (1 << (j-1))][j-1];
        }
    }
}

// 查询[l, r]区间的最小值
int rmq_query(const SparseTableRMQ* rmq, int l, int r) {
    int j = rmq->log_table[r - l + 1];
    int a = rmq->st[l][j];
    int b = rmq->st[r - (1 << j) + 1][j];
    return (a < b) ? a : b;
}
