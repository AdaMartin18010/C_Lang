/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\22_MISRA_C_2023_Rules_22.md
 * Line: 211
 * Language: c
 * Block ID: 04573beb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 缩进 - 4空格 */
void func(void)
{
    if (condition) {
        do_something();
    } else {
        do_other();
    }
}

/* ✅ 大括号 - K&R风格 */
void func(void) {
    while (condition) {
        /* ... */
    }
}

/* ✅ 命名规范 */
#define CONSTANT_VALUE 100    /* 宏：全大写 */

typedef struct {
    int member_var;           /* 成员：小写+下划线 */
} type_name_t;                /* typedef：小写+_t */

void function_name(void);     /* 函数：小写+下划线 */
int global_var;               /* 全局：g_前缀 */
static int static_var;        /* 静态：s_前缀 */

/* ✅ 行长 - 80或100字符 */
void long_function_name(
    int param1,
    int param2,
    int param3);  /* 参数多时分行 */
