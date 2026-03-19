/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 1533
 * Language: c
 * Block ID: a591dc6b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：随机访问导致缓存未命中
void update_positions_bad(Entity* entities, int count) {
    for (int i = 0; i < count; i++) {
        // 假设 entities 是随机排序的
        // 每次访问都是不同的内存位置
        entities[i].position.x += entities[i].velocity.x;
        // 缓存未命中！需要等待100-300个时钟周期
    }
}

// ✅ 正确：顺序访问，预取有效
void update_positions_good(Position* positions, Velocity* velocities, int count) {
    for (int i = 0; i < count; i++) {
        // 连续内存访问，缓存预取有效
        positions[i].x += velocities[i].vx;
        // CPU预取器已加载后续数据
    }
}

// 性能对比：
// 缓存命中: ~4 cycles
// 缓存未命中: ~200 cycles
// 加速比: 约50倍
