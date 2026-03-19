/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 1038
 * Language: c
 * Block ID: 3563df22
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基类
typedef struct {
    int x, y;
    void (*draw)(void* self);
} Point;

// 派生类 (包含基类作为第一个成员)
typedef struct {
    Point base;  // 必须作为第一个成员
    int radius;
} Circle;

// 多态使用
void draw_any_point(Point* p) {
    p->draw(p);  // 虚函数调用
}

// 安全的向上转型
Circle* circle_from_point(Point* p) {
    return (Circle*)p;  // 因为 Point 是 Circle 的第一个成员
}
