/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\14_MISRA_C_2023_Rules_14.md
 * Line: 252
 * Language: c
 * Block ID: a838b1d5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 编译时常量初始化 */
static const int MAX_SIZE = 100;
static const float PI = 3.14159f;

/* ✅ 合规 - 显式运行时初始化 */
static int initialized = 0;
static Config g_config;

void init_system(void)
{
    if (!initialized) {
        g_config.timeout = 30;
        g_config.retries = 3;
        initialized = 1;
    }
}

/* ✅ 合规 - 使用构造函数模式 */
typedef struct {
    int data[100];
    size_t count;
} Buffer;

Buffer buffer_create(void)
{
    Buffer buf = {0};  /* 编译时初始化 */
    return buf;
}
