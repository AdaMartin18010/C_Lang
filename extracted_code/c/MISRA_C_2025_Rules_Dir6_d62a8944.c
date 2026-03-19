/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 320
 * Language: c
 * Block ID: d62a8944
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 复合类型（结构体/联合体）使用 */

/* ❌ 违反 [待官方确认] */
struct { int x; int y; } point;  /* 匿名结构体 */

/* ✅ 合规 [待官方确认] */
struct Point { int x; int y; };  /* 命名结构体 */
struct Point point;

/* ✅ 合规 [待官方确认] - typedef */
typedef struct {
    int x;
    int y;
} Point_t;
Point_t point;
