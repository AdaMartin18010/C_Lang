/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 1717
 * Language: c
 * Block ID: 7a0e1be0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：每帧都加锁
pthread_mutex_t world_mutex;

void update_system_locked(World* world) {
    pthread_mutex_lock(&world_mutex);
    // 处理所有实体...
    pthread_mutex_unlock(&world_mutex);
}

// 问题：
// - 锁竞争严重
// - 缓存一致性流量
// - 线程串行化

// ✅ 正确：无锁设计或任务分解
// 方案1: 每个线程处理世界的不同部分
void update_system_parallel(World* world, int thread_id, int num_threads) {
    int start = (world->entity_count * thread_id) / num_threads;
    int end = (world->entity_count * (thread_id + 1)) / num_threads;

    // 每个线程只访问自己的范围，无冲突
    for (int i = start; i < end; i++) {
        process(&world->entities[i]);
    }
}

// 方案2: 双缓冲
void update_system_double_buffer(World* read_world, World* write_world) {
    // 读取read_world，写入write_world
    // 无读写冲突
}
