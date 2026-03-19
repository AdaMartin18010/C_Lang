/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
 * Line: 1217
 * Language: c
 * Block ID: f48a400f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: 虚函数表模拟 (面向对象风格)

// "类" 定义
typedef struct {
    // 虚函数表指针
    const struct VTable* vtable;
    // 实例数据
    int data;
} Shape;

typedef struct VTable {
    double (*area)(const Shape* self);
    void (*draw)(const Shape* self);
    void (*destroy)(Shape* self);
} VTable;

// 实现
double circle_area(const Shape* self) { /* ... */ }
void circle_draw(const Shape* self) { /* ... */ }

static const VTable circle_vtable = {
    .area = circle_area,
    .draw = circle_draw,
    .destroy = /* ... */,
};

// 创建对象
Shape* create_circle(void) {
    Shape* s = malloc(sizeof(Shape));
    s->vtable = &circle_vtable;
    s->data = 0;
    return s;
}

// 虚函数调用 (通过函数指针)
double get_area(const Shape* s) {
    return s->vtable->area(s);
}
