/*
 * Auto-generated from: 12_Practice_Exercises\12_Mini_Projects.md
 * Line: 245
 * Language: c
 * Block ID: cd5523c6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 2D渲染
void draw_line(FrameBuffer *fb, int x0, int y0, int x1, int y1, Color c);
void draw_triangle(FrameBuffer *fb, Vec2 v[3], Color c);
void fill_triangle(FrameBuffer *fb, Vec2 v[3], Color c);

// 3D渲染（软光栅）
void draw_mesh(FrameBuffer *fb, const Mesh *mesh,
               const Mat4 *mvp, const Camera *cam);
