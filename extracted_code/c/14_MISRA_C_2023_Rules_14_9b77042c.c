/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\14_MISRA_C_2023_Rules_14.md
 * Line: 74
 * Language: c
 * Block ID: 9b77042c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 声明时初始化 */
void func(void)
{
    int x = 0;
    printf("%d\n", x);
}

/* ✅ 合规 - 所有路径初始化 */
void process(int flag)
{
    int value = 0;  /* 默认初始化 */
    if (flag) {
        value = 10;
    }
    use(value);
}

/* ✅ 合规 - 循环变量初始化 */
void loop_safe(void)
{
    int sum = 0;  /* 初始化 */
    for (int i = 0; i < 10; i++) {
        sum = sum + i;
    }
}

/* ✅ 合规 - 防御性初始化 */
void complex_flow(int a, int b)
{
    int result = 0;  /* 所有路径都有默认值 */
    if (a > 0) {
        if (b > 0) {
            result = a + b;
        }
    }
    use(result);
}

/* ✅ 合规 - 使用三元运算符 */
void compact_init(int flag)
{
    int value = flag ? 10 : 0;  /* 明确初始化 */
    use(value);
}
