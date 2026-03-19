/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\01_Creational_Patterns.md
 * Line: 112
 * Language: c
 * Block ID: 2f5d4f16
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 对象类型 */
typedef enum {
    TYPE_CIRCLE,
    TYPE_RECTANGLE,
    TYPE_TRIANGLE
} shape_type_t;

typedef struct shape shape_t;

struct shape {
    shape_type_t type;
    void (*draw)(shape_t*);
    void (*destroy)(shape_t*);
    float area;
};

/* 具体类型 */
typedef struct {
    shape_t base;
    float radius;
} circle_t;

void circle_draw(shape_t* s) {
    circle_t* c = (circle_t*)s;
    printf("Circle: r=%.2f\n", c->radius);
}

float circle_area(shape_t* s) {
    circle_t* c = (circle_t*)s;
    return 3.14159f * c->radius * c->radius;
}

/* 工厂函数 */
shape_t* shape_create(shape_type_t type, ...) {
    va_list args;
    va_start(args, type);

    shape_t* shape = NULL;

    switch (type) {
        case TYPE_CIRCLE: {
            circle_t* c = malloc(sizeof(circle_t));
            c->base.type = TYPE_CIRCLE;
            c->base.draw = circle_draw;
            c->radius = va_arg(args, double);
            c->base.area = circle_area((shape_t*)c);
            shape = (shape_t*)c;
            break;
        }
        /* 其他类型... */
    }

    va_end(args);
    return shape;
}

/* 使用 */
void example_factory(void) {
    shape_t* circle = shape_create(TYPE_CIRCLE, 5.0);
    circle->draw(circle);
    free(circle);
}
