/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\02_Searching_Algorithms.md
 * Line: 162
 * Language: c
 * Block ID: 6e58eb06
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 线性搜索算法集
 * 文件名: linear_search.c
 * 编译: gcc -std=c23 -o linear_search linear_search.c
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/**
 * @brief 基础线性搜索
 * @param arr 待搜索数组
 * @param n 数组长度
 * @param target 目标值
 * @return 目标值的索引，未找到返回-1
 */
int linear_search_basic(const int arr[], size_t n, int target) {
    for (size_t i = 0; i < n; i++) {
        if (arr[i] == target) {
            return (int)i;
        }
    }
    return -1;
}

/**
 * @brief 带哨兵的线性搜索 - 减少比较次数
 * @note 需要可修改的数组
 */
int linear_search_sentinel(int arr[], size_t n, int target) {
    if (n == 0) return -1;

    int last = arr[n - 1];  // 保存最后一个元素
    arr[n - 1] = target;     // 设置哨兵

    size_t i = 0;
    while (arr[i] != target) {
        i++;
    }

    arr[n - 1] = last;  // 恢复原值

    if (i < n - 1 || last == target) {
        return (int)i;
    }
    return -1;
}

/**
 * @brief 递归线性搜索
 */
int linear_search_recursive(const int arr[], size_t n, int target, size_t index) {
    if (index >= n) return -1;
    if (arr[index] == target) return (int)index;
    return linear_search_recursive(arr, n, target, index + 1);
}

/**
 * @brief 查找最后一个匹配的元素
 */
int linear_search_last(const int arr[], size_t n, int target) {
    int result = -1;
    for (size_t i = 0; i < n; i++) {
        if (arr[i] == target) {
            result = (int)i;  // 继续搜索，记录最后一个
        }
    }
    return result;
}

/**
 * @brief 查找所有匹配的元素
 * @param result 结果索引数组
 * @return 匹配数量
 */
int linear_search_all(const int arr[], size_t n, int target,
                       int result[], size_t max_results) {
    size_t count = 0;
    for (size_t i = 0; i < n && count < max_results; i++) {
        if (arr[i] == target) {
            result[count++] = (int)i;
        }
    }
    return (int)count;
}

/**
 * @brief 查找满足条件的元素（泛型）
 * @param predicate 条件函数指针
 */
typedef bool (*Predicate)(int value, void *context);

int linear_search_if(const int arr[], size_t n, Predicate pred, void *ctx) {
    for (size_t i = 0; i < n; i++) {
        if (pred(arr[i], ctx)) {
            return (int)i;
        }
    }
    return -1;
}

// 使用示例：查找第一个大于10的元素
bool greater_than_10(int value, void *ctx) {
    (void)ctx;
    return value > 10;
}

/**
 * @brief 双向搜索 - 从两端同时搜索
 */
int bidirectional_search(const int arr[], size_t n, int target) {
    size_t left = 0;
    size_t right = n;

    while (left < right) {
        if (arr[left] == target) return (int)left;
        if (arr[right - 1] == target) return (int)(right - 1);
        left++;
        right--;
    }
    return -1;
}

/**
 * @brief 块搜索/跳跃搜索 - 用于无序数据的预处理搜索
 * @note 将数据分块，先确定块再线性搜索
 */
typedef struct {
    int min_val;
    int max_val;
    size_t start;
    size_t end;
} Block;

int block_search(int arr[], size_t n, int target, Block blocks[], size_t num_blocks) {
    // 确定目标可能在哪一块
    size_t target_block = num_blocks;
    for (size_t b = 0; b < num_blocks; b++) {
        if (target >= blocks[b].min_val && target <= blocks[b].max_val) {
            target_block = b;
            break;
        }
    }

    if (target_block == num_blocks) return -1;

    // 在块内线性搜索
    Block *blk = &blocks[target_block];
    for (size_t i = blk->start; i <= blk->end && i < n; i++) {
        if (arr[i] == target) return (int)i;
    }
    return -1;
}

void print_search_result(const char *name, int index, int target) {
    printf("%s: ", name);
    if (index >= 0) {
        printf("找到 %d 在索引 %d\n", target, index);
    } else {
        printf("未找到 %d\n", target);
    }
}

int main(void) {
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 22, 5, 77};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    int target = 22;

    printf("=== 线性搜索演示 ===\n");
    printf("数组: [");
    for (size_t i = 0; i < n; i++) {
        printf("%d%s", arr[i], i < n - 1 ? ", " : "");
    }
    printf("]\n");
    printf("目标值: %d\n\n", target);

    print_search_result("基础线性搜索",
        linear_search_basic(arr, n, target), target);

    int arr_copy[10];
    memcpy(arr_copy, arr, sizeof(arr));
    print_search_result("哨兵线性搜索",
        linear_search_sentinel(arr_copy, n, target), target);

    print_search_result("递归线性搜索",
        linear_search_recursive(arr, n, target, 0), target);

    print_search_result("查找最后一个",
        linear_search_last(arr, n, target), target);

    int all_results[10];
    int count = linear_search_all(arr, n, target, all_results, 10);
    printf("查找所有匹配: 找到 %d 个，索引为 [", count);
    for (int i = 0; i < count; i++) {
        printf("%d%s", all_results[i], i < count - 1 ? ", " : "");
    }
    printf("]\n");

    print_search_result("双向搜索",
        bidirectional_search(arr, n, target), target);

    print_search_result("条件搜索(>10)",
        linear_search_if(arr, n, greater_than_10, NULL), 0);

    return 0;
}
