/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\04_Functions_Scope.md
 * Line: 335
 * Language: c
 * Block ID: fcdaad7c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 危险：scanf返回值被忽略
int x;
scanf("%d", &x);  // 如果输入失败，x未初始化！

// 正确做法
if (scanf("%d", &x) != 1) {
    fprintf(stderr, "Input error\n");
    return 1;
}
