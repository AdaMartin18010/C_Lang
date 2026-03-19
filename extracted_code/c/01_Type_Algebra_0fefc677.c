/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\02_Algebraic_Topology\01_Type_Algebra.md
 * Line: 56
 * Language: c
 * Block ID: 0fefc677
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 积类型: A × B
// 基数 = |A| × |B|
struct Pair {      // bool × bool = 4种值
    bool first;
    bool second;
};

// 和类型: A + B
// 基数 = |A| + |B|
enum EitherTag { LEFT, RIGHT };
struct Either {    // bool + char = 2 + 256 = 258
    enum EitherTag tag;
    union {
        bool left;
        char right;
    };
};

// 单位类型: 1
// void 只有一个值（终止）
void unit(void) { }

// 空类型: 0
// 无法构造的值（如 never 返回的函数）
_Noreturn void absurd(void) {
    while (1);  // 永不返回
}
