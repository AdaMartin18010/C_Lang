/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_New_Proposals_2026.md
 * Line: 248
 * Language: c
 * Block ID: 44efa2f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C2y 提案 (N3636)

// 生成唯一标识符
#define UNIQUE_NAME(prefix) prefix##__COUNTER__

int UNIQUE_NAME(var_);  // 展开为 var_0
int UNIQUE_NAME(var_);  // 展开为 var_1
int UNIQUE_NAME(var_);  // 展开为 var_2

// 用于注册表
#define REGISTER(func) \
    static void register_##func(void) __attribute__((constructor)); \
    static void register_##func(void) { \
        registry[__COUNTER__] = func; \
    }
