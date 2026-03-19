/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\02_Searching_Algorithms.md
 * Line: 1290
 * Language: c
 * Block ID: 08dd70f2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief Rabin-Karp算法 - 滚动哈希
 * @note 使用哈希值快速筛选可能匹配的位置
 */

#define PRIME 101  // 一个较大的质数
#define BASE 256   // 字符集大小

// 计算哈希值
unsigned long long compute_hash(const char *str, int len) {
    unsigned long long hash = 0;
    for (int i = 0; i < len; i++) {
        hash = (hash * BASE + (unsigned char)str[i]) % PRIME;
    }
    return hash;
}

int rabin_karp_search(const char *text, const char *pattern) {
    int m = (int)strlen(pattern);
    int n = (int)strlen(text);

    if (m == 0) return 0;
    if (m > n) return -1;

    // 计算BASE^(m-1) % PRIME
    unsigned long long h = 1;
    for (int i = 0; i < m - 1; i++) {
        h = (h * BASE) % PRIME;
    }

    unsigned long long pattern_hash = compute_hash(pattern, m);
    unsigned long long text_hash = compute_hash(text, m);

    for (int i = 0; i <= n - m; i++) {
        // 哈希值相等，验证是否真正匹配
        if (pattern_hash == text_hash) {
            if (strncmp(text + i, pattern, m) == 0) {
                return i;
            }
        }

        // 滚动计算下一个哈希值
        if (i < n - m) {
            text_hash = (BASE * (text_hash + PRIME -
                        (unsigned char)text[i] * h % PRIME) +
                        (unsigned char)text[i + m]) % PRIME;
        }
    }

    return -1;
}

// 多模式匹配版本的Rabin-Karp
void rabin_karp_multiple(const char *text, const char *patterns[],
                         int num_patterns, int results[]) {
    int n = (int)strlen(text);

    for (int p = 0; p < num_patterns; p++) {
        results[p] = rabin_karp_search(text, patterns[p]);
    }
}
