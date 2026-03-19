/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 335
 * Language: c
 * Block ID: e4bdb839
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 不可达代码 */
void func(void)
{
    return;
    int x = 0;  /* 永远不会执行 */
    (void)x;
}

/* ❌ 违反 - 条件导致的死代码 */
void test(int x)
{
    if (x > 0) {
        return;
    } else if (x <= 0) {
        return;
    }
    int y = 0;  /* 永远不会到达 */
    (void)y;
}

/* ❌ 违反 - break后的代码 */
switch (x) {
    case 1:
        break;
        x = 0;  /* 不可达 */
}
