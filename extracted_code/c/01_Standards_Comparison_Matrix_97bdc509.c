/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 738
 * Language: c
 * Block ID: 97bdc509
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    int type;
    union {
        struct {  /* 匿名结构体 */
            int x;
            int y;
        };
        struct {
            int width;
            int height;
        };
        int data[2];
    };
} Point;

int main(void) {
    Point p = { .type = 0, .x = 10, .y = 20 };
    printf("x=%d, y=%d\n", p.x, p.y);  /* 直接访问匿名成员 */
    return 0;
}
