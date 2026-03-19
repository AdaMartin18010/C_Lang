/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\14_MISRA_C_2023_Rules_14.md
 * Line: 172
 * Language: c
 * Block ID: ad5dd48a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 数组完全初始化 */
void array_safe(void)
{
    int arr[10] = {0};  /* 所有元素初始化为0 */
    int arr2[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};  /* 显式初始化所有 */
}

/* ✅ 合规 - 指定初始化器 (C99) */
void designated_init(void)
{
    int arr[10] = {[0] = 1, [5] = 5, [9] = 9};  /* 其余为0 */

    struct Point p = {.x = 10, .y = 20};  /* 指定成员 */
}

/* ✅ 合规 - 结构体初始化 */
struct Point { int x, y; };

void struct_safe(void)
{
    struct Point p = {0};  /* 所有成员为0 */
    struct Point p2 = {10, 20};  /* 按顺序初始化 */
    struct Point p3 = {.y = 20, .x = 10};  /* 指定顺序 */
}

/* ✅ 合规 - 嵌套结构体初始化 */
struct Rect {
    struct Point top_left;
    struct Point bottom_right;
};

void nested_safe(void)
{
    struct Rect r = {
        .top_left = {.x = 0, .y = 0},
        .bottom_right = {.x = 100, .y = 100}
    };

    struct Rect r2 = {0};  /* 全部清零 */
}

/* ✅ 合规 - 联合体初始化 */
union Data {
    int i;
    float f;
};

void union_init(void)
{
    union Data d = {.i = 10};  /* 初始化第一个成员 */
    union Data d2 = {0};  /* 全部清零 */
}
