/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 201
 * Language: c
 * Block ID: 26341342
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// circle.c
#include "shape.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct {
    Shape base;
    double radius;
} Circle;

static void circle_draw(const Shape* self) {
    const Circle* c = (const Circle*)self;
    printf("Circle: radius=%.2f\n", c->radius);
}

static double circle_area(const Shape* self) {
    const Circle* c = (const Circle*)self;
    return M_PI * c->radius * c->radius;
}

static void circle_destroy(Shape* self) {
    free(self);
}

static const ShapeVtbl circle_vtbl = {
    .draw = circle_draw,
    .area = circle_area,
    .destroy = circle_destroy
};

Shape* shape_create_circle(double radius) {
    Circle* c = malloc(sizeof(Circle));
    if (c) {
        c->base.vtbl = &circle_vtbl;
        c->radius = radius;
    }
    return (Shape*)c;
}
