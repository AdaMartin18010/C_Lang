/*
 * Auto-generated from: 12_Practice_Exercises\12_Mini_Projects.md
 * Line: 258
 * Language: c
 * Block ID: 51977f5d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单光线追踪
typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

typedef struct {
    bool hit;
    float t;
    Vec3 point;
    Vec3 normal;
    Material *material;
} HitRecord;

Color trace_ray(const Scene *scene, const Ray *ray, int depth);
