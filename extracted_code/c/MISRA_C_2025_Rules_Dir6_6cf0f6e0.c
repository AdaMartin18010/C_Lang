/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 400
 * Language: c
 * Block ID: 6cf0f6e0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 结构体初始化规范 */

struct Point {
    int x;
    int y;
};

/* ✅ 合规 [待官方确认] */
struct Point p1 = {10, 20};  /* 顺序初始化 */
struct Point p2 = {.x = 10, .y = 20};  /* 命名初始化 */
struct Point p3 = {.x = 10};  /* 部分初始化，y为0 */
struct Point p4 = {0};  /* 全部初始化为0 */

/* ✅ 合规 [待官方确认] - 嵌套结构体 */
struct Rect {
    struct Point top_left;
    struct Point bottom_right;
};
struct Rect r = {{0, 0}, {100, 100}};
