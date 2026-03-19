/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\16_MISRA_C_2023_Rules_16.md
 * Line: 45
 * Language: c
 * Block ID: 0cabdd24
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 原型声明 */
void func(int a, int b);

void call(void)
{
    func(1, 2);  /* 匹配 */
}

/* ✅ 合规 - 使用结构体传递多参数 */
typedef struct {
    int x;
    int y;
    int width;
    int height;
} Rect;

void draw_rect(const Rect *r);

/* 使用 */
Rect r = {.x = 0, .y = 0, .width = 100, .height = 100};
draw_rect(&r);
