/*
 * Auto-generated from: 06_Thinking_Representation\04_Application_Scenario_Trees\01_Industry_Application_Scenario_Tree.md
 * Line: 193
 * Language: c
 * Block ID: 5175652a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Entity-Component-System
// 数据导向设计，缓存友好

typedef struct {
    float position[3];
    float velocity[3];
} Transform;

typedef struct {
    Transform *transforms;  // SoA布局
    size_t count;
} World;

void update_positions(World *world, float dt) {
    // 顺序访问，缓存友好
    for (size_t i = 0; i < world->count; i++) {
        Transform *t = &world->transforms[i];
        t->position[0] += t->velocity[0] * dt;
        t->position[1] += t->velocity[1] * dt;
        t->position[2] += t->velocity[2] * dt;
    }
}
