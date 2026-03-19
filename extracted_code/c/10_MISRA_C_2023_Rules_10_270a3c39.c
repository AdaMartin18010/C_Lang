/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 153
 * Language: c
 * Block ID: 270a3c39
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 设计模式：防御性初始化 */

/* 1. 标量类型 */
bool flag = false;
int count = 0;
float value = 0.0f;
char *ptr = NULL;

/* 2. 数组 */
/* 静态数组 */
int buffer[BUF_SIZE] = {0};

/* 动态数组 */
int *arr = calloc(n, sizeof(int));  /* calloc自动清零 */
if (arr == NULL) {
    /* 错误处理 */
}

/* 3. 结构体 */
struct Config {
    int timeout;
    int retries;
    bool debug;
};

/* 静态初始化 */
static struct Config default_config = {
    .timeout = 30,
    .retries = 3,
    .debug = false
};

/* 运行时初始化 */
struct Config load_config(void)
{
    struct Config cfg = {0};  /* 先清零 */

    /* 从文件读取 */
    cfg.timeout = read_timeout();
    if (cfg.timeout <= 0) {
        cfg.timeout = default_config.timeout;
    }

    return cfg;
}
