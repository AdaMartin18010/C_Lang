/*
 * Auto-generated from: 10_WebAssembly_C\02_Emscripten_Advanced_Usage.md
 * Line: 3033
 * Language: c
 * Block ID: d4140257
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// physics_engine.c
#include <emscripten/emscripten.h>
#include <math.h>

#define MAX_BODIES 1000
#define GRAVITY 9.81f
#define TIME_STEP 0.016f

typedef struct {
    float x, y;        // 位置
    float vx, vy;      // 速度
    float ax, ay;      // 加速度
    float mass;        // 质量
    float radius;      // 半径（用于碰撞）
    int active;        // 是否激活
} RigidBody;

static RigidBody bodies[MAX_BODIES];
static int num_bodies = 0;

// 创建物体
EMSCRIPTEN_KEEPALIVE
int create_body(float x, float y, float mass, float radius) {
    if (num_bodies >= MAX_BODIES) return -1;

    int id = num_bodies++;
    bodies[id].x = x;
    bodies[id].y = y;
    bodies[id].vx = 0;
    bodies[id].vy = 0;
    bodies[id].ax = 0;
    bodies[id].ay = 0;
    bodies[id].mass = mass;
    bodies[id].radius = radius;
    bodies[id].active = 1;

    return id;
}

// 施加力
EMSCRIPTEN_KEEPALIVE
void apply_force(int body_id, float fx, float fy) {
    if (body_id < 0 || body_id >= num_bodies) return;
    if (!bodies[body_id].active) return;

    bodies[body_id].ax += fx / bodies[body_id].mass;
    bodies[body_id].ay += fy / bodies[body_id].mass;
}

// 更新物理
EMSCRIPTEN_KEEPALIVE
void step_physics() {
    // 应用重力
    for (int i = 0; i < num_bodies; i++) {
        if (!bodies[i].active) continue;
        bodies[i].ay += GRAVITY;
    }

    // 更新速度和位置
    for (int i = 0; i < num_bodies; i++) {
        if (!bodies[i].active) continue;

        bodies[i].vx += bodies[i].ax * TIME_STEP;
        bodies[i].vy += bodies[i].ay * TIME_STEP;

        bodies[i].x += bodies[i].vx * TIME_STEP;
        bodies[i].y += bodies[i].vy * TIME_STEP;

        // 阻尼
        bodies[i].vx *= 0.99f;
        bodies[i].vy *= 0.99f;

        // 重置加速度
        bodies[i].ax = 0;
        bodies[i].ay = 0;
    }

    // 简单碰撞检测
    for (int i = 0; i < num_bodies; i++) {
        if (!bodies[i].active) continue;

        for (int j = i + 1; j < num_bodies; j++) {
            if (!bodies[j].active) continue;

            float dx = bodies[j].x - bodies[i].x;
            float dy = bodies[j].y - bodies[i].y;
            float dist = sqrtf(dx * dx + dy * dy);
            float min_dist = bodies[i].radius + bodies[j].radius;

            if (dist < min_dist) {
                // 简单弹性碰撞响应
                float nx = dx / dist;
                float ny = dy / dist;

                float dvx = bodies[j].vx - bodies[i].vx;
                float dvy = bodies[j].vy - bodies[i].vy;

                float vel_along_normal = dvx * nx + dvy * ny;

                if (vel_along_normal > 0) continue;

                float e = 0.8f;  // 弹性系数
                float j_val = -(1 + e) * vel_along_normal;
                j_val /= (1 / bodies[i].mass + 1 / bodies[j].mass);

                float impulse_x = j_val * nx;
                float impulse_y = j_val * ny;

                bodies[i].vx -= impulse_x / bodies[i].mass;
                bodies[i].vy -= impulse_y / bodies[i].mass;
                bodies[j].vx += impulse_x / bodies[j].mass;
                bodies[j].vy += impulse_y / bodies[j].mass;
            }
        }
    }
}

// 获取物体位置
EMSCRIPTEN_KEEPALIVE
void get_body_position(int body_id, float* x, float* y) {
    if (body_id < 0 || body_id >= num_bodies) return;
    *x = bodies[body_id].x;
    *y = bodies[body_id].y;
}

// 设置物体位置
EMSCRIPTEN_KEEPALIVE
void set_body_position(int body_id, float x, float y) {
    if (body_id < 0 || body_id >= num_bodies) return;
    bodies[body_id].x = x;
    bodies[body_id].y = y;
}
