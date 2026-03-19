/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 933
 * Language: c
 * Block ID: b1252c47
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 复合类型推导

typedef struct {
    int x;
    int y;
} Point;

typedef union {
    int i;
    float f;
} IntFloat;

typedef enum {
    RED, GREEN, BLUE
} Color;

void c23_composite_typeof(void) {
    Point p = {1, 2};
    IntFloat uf = {.i = 42};
    Color c = RED;

    // 结构体推导
    typeof(p) p2;           // Point (struct {int x; int y;})
    typeof(p.x) px;         // int

    // 联合体推导
    typeof(uf) uf2;         // IntFloat
    typeof(uf.i) fi;        // int
    typeof(uf.f) ff;        // float

    // 枚举推导
    typeof(c) c2;           // Color (enum {RED, GREEN, BLUE})
    typeof(RED) e;          // Color (枚举常量是枚举类型)

    // 复杂嵌套
    struct {
        Point pt;
        Color col;
    } composite;

    typeof(composite) comp2;
    typeof(composite.pt) pt2;   // Point
    typeof(composite.col) col2; // Color
}
