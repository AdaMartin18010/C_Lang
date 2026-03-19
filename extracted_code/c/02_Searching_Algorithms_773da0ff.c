/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\02_Searching_Algorithms.md
 * Line: 1011
 * Language: c
 * Block ID: 773da0ff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 字符串搜索算法集
 * 文件名: string_search.c
 * 编译: gcc -std=c23 -o string_search string_search.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 构建KMP的部分匹配表（LPS数组）
 * @param pattern 模式串
 * @param lps 输出的LPS数组
 */
void compute_lps_array(const char *pattern, int lps[]) {
    size_t m = strlen(pattern);
    size_t len = 0;  // 当前最长前后缀长度
    lps[0] = 0;

    size_t i = 1;
    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = (int)len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

/**
 * @brief KMP字符串搜索
 * @param text 文本串
 * @param pattern 模式串
 * @return 第一个匹配位置的索引，未找到返回-1
 */
int kmp_search(const char *text, const char *pattern) {
    size_t n = strlen(text);
    size_t m = strlen(pattern);

    if (m == 0) return 0;
    if (m > n) return -1;

    int *lps = malloc(m * sizeof(int));
    if (!lps) return -1;

    compute_lps_array(pattern, lps);

    size_t i = 0;  // text索引
    size_t j = 0;  // pattern索引

    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }

        if (j == m) {
            free(lps);
            return (int)(i - j);  // 找到匹配
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    free(lps);
    return -1;
}

/**
 * @brief KMP搜索所有匹配位置
 */
void kmp_search_all(const char *text, const char *pattern,
                    int results[], int *count, int max_results) {
    size_t n = strlen(text);
    size_t m = strlen(pattern);
    *count = 0;

    if (m == 0 || m > n) return;

    int *lps = malloc(m * sizeof(int));
    if (!lps) return;

    compute_lps_array(pattern, lps);

    size_t i = 0, j = 0;
    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }

        if (j == m) {
            if (*count < max_results) {
                results[(*count)++] = (int)(i - j);
            }
            j = lps[j - 1];
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    free(lps);
}

/**
 * @brief 简化的KMP实现（教学版）
 */
int kmp_simple(const char *text, const char *pattern) {
    int m = (int)strlen(pattern);
    int n = (int)strlen(text);

    if (m == 0) return 0;

    // 构建LPS
    int lps[m];
    lps[0] = 0;
    for (int i = 1, len = 0; i < m; ) {
        if (pattern[i] == pattern[len]) {
            lps[i++] = ++len;
        } else if (len > 0) {
            len = lps[len - 1];
        } else {
            lps[i++] = 0;
        }
    }

    // 搜索
    for (int i = 0, j = 0; i < n; ) {
        if (text[i] == pattern[j]) {
            i++; j++;
            if (j == m) return i - j;
        } else if (j > 0) {
            j = lps[j - 1];
        } else {
            i++;
        }
    }
    return -1;
}
