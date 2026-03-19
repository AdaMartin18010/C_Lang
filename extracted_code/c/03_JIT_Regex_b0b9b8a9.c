/*
 * Auto-generated from: 03_System_Technology_Domains\03_JIT_Regex.md
 * Line: 124
 * Language: c
 * Block ID: b0b9b8a9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DFA状态表示
typedef struct DFAState {
    uint32_t id;
    bool is_accept;
    struct DFAState *trans[256];  // ASCII字符集
    BitSet *nfa_states;            // 对应的NFA状态集
} DFAState;

// NFA状态（与Thompson构造一致）
typedef struct NFAState {
    uint32_t id;
    char c;                        // CHAR: 匹配字符
    struct NFAState *out1;         // 出边1
    struct NFAState *out2;         // 出边2（用于SPLIT）
    int type;                      // 状态类型
} NFAState;
