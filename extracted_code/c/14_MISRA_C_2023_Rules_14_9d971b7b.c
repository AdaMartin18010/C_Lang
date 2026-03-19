/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\14_MISRA_C_2023_Rules_14.md
 * Line: 295
 * Language: c
 * Block ID: 9d971b7b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 标量零初始化 */
int x = 0;
float f = 0.0f;
char *p = NULL;
bool flag = false;

/* ✅ 聚合类型零初始化 */
int arr[100] = {0};  /* 所有元素为0 */

struct Point p = {0};  /* 所有成员为0 */

struct Config {
    int timeout;
    int retries;
    char name[32];
    float threshold;
};

struct Config cfg = {0};  /* 全部清零：timeout=0, retries=0, name="", threshold=0.0 */

/* ✅ 静态/全局自动零初始化 */
static int s_counter;  /* 隐式0，但建议显式 */
static int s_counter_explicit = 0;  /* 推荐 */

/* 全局变量 */
int g_global = 0;  /* 显式 */
