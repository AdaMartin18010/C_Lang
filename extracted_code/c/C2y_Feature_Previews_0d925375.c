/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 1174
 * Language: c
 * Block ID: 0d925375
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// N3183: 内存安全注解

// 边界安全属性 (提案语法)
void safe_copy(
    char* [[bounds(__count_to(n))]] dst,
    const char* [[bounds(__count_to(n))]] src,
    size_t n
);

// 等价意图: 编译器可以检查缓冲区溢出
// 实际C2y可能更简单的语法

// 可能的标准属性方案
void safe_copy(
    [[count(n)]] char* dst,
    [[count(n)]] const char* src,
    size_t n
);
