/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\07_MISRA_C_2023_Rules_7.md
 * Line: 70
 * Language: c
 * Block ID: e71a3e6a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 直接解引用 */
void process_data(int *data)
{
    *data = 42;  /* 可能为空指针！ */
}

/* ❌ 违反 - 返回值未检查 */
FILE *fp = fopen("file.txt", "r");
fgetc(fp);  /* fp可能为NULL */

/* ❌ 违反 - malloc返回未检查 */
int *arr = malloc(100 * sizeof(int));
arr[0] = 1;  /* arr可能为NULL */

/* ❌ 违反 - 间接解引用 */
void func(int **pp)
{
    **pp = 42;  /* pp或*pp可能为NULL */
}
