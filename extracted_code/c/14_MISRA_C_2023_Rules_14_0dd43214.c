/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\14_MISRA_C_2023_Rules_14.md
 * Line: 133
 * Language: c
 * Block ID: 0dd43214
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 数组未初始化 */
void array_issues(void)
{
    int arr[10];
    int x = arr[0];  /* 未定义行为 */
}

/* ❌ 违反 - 部分初始化 */
void partial_init(void)
{
    int arr[10] = {1, 2, 3};  /* 只有前3个初始化 */
    /* arr[3]到arr[9]未明确初始化 */
}

/* ❌ 违反 - 结构体未初始化 */
struct Point { int x, y; };

void struct_issue(void)
{
    struct Point p;
    printf("%d\n", p.x);  /* 未定义行为 */
}

/* ❌ 违反 - 嵌套结构体 */
struct Rect {
    struct Point top_left;
    struct Point bottom_right;
};

void nested_issue(void)
{
    struct Rect r;
    use(r.top_left.x);  /* 未初始化 */
}
