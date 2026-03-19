/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 863
 * Language: c
 * Block ID: dcc3a4e9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file cache_prefetching.c
 * @brief 软件预取优化技术
 *
 * 硬件预取：现代CPU自动识别顺序访问模式
 * 软件预取：程序员显式插入预取指令，适用于非线性访问
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <x86intrin.h>

#define ARRAY_SIZE (16 * 1024 * 1024)  /* 16MB - 超出L3 */
#define PREFETCH_DISTANCE 64            /* 预取距离（元素数） */

/* 普通求和 - 依赖硬件预取 */
long long sum_naive(const int *arr, int n) {
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

/* 软件预取版本 */
long long sum_prefetch(const int *arr, int n) {
    long long sum = 0;

    /* 预热前几个元素 */
    for (int i = 0; i < PREFETCH_DISTANCE && i < n; i++) {
        sum += arr[i];
    }

    /* 主循环：访问当前，预取未来 */
    for (int i = PREFETCH_DISTANCE; i < n; i++) {
        /* _mm_prefetch: 提示CPU预取指定地址到缓存
         * _MM_HINT_T0: 预取到L1
         * _MM_HINT_T1: 预取到L2
         * _MM_HINT_T2: 预取到L3
         * _MM_HINT_NTA: 非临时访问（流式）
         */
        _mm_prefetch((const char*)&arr[i], _MM_HINT_T0);
        sum += arr[i - PREFETCH_DISTANCE];
    }

    /* 处理剩余元素 */
    for (int i = n - PREFETCH_DISTANCE; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}

/* 非线性访问 - 链表遍历 */
typedef struct Node {
    struct Node *next;
    int data;
    char padding[64 - sizeof(void*) - sizeof(int)];  /* 对齐到64字节 */
} Node;

/* 创建随机链表 */
Node* create_random_list(int n) {
    Node *nodes = aligned_alloc(64, n * sizeof(Node));
    if (!nodes) return NULL;

    /* 初始化数组 */
    for (int i = 0; i < n; i++) {
        nodes[i].data = i;
        nodes[i].next = NULL;
    }

    /* 随机打乱并链接 */
    srand(42);
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        /* 交换 */
        Node tmp = nodes[i];
        nodes[i] = nodes[j];
        nodes[j] = tmp;
    }

    /* 建立链表 */
    for (int i = 0; i < n - 1; i++) {
        nodes[i].next = &nodes[i + 1];
    }
    nodes[n - 1].next = NULL;

    return nodes;
}

/* 普通链表遍历 */
long long list_sum_naive(Node *head) {
    long long sum = 0;
    while (head) {
        sum += head->data;
        head = head->next;
    }
    return sum;
}

/* 预取优化链表遍历 */
long long list_sum_prefetch(Node *head) {
    long long sum = 0;
    Node *curr = head;
    Node *prefetch_ptr = head;

    /* 提前移动预取指针 */
    for (int i = 0; i < 8 && prefetch_ptr; i++) {
        prefetch_ptr = prefetch_ptr->next;
    }

    while (curr) {
        /* 预取8个节点之后的节点 */
        if (prefetch_ptr) {
            _mm_prefetch((const char*)prefetch_ptr, _MM_HINT_T0);
            prefetch_ptr = prefetch_ptr->next;
        }

        sum += curr->data;
        curr = curr->next;
    }

    return sum;
}

/* 矩阵转置 - 缓存不友好访问模式 */
void transpose_naive(const float *src, float *dst, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dst[j * n + i] = src[i * n + j];  /* 列访问不连续 */
        }
    }
}

/* 分块转置 + 预取 */
#define BLOCK_SIZE 64
void transpose_blocked_prefetch(const float *src, float *dst, int n) {
    for (int ii = 0; ii < n; ii += BLOCK_SIZE) {
        for (int jj = 0; jj < n; jj += BLOCK_SIZE) {
            /* 处理一个块 */
            for (int i = ii; i < ii + BLOCK_SIZE && i < n; i++) {
                /* 预取下一行 */
                if (i + 1 < ii + BLOCK_SIZE && i + 1 < n) {
                    _mm_prefetch((const char*)&src[(i + 1) * n + jj],
                                _MM_HINT_T0);
                }

                for (int j = jj; j < jj + BLOCK_SIZE && j < n; j++) {
                    dst[j * n + i] = src[i * n + j];
                }
            }
        }
    }
}

void benchmark_prefetch(void) {
    printf("=== Cache Prefetching Benchmark ===\n\n");

    /* 测试1: 数组求和 */
    int *arr = aligned_alloc(64, ARRAY_SIZE * sizeof(int));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = i;
    }

    printf("Array sum (16M elements):\n");

    /* 预热 */
    volatile long long s = sum_naive(arr, ARRAY_SIZE);
    (void)s;

    clock_t start = clock();
    volatile long long sum1 = sum_naive(arr, ARRAY_SIZE);
    clock_t end = clock();
    printf("  Naive: %ld ms (sum=%lld)\n",
           (end - start) * 1000 / CLOCKS_PER_SEC, sum1);

    start = clock();
    volatile long long sum2 = sum_prefetch(arr, ARRAY_SIZE);
    end = clock();
    printf("  Prefetch: %ld ms (sum=%lld)\n",
           (end - start) * 1000 / CLOCKS_PER_SEC, sum2);

    /* 测试2: 链表遍历 */
    printf("\nLinked list traversal (1M nodes):\n");
    Node *list = create_random_list(1000000);

    start = clock();
    volatile long long ls1 = list_sum_naive(list);
    end = clock();
    printf("  Naive: %ld ms\n", (end - start) * 1000 / CLOCKS_PER_SEC);

    start = clock();
    volatile long long ls2 = list_sum_prefetch(list);
    end = clock();
    printf("  Prefetch: %ld ms\n", (end - start) * 1000 / CLOCKS_PER_SEC);

    /* 测试3: 矩阵转置 */
    printf("\nMatrix transpose (4096x4096):\n");
    int n = 4096;
    float *m1 = aligned_alloc(64, n * n * sizeof(float));
    float *m2 = aligned_alloc(64, n * n * sizeof(float));

    start = clock();
    transpose_naive(m1, m2, n);
    end = clock();
    printf("  Naive: %ld ms\n", (end - start) * 1000 / CLOCKS_PER_SEC);

    start = clock();
    transpose_blocked_prefetch(m1, m2, n);
    end = clock();
    printf("  Blocked+Prefetch: %ld ms\n", (end - start) * 1000 / CLOCKS_PER_SEC);

    free(arr);
    free(list);
    free(m1);
    free(m2);
}

int main(void) {
    benchmark_prefetch();
    return 0;
}
