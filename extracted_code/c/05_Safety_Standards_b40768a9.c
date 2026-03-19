/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\05_Safety_Standards.md
 * Line: 321
 * Language: c
 * Block ID: b40768a9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 违反规则：不安全的宏 (Rule 20.7)
#define SQUARE(x) (x * x)
SQUARE(a + b);  // 展开为 a + b * a + b，错误！

// ✅ 合规代码 (Rule 20.7)
#define SQUARE(x) ((x) * (x))

// ❌ 违反规则：多语句宏没有do-while
#define INIT(x, y) x = 0; y = 0;

// ✅ 合规代码 (Rule 20.7)
#define INIT(x, y) do { (x) = 0; (y) = 0; } while(0)

// ✅ 更好：使用内联函数 (Rule 20.4)
static inline int32_t square(int32_t x) {
    return x * x;
}
