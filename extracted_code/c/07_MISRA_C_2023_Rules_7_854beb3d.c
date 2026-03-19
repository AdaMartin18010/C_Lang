/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\07_MISRA_C_2023_Rules_7.md
 * Line: 94
 * Language: c
 * Block ID: 854beb3d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 参数检查 */
void process_data(int *data)
{
    if (data == NULL) {
        /* 错误处理 */
        return;
    }
    *data = 42;
}

/* ✅ 合规 - 契约式编程 */
void process_data_nonnull(int *data)
{
    /* 前置条件：data非NULL（契约保证）*/
    /* 运行时检查（防御性）*/
    if (data == NULL) {
        /* 违反契约，记录错误 */
        return;
    }
    *data = 42;
}

/* ✅ 合规 - 资源获取检查 */
FILE *fp = fopen("file.txt", "r");
if (fp == NULL) {
    /* 错误处理 */
    return;
}
/* 安全使用fp */
fgetc(fp);
fclose(fp);

/* ✅ 合规 - malloc检查 */
int *arr = malloc(100 * sizeof(int));
if (arr == NULL) {
    /* 内存分配失败处理 */
    return;
}
/* 安全使用arr */
arr[0] = 1;
free(arr);
