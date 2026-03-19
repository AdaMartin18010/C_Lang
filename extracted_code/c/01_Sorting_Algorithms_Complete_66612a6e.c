/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\01_Sorting_Algorithms_Complete.md
 * Line: 1947
 * Language: c
 * Block ID: 66612a6e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 桶排序
 * 文件名: bucket_sort.c
 * 编译: gcc -std=c23 -o bucket_sort bucket_sort.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// 桶节点定义
typedef struct Node {
    double value;
    struct Node *next;
} Node;

/**
 * @brief 向桶中插入元素（保持有序）
 */
void bucket_insert(Node **bucket, double value) {
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) return;

    new_node->value = value;
    new_node->next = NULL;

    // 空桶或插入头部
    if (*bucket == NULL || (*bucket)->value > value) {
        new_node->next = *bucket;
        *bucket = new_node;
        return;
    }

    // 找到插入位置
    Node *current = *bucket;
    while (current->next != NULL && current->next->value < value) {
        current = current->next;
    }

    new_node->next = current->next;
    current->next = new_node;
}

/**
 * @brief 基础桶排序 - 用于浮点数
 */
void bucket_sort_float(double arr[], size_t n) {
    if (n <= 1) return;

    // 创建n个桶
    Node **buckets = calloc(n, sizeof(Node *));
    if (!buckets) return;

    // 将元素放入桶中
    for (size_t i = 0; i < n; i++) {
        int bucket_idx = (int)(arr[i] * n);
        if (bucket_idx >= (int)n) bucket_idx = (int)n - 1;
        bucket_insert(&buckets[bucket_idx], arr[i]);
    }

    // 收集结果
    size_t idx = 0;
    for (size_t i = 0; i < n; i++) {
        Node *current = buckets[i];
        while (current != NULL) {
            arr[idx++] = current->value;
            Node *temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(buckets);
}

/**
 * @brief 整数桶排序
 */
void bucket_sort_int(int arr[], size_t n, int min_val, int max_val) {
    if (n <= 1 || min_val == max_val) return;

    // 桶的数量
    int num_buckets = (int)(n / 2);
    if (num_buckets < 2) num_buckets = 2;

    Node **buckets = calloc(num_buckets, sizeof(Node *));
    if (!buckets) return;

    double range = max_val - min_val + 1.0;

    // 放入桶中
    for (size_t i = 0; i < n; i++) {
        int bucket_idx = (int)(((arr[i] - min_val) / range) * num_buckets);
        if (bucket_idx >= num_buckets) bucket_idx = num_buckets - 1;
        if (bucket_idx < 0) bucket_idx = 0;

        // 使用插入排序方式插入
        Node *new_node = malloc(sizeof(Node));
        new_node->value = arr[i];
        new_node->next = NULL;

        if (buckets[bucket_idx] == NULL ||
            (int)buckets[bucket_idx]->value > arr[i]) {
            new_node->next = buckets[bucket_idx];
            buckets[bucket_idx] = new_node;
        } else {
            Node *current = buckets[bucket_idx];
            while (current->next != NULL &&
                   (int)current->next->value < arr[i]) {
                current = current->next;
            }
            new_node->next = current->next;
            current->next = new_node;
        }
    }

    // 收集
    size_t idx = 0;
    for (int i = 0; i < num_buckets; i++) {
        Node *current = buckets[i];
        while (current != NULL) {
            arr[idx++] = (int)current->value;
            Node *temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(buckets);
}

/**
 * @brief 优化桶排序 - 使用动态数组代替链表
 */
typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} Bucket;

void bucket_append(Bucket *bucket, int value) {
    if (bucket->size >= bucket->capacity) {
        bucket->capacity = (bucket->capacity == 0) ? 4 : bucket->capacity * 2;
        bucket->data = realloc(bucket->data, bucket->capacity * sizeof(int));
    }
    bucket->data[bucket->size++] = value;
}

// 插入排序桶内元素
void insertion_sort_bucket(int arr[], size_t n) {
    for (size_t i = 1; i < n; i++) {
        int key = arr[i];
        int j = (int)i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void bucket_sort_optimized(int arr[], size_t n) {
    if (n <= 1) return;

    // 找范围
    int min_val = arr[0], max_val = arr[0];
    for (size_t i = 1; i < n; i++) {
        if (arr[i] < min_val) min_val = arr[i];
        if (arr[i] > max_val) max_val = arr[i];
    }

    int num_buckets = (int)(n / 4);
    if (num_buckets < 2) num_buckets = 2;

    Bucket *buckets = calloc(num_buckets, sizeof(Bucket));
    if (!buckets) return;

    double range = max_val - min_val + 1.0;

    // 分发到桶
    for (size_t i = 0; i < n; i++) {
        int idx = (int)(((arr[i] - min_val) / range) * num_buckets);
        if (idx >= num_buckets) idx = num_buckets - 1;
        bucket_append(&buckets[idx], arr[i]);
    }

    // 排序每个桶并收集
    size_t idx = 0;
    for (int i = 0; i < num_buckets; i++) {
        if (buckets[i].size > 0) {
            insertion_sort_bucket(buckets[i].data, buckets[i].size);
            for (size_t j = 0; j < buckets[i].size; j++) {
                arr[idx++] = buckets[i].data[j];
            }
            free(buckets[i].data);
        }
    }

    free(buckets);
}

void print_float_array(const double arr[], size_t n) {
    printf("[");
    for (size_t i = 0; i < n; i++) {
        printf("%.2f", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

void print_int_array(const int arr[], size_t n) {
    printf("[");
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

int main(void) {
    double arr1[] = {0.78, 0.17, 0.39, 0.26, 0.72, 0.94, 0.21, 0.12, 0.23, 0.68};
    int arr2[] = {64, 34, 25, 12, 22, 11, 90, 5, 77, 30};
    size_t n1 = sizeof(arr1) / sizeof(arr1[0]);
    size_t n2 = sizeof(arr2) / sizeof(arr2[0]);

    printf("=== 桶排序演示 ===\n\n");

    printf("浮点数桶排序:\n");
    printf("原始: ");
    print_float_array(arr1, n1);
    bucket_sort_float(arr1, n1);
    printf("排序: ");
    print_float_array(arr1, n1);

    printf("\n整数桶排序:\n");
    printf("原始: ");
    print_int_array(arr2, n2);
    bucket_sort_optimized(arr2, n2);
    printf("排序: ");
    print_int_array(arr2, n2);

    return 0;
}
