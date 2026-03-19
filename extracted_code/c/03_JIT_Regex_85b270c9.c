/*
 * Auto-generated from: 03_System_Technology_Domains\03_JIT_Regex.md
 * Line: 145
 * Language: c
 * Block ID: 85b270c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 传统回溯的问题: 指数复杂度
// 优化策略1: 记忆化 (Memoization)

typedef struct {
    const char *input_pos;
    int pattern_pc;
    bool matched;
    bool visited;
} MemoEntry;

#define MEMO_TABLE_SIZE 1024

static MemoEntry memo_table[MEMO_TABLE_SIZE];

// 检查记忆表
bool check_memo(const char *input, int pc, bool *result) {
    uint32_t hash = ((uintptr_t)input ^ pc) % MEMO_TABLE_SIZE;
    if (memo_table[hash].visited &&
        memo_table[hash].input_pos == input &&
        memo_table[hash].pattern_pc == pc) {
        *result = memo_table[hash].matched;
        return true;
    }
    return false;
}

// 优化策略2: 独占匹配优化
// 对于模式 (a+)+b，检测嵌套量词并预警

bool has_catastrophic_backtrack(const char *pattern) {
    // 检测危险模式: (a+)+, (a*)*, (a+)* 等
    int depth = 0;
    bool last_was_quantifier = false;

    for (const char *p = pattern; *p; p++) {
        switch (*p) {
            case '(':
                depth++;
                last_was_quantifier = false;
                break;
            case ')':
                if (last_was_quantifier && depth > 0) {
                    // 检测嵌套量词
                    return true;
                }
                depth--;
                last_was_quantifier = true;
                break;
            case '*':
            case '+':
            case '?':
                if (last_was_quantifier) return true;
                last_was_quantifier = true;
                break;
            default:
                last_was_quantifier = false;
        }
    }
    return false;
}
