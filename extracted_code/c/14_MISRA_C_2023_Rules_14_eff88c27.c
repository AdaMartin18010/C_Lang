/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\14_MISRA_C_2023_Rules_14.md
 * Line: 28
 * Language: c
 * Block ID: eff88c27
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 未初始化使用 */
void func(void)
{
    int x;
    printf("%d\n", x);  /* 未定义行为！ */
}

/* ❌ 违反 - 条件路径未初始化 */
void process(int flag)
{
    int value;
    if (flag) {
        value = 10;
    }
    /* else分支未初始化 */
    use(value);  /* 危险！ */
}

/* ❌ 违反 - 循环中未初始化 */
void loop_issue(void)
{
    int sum;
    for (int i = 0; i < 10; i++) {
        sum = sum + i;  /* sum首次使用未初始化 */
    }
}

/* ❌ 违反 - 复杂控制流 */
void complex_flow(int a, int b)
{
    int result;
    if (a > 0) {
        if (b > 0) {
            result = a + b;
        }
    } else {
        result = 0;
    }
    /* 某些路径result未初始化 */
    use(result);
}
