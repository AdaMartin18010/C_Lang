/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\README.md
 * Line: 793
 * Language: c
 * Block ID: b21b2a0b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file re2_strategy.c
 * @brief RE2风格的正则匹配策略
 *
 * RE2保证最坏情况下线性时间复杂度
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* RE2核心策略:
 * 1. 使用DFA进行快速匹配
 * 2. DFA状态数限制，避免内存爆炸
 * 3. 不支持反向引用，避免NP完全问题
 * 4. One-pass优化：某些模式可一次遍历完成
 */

/* One-pass NFA检测 */
bool is_one_pass(const char *pattern) {
    /* 以下条件满足时为one-pass:
     * - 无回溯需求
     * - 每个位置的选择是确定的
     * - 无复杂的贪婪/非贪婪交替
     */

    /* 简化检测：不包含以下模式 */
    if (strstr(pattern, "(?") != NULL) return false;  /* 无扩展语法 */
    if (strstr(pattern, "*?") != NULL) return false; /* 无非贪婪 */
    if (strstr(pattern, "|") != NULL) {
        /* 有选择操作符，需要更复杂分析 */
        /* 简化：假设不是one-pass */
        return false;
    }

    return true;
}

/* Bitap算法（Baeza-Yates-Gonnet）- 近似匹配 */
/* 适合短模式，位并行实现 */

#include <stdint.h>

#define ALPHABET_SIZE 256

/* Bitap匹配 */
int bitap_search(const char *text, const char *pattern, int max_errors) {
    int m = strlen(pattern);
    if (m > 31) return -1;  /* 限制模式长度 */

    /* 构建掩码表 */
    uint32_t mask[ALPHABET_SIZE];
    for (int i = 0; i < ALPHABET_SIZE; i++) mask[i] = ~0;

    for (int i = 0; i < m; i++) {
        mask[(unsigned char)pattern[i]] &= ~(1 << i);
    }

    uint32_t R[max_errors + 1];
    for (int i = 0; i <= max_errors; i++) R[i] = ~0;
    R[0] &= ~1;  /* 初始状态 */

    for (int pos = 0; text[pos] != '\0'; pos++) {
        uint32_t old_R = R[0];
        R[0] = (R[0] | mask[(unsigned char)text[pos]]) << 1;

        for (int e = 1; e <= max_errors; e++) {
            uint32_t tmp = R[e];
            R[e] = ((R[e] | mask[(unsigned char)text[pos]]) << 1)
                   | old_R
                   | (old_R << 1)
                   | (R[e-1] << 1);
            old_R = tmp;
        }

        /* 检查匹配 */
        if (!(R[max_errors] & (1 << m))) {
            return pos - m + 1;
        }
    }

    return -1;
}

/* DFA缓存策略 */
#define DFA_CACHE_SIZE 1024

typedef struct {
    int from_state;
    char input;
    int to_state;
} DFATransition;

static DFATransition dfa_cache[DFA_CACHE_SIZE];
static int cache_size = 0;

/* 简单的DFA转移缓存 */
int dfa_transition(int state, char c, int (*nfa_step)(int, char)) {
    /* 先在缓存中查找 */
    for (int i = 0; i < cache_size; i++) {
        if (dfa_cache[i].from_state == state &&
            dfa_cache[i].input == c) {
            return dfa_cache[i].to_state;
        }
    }

    /* 计算新状态 */
    int new_state = nfa_step(state, c);

    /* 加入缓存 */
    if (cache_size < DFA_CACHE_SIZE) {
        dfa_cache[cache_size++] = (DFATransition){state, c, new_state};
    }

    return new_state;
}

/* RE2风格匹配接口 */
bool re2_match(const char *pattern, const char *text) {
    printf("RE2 matching '%s' against '%s'\n", pattern, text);

    /* 1. 解析模式 */
    /* 2. 构建NFA */
    /* 3. 使用DFA模拟（带缓存）*/
    /* 4. 返回结果 */

    if (is_one_pass(pattern)) {
        printf("  Using one-pass optimization\n");
    }

    /* 简化：使用bitap演示 */
    if (strlen(pattern) <= 31) {
        int pos = bitap_search(text, pattern, 0);
        return pos >= 0;
    }

    return false;
}

int main(void) {
    printf("=== RE2-Style Regex Engine Demo ===\n\n");

    printf("One-pass detection:\n");
    printf("  'abc': %s\n", is_one_pass("abc") ? "yes" : "no");
    printf("  'a|b': %s\n", is_one_pass("a|b") ? "yes" : "no");

    printf("\nBitap approximate search (1 error allowed):\n");
    int pos = bitap_search("hello world", "hallo", 1);
    printf("  'hallo' in 'hello world' (k=1): pos=%d\n", pos);

    return 0;
}
