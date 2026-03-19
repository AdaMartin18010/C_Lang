/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\04_Dynamic_Programming_Basics.md
 * Line: 581
 * Language: c
 * Block ID: 4f20234b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 最长递增子序列 - O(n²)解法
 */
int length_of_lis_n2(const int nums[], int n) {
    if (n == 0) return 0;

    int *dp = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        dp[i] = 1;  // 每个元素至少构成长度为1的序列
    }

    int max_len = 1;

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (nums[j] < nums[i]) {
                dp[i] = dp[i] > dp[j] + 1 ? dp[i] : dp[j] + 1;
            }
        }
        if (dp[i] > max_len) {
            max_len = dp[i];
        }
    }

    free(dp);
    return max_len;
}

/**
 * @brief 最长递增子序列 - O(n log n)解法
 * @note 使用二分搜索维护一个递增序列
 */
int length_of_lis_nlogn(const int nums[], int n) {
    if (n == 0) return 0;

    // tails[i] = 长度为i+1的递增子序列的最小末尾元素
    int *tails = malloc(n * sizeof(int));
    int size = 0;

    for (int i = 0; i < n; i++) {
        int left = 0, right = size;

        // 二分查找插入位置
        while (left < right) {
            int mid = left + (right - left) / 2;
            if (tails[mid] < nums[i]) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }

        tails[left] = nums[i];
        if (left == size) {
            size++;
        }
    }

    free(tails);
    return size;
}

/**
 * @brief LIS - 返回具体序列
 */
int length_of_lis_with_sequence(const int nums[], int n, int result[]) {
    if (n == 0) return 0;

    int *dp = malloc(n * sizeof(int));
    int *parent = malloc(n * sizeof(int));  // 记录前驱

    for (int i = 0; i < n; i++) {
        dp[i] = 1;
        parent[i] = -1;
    }

    int max_len = 1;
    int max_idx = 0;

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (nums[j] < nums[i] && dp[j] + 1 > dp[i]) {
                dp[i] = dp[j] + 1;
                parent[i] = j;
            }
        }
        if (dp[i] > max_len) {
            max_len = dp[i];
            max_idx = i;
        }
    }

    // 回溯构建序列
    int idx = max_len - 1;
    int curr = max_idx;
    while (curr != -1) {
        result[idx--] = nums[curr];
        curr = parent[curr];
    }

    free(dp);
    free(parent);
    return max_len;
}

/**
 * @brief 最长连续递增子序列
 */
int length_of_lcis(const int nums[], int n) {
    if (n == 0) return 0;

    int max_len = 1;
    int current_len = 1;

    for (int i = 1; i < n; i++) {
        if (nums[i] > nums[i - 1]) {
            current_len++;
            if (current_len > max_len) {
                max_len = current_len;
            }
        } else {
            current_len = 1;
        }
    }

    return max_len;
}
