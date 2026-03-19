/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\02_Structural_Patterns.md
 * Line: 384
 * Language: c
 * Block ID: ffe81253
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 实现接口 */
typedef struct {
    void (*draw_line)(void*, int, int, int, int);
    void (*draw_circle)(void*, int, int, int);
    void (*render)(void*);
} renderer_impl_t;

/* 抽象 */
typedef struct {
    renderer_impl_t* impl;
    void (*draw_shape)(void*);
} shape_t;

/* 细化抽象: 圆形 */
typedef struct {
    shape_t base;
    int x, y, radius;
} circle_shape_t;

void circle_draw(void* self) {
    circle_shape_t* circle = self;
    circle->base.impl->draw_circle(circle->base.impl,
                                   circle->x, circle->y, circle->radius);
}

/* 实现: OpenGL */
typedef struct {
    renderer_impl_t interface;
    GLuint program;
} opengl_renderer_t;

void opengl_draw_circle(void* self, int x, int y, int r) {
    opengl_renderer_t* gl = self;
    /* OpenGL绘制 */
}

/* 实现: DirectX */
typedef struct {
    renderer_impl_t interface;
    ID3D11Device* device;
} d3d_renderer_t;

void d3d_draw_circle(void* self, int x, int y, int r) {
    d3d_renderer_t* d3d = self;
    /* DirectX绘制 */
}

/* 桥接: 抽象与实现分离 */
void example_bridge(void) {
    /* 可以用OpenGL或DirectX渲染相同的形状 */
    renderer_impl_t* gl = opengl_renderer_create();
    renderer_impl_t* d3d = d3d_renderer_create();

    circle_shape_t* circle = circle_create(100, 100, 50);

    circle->base.impl = gl;
    circle->base.draw_shape(circle);  /* OpenGL渲染 */

    circle->base.impl = d3d;
    circle->base.draw_shape(circle);  /* DirectX渲染 */
}
