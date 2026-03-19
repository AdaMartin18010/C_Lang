/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\14_MISRA_C_2023_Rules_14.md
 * Line: 328
 * Language: c
 * Block ID: 7add7234
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 初始化模式总结 */

/* 1. 声明时立即初始化 */
void immediate_init(void)
{
    int a = 0;
    float b = 0.0f;
    char *str = NULL;
    struct Point p = {0};
}

/* 2. 常量数据使用const */
const int MAX_RETRIES = 3;
const char *VERSION = "1.0.0";
const float DEFAULT_THRESHOLD = 0.5f;

/* 3. 复杂初始化使用初始化函数 */
static Config g_config;
static bool g_initialized = false;

static void config_init(void)
{
    g_config.timeout = DEFAULT_TIMEOUT;
    g_config.retries = DEFAULT_RETRIES;
    g_config.threshold = DEFAULT_THRESHOLD;
    g_initialized = true;
}

/* 4. 检查初始化状态 */
int config_get_timeout(void)
{
    if (!g_initialized) {
        config_init();
    }
    return g_config.timeout;
}

/* 5. 使用宏确保初始化 */
#define SAFE_DECLARE(type, var, init) type var = init

SAFE_DECLARE(int, counter, 0);
SAFE_DECLARE(float, factor, 1.0f);
