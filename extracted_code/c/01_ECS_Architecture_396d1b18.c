/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 1793
 * Language: c
 * Block ID: 396d1b18
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：隐式依赖
void update_physics(World* world) {
    // 隐式假设Transform已在上一帧更新
    // 如果系统顺序改变，行为异常
    for (Entity e : query<Position, Velocity>()) {
        e.position += e.velocity * dt;
    }
}

void update_transform(World* world) {
    // 隐式依赖父实体的Transform
    for (Entity e : query<Hierarchy, Transform>()) {
        e.world_transform = e.parent->world_transform * e.local_transform;
    }
}

// ✅ 正确：显式依赖声明
SystemDesc physics_system = {
    .name = "Physics",
    .writes = { COMP_TRANSFORM },
    .reads = { COMP_VELOCITY, COMP_TRANSFORM },
    .depends_on = { /* 明确依赖 */ }
};

SystemDesc transform_system = {
    .name = "TransformUpdate",
    .writes = { COMP_WORLD_TRANSFORM },
    .reads = { COMP_LOCAL_TRANSFORM, COMP_PARENT },
    .depends_on = { /* 确保父先更新 */ }
};

// 调度器根据依赖构建执行图
