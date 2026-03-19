/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\02_Searching_Algorithms.md
 * Line: 1171
 * Language: c
 * Block ID: ac18ce4e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief Boyer-Moore算法 - 坏字符规则
 * @note 从右向左比较，利用坏字符和好后缀规则跳跃
 */

#define ALPHABET_SIZE 256

// 构建坏字符表
void build_bad_char_table(const char *pattern, int bad_char[]) {
    int m = (int)strlen(pattern);

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        bad_char[i] = -1;
    }

    for (int i = 0; i < m; i++) {
        bad_char[(unsigned char)pattern[i]] = i;
    }
}

// 构建好后缀表
void build_good_suffix_table(const char *pattern, int good_suffix[]) {
    int m = (int)strlen(pattern);
    int suffix[m + 1];

    // 计算suffix数组
    suffix[m] = m + 1;
    for (int i = m - 1; i >= 0; i--) {
        int j = i;
        while (j >= 0 && pattern[j] == pattern[m - 1 - i + j]) {
            j--;
        }
        suffix[i] = i - j;
    }

    // 计算good_suffix
    for (int i = 0; i < m; i++) {
        good_suffix[i] = m;
    }

    for (int i = m - 1; i >= 0; i--) {
        if (suffix[i] == i + 1) {
            for (int j = 0; j < m - 1 - i; j++) {
                if (good_suffix[j] == m) {
                    good_suffix[j] = m - 1 - i;
                }
            }
        }
    }

    for (int i = 0; i <= m - 2; i++) {
        good_suffix[m - 1 - suffix[i]] = m - 1 - i;
    }
}

int boyer_moore_search(const char *text, const char *pattern) {
    int m = (int)strlen(pattern);
    int n = (int)strlen(text);

    if (m == 0) return 0;
    if (m > n) return -1;

    int bad_char[ALPHABET_SIZE];
    int good_suffix[m];

    build_bad_char_table(pattern, bad_char);
    build_good_suffix_table(pattern, good_suffix);

    int s = 0;  // 当前对齐位置
    while (s <= n - m) {
        int j = m - 1;

        // 从右向左比较
        while (j >= 0 && pattern[j] == text[s + j]) {
            j--;
        }

        if (j < 0) {
            return s;  // 找到匹配
        }

        // 计算跳跃距离
        int bc_shift = j - bad_char[(unsigned char)text[s + j]];
        if (bc_shift < 1) bc_shift = 1;

        int gs_shift = good_suffix[j];

        s += (bc_shift > gs_shift) ? bc_shift : gs_shift;
    }

    return -1;
}

// 简化的Boyer-Moore（仅坏字符）
int boyer_moore_simple(const char *text, const char *pattern) {
    int m = (int)strlen(pattern);
    int n = (int)strlen(text);

    if (m == 0) return 0;

    int bad_char[ALPHABET_SIZE];
    for (int i = 0; i < ALPHABET_SIZE; i++) bad_char[i] = m;
    for (int i = 0; i < m; i++) {
        bad_char[(unsigned char)pattern[i]] = m - 1 - i;
    }

    for (int s = 0; s <= n - m; ) {
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[s + j]) j--;
        if (j < 0) return s;
        s += bad_char[(unsigned char)text[s + m - 1]];
    }
    return -1;
}
