/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 377
 * Language: c
 * Block ID: 60f70659
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 数组初始化规范 */

/* ✅ 合规 [待官方确认] */
int arr1[5] = {1, 2, 3, 4, 5};  /* 完全初始化 */
int arr2[5] = {1, 2};  /* 部分初始化，其余为0 */
int arr3[5] = {0};  /* 全部初始化为0 */

/* ✅ 合规 [待官方确认] - 设计ated初始化 */
int arr4[5] = {[0] = 1, [4] = 5};  /* C99起支持 */

/* ❌ 违反 [待官方确认] - 越界初始化 */
int arr5[3] = {1, 2, 3, 4};  /* 错误：初始化项太多 */
