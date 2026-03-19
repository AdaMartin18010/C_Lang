/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 1071
 * Language: c
 * Block ID: 26bb66cd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 2D太空射击游戏 - ECS完整示例
// ============================================================================

#include "ecs_framework.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

// ============================================================================
// 组件定义
// ============================================================================

typedef enum {
    COMP_TRANSFORM = 0,
    COMP_VELOCITY,
    COMP_SPRITE,
    COMP_HEALTH,
    COMP_PLAYER,
    COMP_ENEMY,
    COMP_BULLET,
    COMP_COLLIDER,
    COMP_SPAWNER,
    COMP_LIFETIME,
    COMP_COUNT
} ComponentTypes;

typedef struct {
    float x, y;
    float rotation;
    float scale;
} Transform;

typedef struct {
    float vx, vy;
} Velocity;

typedef struct {
    uint32_t color;
    float size;
} Sprite;

typedef struct {
    float current;
    float max;
} Health;

typedef struct {
    float speed;
    float fire_rate;
    float fire_cooldown;
    int score;
} Player;

typedef struct {
    float speed;
    int damage;
    uint32_t enemy_type;
} Enemy;

typedef struct {
    float speed;
    int damage;
    bool is_player_bullet;
} Bullet;

typedef struct {
    float radius;
    uint32_t layer;      // 碰撞层
    uint32_t mask;       // 可碰撞的层
} Collider;

typedef struct {
    float interval;
    float timer;
    uint32_t spawn_type;
} Spawner;

typedef struct {
    float remaining;
} Lifetime;

// ============================================================================
// 系统实现
// ============================================================================

// 移动系统
void movement_system(World* world, float dt, void* userdata) {
    (void)userdata;

    ComponentType query_types[] = { COMP_TRANSFORM, COMP_VELOCITY };
    QueryIter iter = world_query(world, query_types, 2);

    Entity e;
    while (query_next(&iter, &e)) {
        Transform* t = world_get_component(world, e, COMP_TRANSFORM);
        Velocity* v = world_get_component(world, e, COMP_VELOCITY);

        t->x += v->vx * dt;
        t->y += v->vy * dt;

        // 边界检查（简单环绕）
        if (t->x < -10.0f) t->x = 10.0f;
        if (t->x > 10.0f) t->x = -10.0f;
        if (t->y < -10.0f) t->y = 10.0f;
        if (t->y > 10.0f) t->y = -10.0f;
    }
}

// 玩家控制系统
void player_control_system(World* world, float dt, void* userdata) {
    (void)userdata;

    ComponentType query_types[] = { COMP_TRANSFORM, COMP_VELOCITY, COMP_PLAYER };
    QueryIter iter = world_query(world, query_types, 3);

    // 模拟输入（实际应从输入系统读取）
    static float input_x = 0.0f;
    static float input_y = 0.0f;
    static bool fire_pressed = false;

    // 模拟输入变化
    input_x = sinf((float)clock() / CLOCKS_PER_SEC) * 0.5f;

    Entity e;
    while (query_next(&iter, &e)) {
        Transform* t = world_get_component(world, e, COMP_TRANSFORM);
        Velocity* v = world_get_component(world, e, COMP_VELOCITY);
        Player* p = world_get_component(world, e, COMP_PLAYER);

        // 移动
        v->vx = input_x * p->speed;
        v->vy = input_y * p->speed;

        // 射击
        p->fire_cooldown -= dt;
        if (fire_pressed && p->fire_cooldown <= 0.0f) {
            p->fire_cooldown = 1.0f / p->fire_rate;

            // 创建子弹
            Entity bullet = world_spawn(world);

            Transform bt = {
                .x = t->x,
                .y = t->y + 0.5f,
                .rotation = 0.0f,
                .scale = 0.2f
            };
            world_add_component(world, bullet, COMP_TRANSFORM, &bt);

            Velocity bv = { .vx = 0.0f, .vy = 10.0f };
            world_add_component(world, bullet, COMP_VELOCITY, &bv);

            Bullet b = { .speed = 10.0f, .damage = 10, .is_player_bullet = true };
            world_add_component(world, bullet, COMP_BULLET, &b);

            Sprite bs = { .color = 0xFFFF00, .size = 0.2f };
            world_add_component(world, bullet, COMP_SPRITE, &bs);

            Collider bc = { .radius = 0.1f, .layer = 0x02, .mask = 0x04 };
            world_add_component(world, bullet, COMP_COLLIDER, &bc);

            Lifetime bl = { .remaining = 2.0f };
            world_add_component(world, bullet, COMP_LIFETIME, &bl);
        }
    }
}

// 敌人AI系统
void enemy_ai_system(World* world, float dt, void* userdata) {
    (void)userdata;

    ComponentType query_types[] = { COMP_TRANSFORM, COMP_VELOCITY, COMP_ENEMY };
    QueryIter iter = world_query(world, query_types, 3);

    Entity e;
    while (query_next(&iter, &e)) {
        Transform* t = world_get_component(world, e, COMP_TRANSFORM);
        Velocity* v = world_get_component(world, e, COMP_VELOCITY);
        Enemy* enemy = world_get_component(world, e, COMP_ENEMY);

        // 简单AI：向下移动并左右摇摆
        v->vy = -enemy->speed;
        v->vx = sinf(t->y * 2.0f) * enemy->speed * 0.5f;
    }
}

// 生成器系统
void spawner_system(World* world, float dt, void* userdata) {
    (void)userdata;

    ComponentType query_types[] = { COMP_TRANSFORM, COMP_SPAWNER };
    QueryIter iter = world_query(world, query_types, 2);

    Entity e;
    while (query_next(&iter, &e)) {
        Transform* t = world_get_component(world, e, COMP_TRANSFORM);
        Spawner* s = world_get_component(world, e, COMP_SPAWNER);

        s->timer -= dt;
        if (s->timer <= 0.0f) {
            s->timer = s->interval;

            // 生成敌人
            Entity enemy = world_spawn(world);

            Transform et = {
                .x = (float)(rand() % 20 - 10),
                .y = 8.0f,
                .rotation = 0.0f,
                .scale = 0.5f
            };
            world_add_component(world, enemy, COMP_TRANSFORM, &et);

            Velocity ev = { 0 };
            world_add_component(world, enemy, COMP_VELOCITY, &ev);

            Enemy enemy_data = { .speed = 2.0f + (rand() % 3), .damage = 10, .enemy_type = s->spawn_type };
            world_add_component(world, enemy, COMP_ENEMY, &enemy_data);

            Health eh = { .current = 30.0f, .max = 30.0f };
            world_add_component(world, enemy, COMP_HEALTH, &eh);

            Sprite es = { .color = 0xFF0000, .size = 0.5f };
            world_add_component(world, enemy, COMP_SPRITE, &es);

            Collider ec = { .radius = 0.4f, .layer = 0x04, .mask = 0x03 };
            world_add_component(world, enemy, COMP_COLLIDER, &ec);
        }
    }
}

// 碰撞检测系统
void collision_system(World* world, float dt, void* userdata) {
    (void)dt;
    (void)userdata;

    // 收集所有碰撞体
    #define MAX_COLLIDERS 1024
    struct { Entity e; Transform* t; Collider* c; } colliders[MAX_COLLIDERS];
    size_t collider_count = 0;

    ComponentType query_types[] = { COMP_TRANSFORM, COMP_COLLIDER };
    QueryIter iter = world_query(world, query_types, 2);

    Entity e;
    while (query_next(&iter, &e)) {
        if (collider_count >= MAX_COLLIDERS) break;
        colliders[collider_count].e = e;
        colliders[collider_count].t = world_get_component(world, e, COMP_TRANSFORM);
        colliders[collider_count].c = world_get_component(world, e, COMP_COLLIDER);
        collider_count++;
    }

    // 简单O(n²)碰撞检测
    for (size_t i = 0; i < collider_count; i++) {
        for (size_t j = i + 1; j < collider_count; j++) {
            Collider* c1 = colliders[i].c;
            Collider* c2 = colliders[j].c;

            // 层检查
            if (!(c1->layer & c2->mask) && !(c2->layer & c1->mask)) continue;

            Transform* t1 = colliders[i].t;
            Transform* t2 = colliders[j].t;

            float dx = t1->x - t2->x;
            float dy = t1->y - t2->y;
            float dist_sq = dx * dx + dy * dy;
            float radius_sum = c1->radius + c2->radius;

            if (dist_sq < radius_sum * radius_sum) {
                // 碰撞发生！
                Entity e1 = colliders[i].e;
                Entity e2 = colliders[j].e;

                // 处理子弹伤害
                Bullet* b1 = world_get_component(world, e1, COMP_BULLET);
                Bullet* b2 = world_get_component(world, e2, COMP_BULLET);
                Health* h1 = world_get_component(world, e1, COMP_HEALTH);
                Health* h2 = world_get_component(world, e2, COMP_HEALTH);

                if (b1 && h2) {
                    h2->current -= b1->damage;
                    world_destroy_entity(world, e1);  // 销毁子弹
                }
                if (b2 && h1) {
                    h1->current -= b2->damage;
                    world_destroy_entity(world, e2);  // 销毁子弹
                }
            }
        }
    }
}

// 生命值系统
void health_system(World* world, float dt, void* userdata) {
    (void)dt;
    (void)userdata;

    ComponentType query_types[] = { COMP_HEALTH };
    QueryIter iter = world_query(world, query_types, 1);

    Entity e;
    while (query_next(&iter, &e)) {
        Health* h = world_get_component(world, e, COMP_HEALTH);

        if (h->current <= 0.0f) {
            // 检查是否是敌人（玩家死亡需要特殊处理）
            if (world_has_component(world, e, COMP_ENEMY)) {
                // 增加玩家分数
                // ...
            }
            world_destroy_entity(world, e);
        }
    }
}

// 生命周期系统
void lifetime_system(World* world, float dt, void* userdata) {
    (void)userdata;

    ComponentType query_types[] = { COMP_LIFETIME };
    QueryIter iter = world_query(world, query_types, 1);

    Entity e;
    while (query_next(&iter, &e)) {
        Lifetime* l = world_get_component(world, e, COMP_LIFETIME);
        l->remaining -= dt;

        if (l->remaining <= 0.0f) {
            world_destroy_entity(world, e);
        }
    }
}

// 渲染系统（简化版，实际应使用图形API）
void render_system(World* world, float dt, void* userdata) {
    (void)dt;
    (void)userdata;

    ComponentType query_types[] = { COMP_TRANSFORM, COMP_SPRITE };
    QueryIter iter = world_query(world, query_types, 2);

    printf("\n=== Frame ===\n");

    Entity e;
    while (query_next(&iter, &e)) {
        Transform* t = world_get_component(world, e, COMP_SPRITE);
        Sprite* s = world_get_component(world, e, COMP_SPRITE);

        const char* type = "Unknown";
        if (world_has_component(world, e, COMP_PLAYER)) type = "Player";
        else if (world_has_component(world, e, COMP_ENEMY)) type = "Enemy";
        else if (world_has_component(world, e, COMP_BULLET)) type = "Bullet";

        printf("%s at (%.2f, %.2f) size=%.2f\n", type, t->x, t->y, s->size);
    }
}

// ============================================================================
// 游戏初始化
// ============================================================================

void init_game(World* world) {
    // 注册组件
    world_register_component(world, COMP_TRANSFORM, sizeof(Transform));
    world_register_component(world, COMP_VELOCITY, sizeof(Velocity));
    world_register_component(world, COMP_SPRITE, sizeof(Sprite));
    world_register_component(world, COMP_HEALTH, sizeof(Health));
    world_register_component(world, COMP_PLAYER, sizeof(Player));
    world_register_component(world, COMP_ENEMY, sizeof(Enemy));
    world_register_component(world, COMP_BULLET, sizeof(Bullet));
    world_register_component(world, COMP_COLLIDER, sizeof(Collider));
    world_register_component(world, COMP_SPAWNER, sizeof(Spawner));
    world_register_component(world, COMP_LIFETIME, sizeof(Lifetime));

    // 创建玩家
    Entity player = world_spawn(world);

    Transform pt = { .x = 0.0f, .y = -5.0f, .rotation = 0.0f, .scale = 1.0f };
    world_add_component(world, player, COMP_TRANSFORM, &pt);

    Velocity pv = { 0 };
    world_add_component(world, player, COMP_VELOCITY, &pv);

    Player p = { .speed = 5.0f, .fire_rate = 5.0f, .fire_cooldown = 0.0f, .score = 0 };
    world_add_component(world, player, COMP_PLAYER, &p);

    Health ph = { .current = 100.0f, .max = 100.0f };
    world_add_component(world, player, COMP_HEALTH, &ph);

    Sprite ps = { .color = 0x00FF00, .size = 0.8f };
    world_add_component(world, player, COMP_SPRITE, &ps);

    Collider pc = { .radius = 0.5f, .layer = 0x01, .mask = 0x04 };
    world_add_component(world, player, COMP_COLLIDER, &pc);

    // 创建生成器
    Entity spawner = world_spawn(world);

    Transform st = { .x = 0.0f, .y = 10.0f, .rotation = 0.0f, .scale = 1.0f };
    world_add_component(world, spawner, COMP_TRANSFORM, &st);

    Spawner s = { .interval = 2.0f, .timer = 0.0f, .spawn_type = 0 };
    world_add_component(world, spawner, COMP_SPAWNER, &s);
}

// ============================================================================
// 主循环
// ============================================================================

int main() {
    srand((unsigned)time(NULL));

    Allocator alloc = default_allocator();
    World* world = world_create(&alloc);
    Scheduler* scheduler = scheduler_create(&alloc);

    init_game(world);

    // 注册系统
    SystemDesc systems[] = {
        { 0, player_control_system, NULL, NULL, 0, NULL, 0, NULL, 0, "PlayerControl" },
        { 0, enemy_ai_system, NULL, NULL, 0, NULL, 0, NULL, 0, "EnemyAI" },
        { 0, movement_system, NULL, NULL, 0, NULL, 0, NULL, 0, "Movement" },
        { 0, spawner_system, NULL, NULL, 0, NULL, 0, NULL, 0, "Spawner" },
        { 0, collision_system, NULL, NULL, 0, NULL, 0, NULL, 0, "Collision" },
        { 0, health_system, NULL, NULL, 0, NULL, 0, NULL, 0, "Health" },
        { 0, lifetime_system, NULL, NULL, 0, NULL, 0, NULL, 0, "Lifetime" },
        { 0, render_system, NULL, NULL, 0, NULL, 0, NULL, 0, "Render" },
    };

    for (size_t i = 0; i < sizeof(systems)/sizeof(systems[0]); i++) {
        scheduler_register(scheduler, &systems[i]);
    }

    // 游戏循环
    float dt = 1.0f / 60.0f;
    for (int frame = 0; frame < 60; frame++) {
        scheduler_run(scheduler, world, dt);

        // 简单帧率控制
        struct timespec ts = { 0, (long)(dt * 1e9 / 10) };  // 加速10倍演示
        nanosleep(&ts, NULL);
    }

    scheduler->alloc->free(scheduler, scheduler->alloc->userdata);
    world_destroy(world);

    printf("\nGame completed!\n");
    return 0;
}
