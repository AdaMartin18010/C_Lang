/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 2506
 * Language: c
 * Block ID: 7d014651
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 接口实现（手动 + 宏）
#include <stdio.h>

// 虚表结构（手动实现）
typedef struct {
    void (*draw)(void* self);
    double (*area)(const void* self);
    void (*destroy)(void* self);
} ShapeVtbl;

typedef struct {
    const ShapeVtbl* vtbl;
} Shape;

// 圆形实现
typedef struct {
    Shape base;
    double radius;
} Circle;

void circle_draw(void* self) {
    Circle* c = self;
    printf("Circle(r=%.2f)\n", c->radius);
}

double circle_area(const void* self) {
    const Circle* c = self;
    return 3.14159 * c->radius * c->radius;
}

void circle_destroy(void* self) {
    free(self);
}

constexpr ShapeVtbl CIRCLE_VTBL = {
    .draw = circle_draw,
    .area = circle_area,
    .destroy = circle_destroy
};

Circle* circle_new(double radius) {
    Circle* c = malloc(sizeof(Circle));
    c->base.vtbl = &CIRCLE_VTBL;
    c->radius = radius;
    return c;
}

// 宏简化定义
#define DEFINE_SHAPE_METHODS(Type, draw_fn, area_fn) \
    constexpr ShapeVtbl Type##_VTBL = { \
        .draw = draw_fn, \
        .area = area_fn, \
        .destroy = Type##_destroy \
    }

// 使用
void shape_example(void) {
    Circle* c = circle_new(5.0);
    c->base.vtbl->draw(c);
    printf("Area: %.2f\n", c->base.vtbl->area(c));
    c->base.vtbl->destroy(c);
}
