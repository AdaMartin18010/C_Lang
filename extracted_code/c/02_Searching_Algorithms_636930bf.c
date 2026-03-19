/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\02_Searching_Algorithms.md
 * Line: 743
 * Language: c
 * Block ID: 636930bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 高级搜索算法集
 * 文件名: advanced_search.c
 * 编译: gcc -std=c23 -o advanced_search advanced_search.c
 */

#include <stdio.h>
#include <math.h>

/**
 * @brief 插值搜索
 * @note 数据应均匀分布以获得最佳性能
 */
int interpolation_search(const int arr[], size_t n, int target) {
    size_t left = 0;
    size_t right = n - 1;

    while (left <= right && target >= arr[left] && target <= arr[right]) {
        if (left == right) {
            return (arr[left] == target) ? (int)left : -1;
        }

        // 计算插值位置
        size_t pos = left + ((target - arr[left]) * (right - left)) /
                     (arr[right] - arr[left]);

        if (arr[pos] == target) {
            return (int)pos;
        }

        if (arr[pos] < target) {
            left = pos + 1;
        } else {
            right = pos - 1;
        }
    }
    return -1;
}

/**
 * @brief 指数搜索（倍增搜索）
 * @note 适合无界数组或目标在开头附近的情况
 */
int exponential_search(const int arr[], size_t n, int target) {
    if (n == 0) return -1;
    if (arr[0] == target) return 0;

    // 找到范围
    size_t bound = 1;
    while (bound < n && arr[bound] <= target) {
        bound *= 2;
    }

    // 在确定范围内二分搜索
    size_t left = bound / 2;
    size_t right = (bound < n) ? bound : n;

    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (arr[mid] == target) return (int)mid;
        if (arr[mid] < target) left = mid + 1;
        else right = mid;
    }
    return -1;
}

/**
 * @brief 跳跃搜索（Jump Search）
 * @note 步长为 √n，在块内线性搜索
 */
int jump_search(const int arr[], size_t n, int target) {
    size_t step = (size_t)sqrt(n);
    size_t prev = 0;

    // 找到所在块
    while (prev < n && arr[(prev + step < n) ? prev + step : n - 1] < target) {
        prev += step;
        if (prev >= n) return -1;
    }

    // 在块内线性搜索
    size_t end = (prev + step < n) ? prev + step : n;
    for (size_t i = prev; i < end; i++) {
        if (arr[i] == target) return (int)i;
        if (arr[i] > target) break;
    }
    return -1;
}

/**
 * @brief 斐波那契搜索
 * @note 使用斐波那契数列分割数组
 */
int fibonacci_search(const int arr[], size_t n, int target) {
    // 找到大于等于n的最小斐波那契数
    int fib2 = 0;  // F(k-2)
    int fib1 = 1;  // F(k-1)
    int fib = fib1 + fib2;  // F(k)

    while ((size_t)fib < n) {
        fib2 = fib1;
        fib1 = fib;
        fib = fib1 + fib2;
    }

    int offset = -1;

    while (fib > 1) {
        int i = (offset + fib2 < (int)n) ? offset + fib2 : (int)n - 1;

        if (arr[i] < target) {
            fib = fib1;
            fib1 = fib2;
            fib2 = fib - fib1;
            offset = i;
        } else if (arr[i] > target) {
            fib = fib2;
            fib1 = fib1 - fib2;
            fib2 = fib - fib1;
        } else {
            return i;
        }
    }

    if (fib1 && (size_t)(offset + 1) < n && arr[offset + 1] == target) {
        return offset + 1;
    }
    return -1;
}

/**
 * @brief 二分搜索变体 - 查找旋转有序数组中的最小值
 */
int find_min_in_rotated(const int arr[], size_t n) {
    size_t left = 0;
    size_t right = n - 1;

    while (left < right) {
        size_t mid = left + (right - left) / 2;

        if (arr[mid] > arr[right]) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return (int)left;
}

/**
 * @brief 在旋转有序数组中搜索
 */
int search_in_rotated(const int arr[], size_t n, int target) {
    size_t left = 0;
    size_t right = n;

    while (left < right) {
        size_t mid = left + (right - left) / 2;

        // 判断哪一半是有序的
        if (arr[mid] == target) return (int)mid;

        if (arr[left] <= arr[mid]) {
            // 左半部分有序
            if (arr[left] <= target && target < arr[mid]) {
                right = mid;
            } else {
                left = mid + 1;
            }
        } else {
            // 右半部分有序
            if (arr[mid] < target && target <= arr[right - 1]) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
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
    // 均匀分布数据适合插值搜索
    int uniform_arr[100];
    for (int i = 0; i < 100; i++) {
        uniform_arr[i] = i * 2;  // 0, 2, 4, 6, ...
    }

    // 普通有序数组
    int arr[] = {5, 11, 12, 22, 25, 34, 64, 77, 90, 100, 120, 150};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    int target = 34;

    printf("=== 高级搜索算法演示 ===\n\n");

    printf("数组: [");
    for (size_t i = 0; i < n; i++) {
        printf("%d%s", arr[i], i < n - 1 ? ", " : "");
    }
    printf("]\n");
    printf("目标值: %d\n\n", target);

    print_search_result("插值搜索",
        interpolation_search(uniform_arr, 100, 68), 68);
    print_search_result("指数搜索",
        exponential_search(arr, n, target), target);
    print_search_result("跳跃搜索",
        jump_search(arr, n, target), target);
    print_search_result("斐波那契搜索",
        fibonacci_search(arr, n, target), target);

    // 旋转数组示例
    int rotated[] = {25, 34, 64, 77, 90, 5, 11, 12, 22};
    printf("\n旋转数组: [25, 34, 64, 77, 90, 5, 11, 12, 22]\n");
    int min_idx = find_min_in_rotated(rotated, 9);
    printf("最小值索引: %d (值: %d)\n", min_idx, rotated[min_idx]);
    print_search_result("在旋转数组中搜索",
        search_in_rotated(rotated, 9, 11), 11);

    return 0;
}
