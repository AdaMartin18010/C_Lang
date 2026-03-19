/*
 * Auto-generated from: 12_Practice_Exercises\07_Algorithm_Challenges.md
 * Line: 100
 * Language: c
 * Block ID: 29e0f3fc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 暴力匹配
int brute_force(const char *text, const char *pattern);

// KMP算法
int kmp_search(const char *text, const char *pattern);

// Boyer-Moore算法
int bm_search(const char *text, const char *pattern);

// Rabin-Karp算法
int rk_search(const char *text, const char *pattern);

// Aho-Corasick算法（多模式匹配）
typedef struct ACNode ACNode;
typedef struct {
    ACNode *root;
    int pattern_count;
} ACAutomaton;

ACAutomaton* ac_build(const char **patterns, int n);
void ac_search(ACAutomaton *ac, const char *text,
               void (*callback)(int pattern_id, int pos));
