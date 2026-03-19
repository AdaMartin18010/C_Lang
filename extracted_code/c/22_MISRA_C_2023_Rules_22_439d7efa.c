/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\22_MISRA_C_2023_Rules_22.md
 * Line: 93
 * Language: c
 * Block ID: 439d7efa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - return后的代码 */
int func(void)
{
    return 0;
    printf("Never reached\n");  /* 死代码 */
}

/* ❌ 违反 - 无条件跳转后的代码 */
void func(void)
{
    goto end;
    printf("Never reached\n");  /* 死代码 */
end:
    return;
}

/* ❌ 违反 - 总是假的条件 */
void func(void)
{
    if (0) {  /* 编译时常量 */
        printf("Never executed\n");  /* 死代码 */
    }
}
