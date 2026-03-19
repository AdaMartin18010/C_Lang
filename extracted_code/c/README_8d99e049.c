/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\README.md
 * Line: 367
 * Language: c
 * Block ID: 8d99e049
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基类
// shape.h
typedef struct {
    float x, y;
    void (*draw)(void* self);
    float (*area)(void* self);
} shape_t;

void shape_draw(shape_t* s);
float shape_area(shape_t* s);

// 派生类 - 圆形
// circle.h
typedef struct {
    shape_t base;  // 继承：基类作为第一个成员
    float radius;
} circle_t;

circle_t* circle_create(float x, float y, float r);

// circle.c
static void circle_draw(void* self) {
    circle_t* c = (circle_t*)self;
    printf("Drawing circle at (%.1f, %.1f) with radius %.1f\n",
           c->base.x, c->base.y, c->radius);
}

static float circle_area(void* self) {
    circle_t* c = (circle_t*)self;
    return 3.14159f * c->radius * c->radius;
}

circle_t* circle_create(float x, float y, float r) {
    circle_t* c = malloc(sizeof(circle_t));
    c->base.x = x;
    c->base.y = y;
    c->base.draw = circle_draw;
    c->base.area = circle_area;
    c->radius = r;
    return c;
}
