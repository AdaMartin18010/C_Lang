/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 342
 * Language: c
 * Block ID: 1450fc66
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 Rule 2.3 - 未使用的类型 */
typedef int MyInt;  /* 未使用 */

struct UnusedStruct {  /* 未使用 */
    int x;
    int y;
};

union UnusedUnion {  /* 未使用 */
    int i;
    float f;
};

enum UnusedEnum {  /* 未使用 */
    STATE_A,
    STATE_B
};

void func(void)
{
    /* 没有使用上述任何类型 */
}
