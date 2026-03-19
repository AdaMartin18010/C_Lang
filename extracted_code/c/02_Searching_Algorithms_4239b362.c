/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\02_Searching_Algorithms.md
 * Line: 429
 * Language: c
 * Block ID: 4239b362
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 二分搜索算法集
 * 文件名: binary_search.c
 * 编译: gcc -std=c23 -o binary_search binary_search.c
 */

#include <stdio.h>
#include <stdbool.h>

/**
 * @brief 基础二分搜索（迭代版）
 * @param arr 有序数组
 * @param n 数组长度
 * @param target 目标值
 * @return 目标值的索引，未找到返回-1
 */
int binary_search_basic(const int arr[], size_t n, int target) {
    size_t left = 0;
    size_t right = n;

    while (left < right) {
        size_t mid = left + (right - left) / 2;

        if (arr[mid] == target) {
            return (int)mid;
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return -1;
}

/**
 * @brief 基础二分搜索（递归版）
 */
int binary_search_recursive(const int arr[], int target,
                            size_t left, size_t right) {
    if (left >= right) return -1;

    size_t mid = left + (right - left) / 2;

    if (arr[mid] == target) {
        return (int)mid;
    } else if (arr[mid] < target) {
        return binary_search_recursive(arr, target, mid + 1, right);
    } else {
        return binary_search_recursive(arr, target, left, mid);
    }
}

/**
 * @brief 查找第一个等于目标值的元素
 * @note 处理重复元素的情况
 */
int binary_search_first(const int arr[], size_t n, int target) {
    size_t left = 0;
    size_t right = n;
    int result = -1;

    while (left < right) {
        size_t mid = left + (right - left) / 2;

        if (arr[mid] == target) {
            result = (int)mid;
            right = mid;  // 继续在左半部分查找
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return result;
}

/**
 * @brief 查找最后一个等于目标值的元素
 */
int binary_search_last(const int arr[], size_t n, int target) {
    size_t left = 0;
    size_t right = n;
    int result = -1;

    while (left < right) {
        size_t mid = left + (right - left) / 2;

        if (arr[mid] == target) {
            result = (int)mid;
            left = mid + 1;  // 继续在右半部分查找
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return result;
}

/**
 * @brief 查找第一个大于等于目标值的元素（lower_bound）
 */
int lower_bound(const int arr[], size_t n, int target) {
    size_t left = 0;
    size_t right = n;

    while (left < right) {
        size_t mid = left + (right - left) / 2;

        if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return (left < n) ? (int)left : -1;
}

/**
 * @brief 查找第一个大于目标值的元素（upper_bound）
 */
int upper_bound(const int arr[], size_t n, int target) {
    size_t left = 0;
    size_t right = n;

    while (left < right) {
        size_t mid = left + (right - left) / 2;

        if (arr[mid] <= target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return (left < n) ? (int)left : -1;
}

/**
 * @brief 二分搜索范围 - 查找目标值的所有位置
 * @return 通过指针返回起始和结束位置
 */
void binary_search_range(const int arr[], size_t n, int target,
                         int *first_idx, int *last_idx) {
    *first_idx = binary_search_first(arr, n, target);
    if (*first_idx == -1) {
        *last_idx = -1;
        return;
    }
    *last_idx = binary_search_last(arr, n, target);
}

/**
 * @brief 搜索插入位置 - 保持数组有序
 * @return 应该插入的位置
 */
int binary_search_insert_position(const int arr[], size_t n, int target) {
    size_t left = 0;
    size_t right = n;

    while (left < right) {
        size_t mid = left + (right - left) / 2;

        if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return (int)left;
}

/**
 * @brief 浮点数二分搜索（处理精度问题）
 */
int binary_search_double(const double arr[], size_t n, double target,
                          double epsilon) {
    size_t left = 0;
    size_t right = n;

    while (left < right) {
        size_t mid = left + (right - left) / 2;

        if (arr[mid] > target - epsilon && arr[mid] < target + epsilon) {
            return (int)mid;  // 在误差范围内相等
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return -1;
}

/**
 * @brief 三分搜索 - 用于单峰函数求极值
 */
double ternary_search_max(double (*f)(double), double left,
                          double right, double epsilon) {
    while (right - left > epsilon) {
        double m1 = left + (right - left) / 3;
        double m2 = right - (right - left) / 3;

        if (f(m1) < f(m2)) {
            left = m1;
        } else {
            right = m2;
        }
    }
    return (left + right) / 2;
}

// 示例函数：f(x) = -x² + 4x + 5，在x=2处取得最大值9
double example_func(double x) {
    return -x * x + 4 * x + 5;
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
    int arr[] = {5, 11, 12, 22, 22, 22, 25, 34, 64, 77, 90};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    int target = 22;
    int not_found = 30;

    printf("=== 二分搜索演示 ===\n");
    printf("有序数组: [");
    for (size_t i = 0; i < n; i++) {
        printf("%d%s", arr[i], i < n - 1 ? ", " : "");
    }
    printf("]\n\n");

    printf("目标值 %d:\n", target);
    print_search_result("  基础二分搜索",
        binary_search_basic(arr, n, target), target);
    print_search_result("  递归二分搜索",
        binary_search_recursive(arr, target, 0, n), target);
    print_search_result("  查找第一个",
        binary_search_first(arr, n, target), target);
    print_search_result("  查找最后一个",
        binary_search_last(arr, n, target), target);

    int first, last;
    binary_search_range(arr, n, target, &first, &last);
    printf("  搜索范围: [%d, %d]\n", first, last);

    printf("\n目标值 %d:\n", not_found);
    print_search_result("  基础二分搜索",
        binary_search_basic(arr, n, not_found), not_found);

    int lb = lower_bound(arr, n, not_found);
    int ub = upper_bound(arr, n, not_found);
    printf("\nLower bound of %d: 索引 %d (值 %d)\n",
           not_found, lb, lb >= 0 ? arr[lb] : -1);
    printf("Upper bound of %d: 索引 %d (值 %d)\n",
           not_found, ub, ub >= 0 ? arr[ub] : -1);

    int insert_pos = binary_search_insert_position(arr, n, 30);
    printf("\n30 应该插入的位置: %d\n", insert_pos);

    // 三分搜索示例
    double max_point = ternary_search_max(example_func, 0, 10, 1e-6);
    printf("\n三分搜索: f(x)=-x²+4x+5 的最大值在 x=%.6f 处\n", max_point);

    return 0;
}
