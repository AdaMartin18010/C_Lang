/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\README.md
 * Line: 277
 * Language: c
 * Block ID: 1a8efc43
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 匿名结构体成员 (C11)
struct Vector3D {
    union {
        struct { double x, y, z; };  // 匿名结构体
        double coords[3];
    };
};

void vector_demo(void)
{
    struct Vector3D v = { .x = 1.0, .y = 2.0, .z = 3.0 };

    // 两种访问方式
    printf("x = %f\n", v.x);           // 通过命名成员
    printf("x = %f\n", v.coords[0]);   // 通过数组
}

// 标签联合体（类型安全）
enum ShapeType { SHAPE_CIRCLE, SHAPE_RECTANGLE, SHAPE_TRIANGLE };

struct Shape {
    enum ShapeType type;
    union {
        struct { double radius; } circle;
        struct { double width, height; } rectangle;
        struct { double a, b, c; } triangle;
    } data;
};

double shape_area(const struct Shape *s)
{
    switch (s->type) {
        case SHAPE_CIRCLE:
            return 3.14159 * s->data.circle.radius * s->data.circle.radius;
        case SHAPE_RECTANGLE:
            return s->data.rectangle.width * s->data.rectangle.height;
        default:
            return 0.0;
    }
}
