/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 243
 * Language: c
 * Block ID: 101d0f42
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// shape_factory.c - 使用示例
#include "shape.h"
#include <stdio.h>

typedef enum {
    SHAPE_CIRCLE,
    SHAPE_RECTANGLE
} ShapeType;

Shape* shape_factory_create(ShapeType type, ...) {
    // 简化的工厂函数
    switch (type) {
        case SHAPE_CIRCLE: {
            double radius = 5.0; // 实际应从可变参数获取
            return shape_create_circle(radius);
        }
        case SHAPE_RECTANGLE: {
            double w = 4.0, h = 6.0;
            return shape_create_rectangle(w, h);
        }
        default:
            return NULL;
    }
}

// 使用
void factory_example(void) {
    Shape* shapes[2];
    shapes[0] = shape_create_circle(5.0);
    shapes[1] = shape_create_rectangle(4.0, 6.0);

    for (int i = 0; i < 2; i++) {
        shape_draw(shapes[i]);
        printf("Area: %.2f\n", shape_area(shapes[i]));
        shape_destroy(shapes[i]);
    }
}
