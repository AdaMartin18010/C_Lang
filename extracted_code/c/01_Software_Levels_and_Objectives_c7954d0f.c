/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\DO_178C\01_Software_Levels_and_Objectives.md
 * Line: 167
 * Language: c
 * Block ID: c7954d0f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * DAL A级别编码要求
 */

/* 1. 无动态内存分配 */
/* 禁止: malloc/free在运行时 */

/* 2. 无递归 */
int factorial(int n)  /* 不允许递归 */
{
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

/* 3. 所有路径有确定行为 */
int divide(int a, int b, int *result)
{
    if (b == 0) {
        *result = 0;
        return ERROR_DIV_ZERO;  /* 明确处理 */
    }
    *result = a / b;
    return SUCCESS;
}

/* 4. 初始化所有变量 */
void process(void)
{
    int sum = 0;  /* 初始化 */
    for (int i = 0; i < 10; i++) {
        sum += i;
    }
}

/* 5. 无未使用代码 */
/* 所有函数、变量、宏必须使用 */

/* 6. 单一出口点 */
int func(int x)
{
    int result;
    if (x < 0) {
        result = ERROR_NEGATIVE;
    } else if (x > 100) {
        result = ERROR_TOO_LARGE;
    } else {
        result = process(x);
    }
    return result;  /* 单一出口 */
}
