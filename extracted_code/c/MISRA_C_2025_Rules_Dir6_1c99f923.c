/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 464
 * Language: c
 * Block ID: 1c99f923
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 运行时初始化 */

/* ❌ 违反 [待官方确认] - 未检查动态分配 */
int *p = malloc(sizeof(int) * n);
p[0] = 0;  /* 如果malloc失败，UB */

/* ✅ 合规 [待官方确认] */
int *p = malloc(sizeof(int) * n);
if (p != NULL) {
    p[0] = 0;  /* 安全使用 */
} else {
    /* 错误处理 */
}
