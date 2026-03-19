/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 1049
 * Language: c
 * Block ID: 4e407a6a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// N3654: 概念/约束 (早期提案，可能C3x)

// 概念定义 (提案语法，非最终)
concept Addable = {
    { a + b } -> typeof(a);  // 要求支持+
};

// 约束模板 (类似_Generic但声明式)
void add(Addable a, Addable b) {
    return a + b;
}

// 更可能的C2y方案: _Generic增强
#define CONCEPT_CHECK(T, ...) \
    _Generic(T, __VA_ARGS__, default: static_assert(0, "Type mismatch"))

// 实际可用的C2y方案: 类型列表
#define IS_ARITHMETIC(T) \
    _Generic(T, \
        char: 1, unsigned char: 1, short: 1, unsigned short: 1, \
        int: 1, unsigned int: 1, long: 1, unsigned long: 1, \
        float: 1, double: 1, long double: 1, \
        default: 0 \
    )
