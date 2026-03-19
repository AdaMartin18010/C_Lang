/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 1753
 * Language: c
 * Block ID: 339f44d5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：多线程同时写入
void parallel_update_bad(World* world) {
    #pragma omp parallel for
    for (int i = 0; i < world->count; i++) {
        // 多个线程可能同时修改共享的world->score
        world->score += calculate_points(&world->entities[i]);  // 数据竞争！
    }
}

// ✅ 正确：线程本地累加
void parallel_update_good(World* world) {
    int local_scores[8] = {0};  // 每个线程一个

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        #pragma omp for
        for (int i = 0; i < world->count; i++) {
            local_scores[tid] += calculate_points(&world->entities[i]);
        }
    }

    // 主线程合并
    for (int i = 0; i < 8; i++) {
        world->score += local_scores[i];
    }
}

// ✅ 正确：原子操作（用于计数器）
_Atomic int entity_counter = 0;

Entity create_entity_atomic() {
    int id = atomic_fetch_add(&entity_counter, 1);
    return create_entity_with_id(id);
}
