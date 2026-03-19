/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 164
 * Language: c
 * Block ID: ae4e0fb9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// shape.h - 抽象接口
#ifndef SHAPE_H
#define SHAPE_H

typedef struct Shape Shape;

typedef struct {
    void (*draw)(const Shape* self);
    double (*area)(const Shape* self);
    void (*destroy)(Shape* self);
} ShapeVtbl;

struct Shape {
    const ShapeVtbl* vtbl;
};

// 内联虚函数调用
static inline void shape_draw(const Shape* self) {
    self->vtbl->draw(self);
}

static inline double shape_area(const Shape* self) {
    return self->vtbl->area(self);
}

static inline void shape_destroy(Shape* self) {
    self->vtbl->destroy(self);
}

// 工厂函数
Shape* shape_create_circle(double radius);
Shape* shape_create_rectangle(double width, double height);

#endif
