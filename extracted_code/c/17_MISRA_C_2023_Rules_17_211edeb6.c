/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\17_MISRA_C_2023_Rules_17.md
 * Line: 278
 * Language: c
 * Block ID: 211edeb6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 一致使用标签 */
struct Point {
    int x, y;
};

/* 声明 */
struct Point p1;           /* 使用struct标签 */
struct Point make_point(int x, int y);

/* typedef（可选但常见）*/
typedef struct Point Point;
Point p2;                  /* 使用typedef */

/* ✅ 合规 - 前向声明 */
struct Node;  /* 不完整类型 */

struct List {
    struct Node *head;  /* 指向不完整类型 */
};

struct Node {
    int data;
    struct Node *next;
};
