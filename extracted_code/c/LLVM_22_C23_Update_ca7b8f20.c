/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_22_C23_Update.md
 * Line: 219
 * Language: c
 * Block ID: ca7b8f20
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 生成唯一的标识符名
#define UNIQUE_NAME(base) base##__COUNTER__
#define UNIQUE_VAR(base) UNIQUE_NAME(_##base##_)

// 使用示例
void example(void) {
    int UNIQUE_VAR(temp) = 1;   // 展开为: int _temp_0 = 1;
    int UNIQUE_VAR(temp) = 2;   // 展开为: int _temp_1 = 2;
    int UNIQUE_VAR(temp) = 3;   // 展开为: int _temp_2 = 3;
}
