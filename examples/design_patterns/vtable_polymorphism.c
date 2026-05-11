/*
 * 虚表多态（C模拟OOP）示例
 * 编译: gcc -Wall -Wextra -o vtable_polymorphism vtable_polymorphism.c
 */
#include <stdio.h>
#include <stdlib.h>

/* 虚函数表 */
typedef struct ShapeVTable ShapeVTable;
struct ShapeVTable {
    double (*area)(const void *self);
    void   (*draw)(const void *self);
    void   (*destroy)(void *self);
};

/* 基类 */
typedef struct {
    const ShapeVTable *vtable;
    int x, y;
} Shape;

double shape_area(const Shape *s) { return s->vtable->area(s); }
void   shape_draw(const Shape *s) { s->vtable->draw(s); }
void   shape_destroy(Shape *s)    { s->vtable->destroy(s); }

/* Circle */
typedef struct {
    Shape base;
    int radius;
} Circle;

static double circle_area(const void *self) {
    const Circle *c = self;
    return 3.14159 * c->radius * c->radius;
}

static void circle_draw(const void *self) {
    const Circle *c = self;
    printf("Circle at (%d,%d) r=%d\n", c->base.x, c->base.y, c->radius);
}

static void circle_destroy(void *self) { free(self); }

static const ShapeVTable circle_vtable = {
    .area = circle_area,
    .draw = circle_draw,
    .destroy = circle_destroy,
};

Shape *circle_create(int x, int y, int r) {
    Circle *c = malloc(sizeof(Circle));
    c->base.vtable = &circle_vtable;
    c->base.x = x; c->base.y = y;
    c->radius = r;
    return (Shape *)c;
}

/* Rectangle */
typedef struct {
    Shape base;
    int width, height;
} Rectangle;

static double rect_area(const void *self) {
    const Rectangle *r = self;
    return r->width * r->height;
}

static void rect_draw(const void *self) {
    const Rectangle *r = self;
    printf("Rect at (%d,%d) %dx%d\n", r->base.x, r->base.y, r->width, r->height);
}

static void rect_destroy(void *self) { free(self); }

static const ShapeVTable rect_vtable = {
    .area = rect_area,
    .draw = rect_draw,
    .destroy = rect_destroy,
};

Shape *rect_create(int x, int y, int w, int h) {
    Rectangle *r = malloc(sizeof(Rectangle));
    r->base.vtable = &rect_vtable;
    r->base.x = x; r->base.y = y;
    r->width = w; r->height = h;
    return (Shape *)r;
}

int main(void) {
    Shape *shapes[2];
    shapes[0] = circle_create(0, 0, 10);
    shapes[1] = rect_create(5, 5, 20, 30);
    
    for (int i = 0; i < 2; i++) {
        shape_draw(shapes[i]);
        printf("  area = %.2f\n", shape_area(shapes[i]));
        shape_destroy(shapes[i]);
    }
    return 0;
}
