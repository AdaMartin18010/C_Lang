/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 2219
 * Language: c
 * Block ID: 5e79c7d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 渲染批处理：合并相同材质的绘制调用
typedef struct {
    Material material;
    Mesh mesh;
    Transform* transforms;
    int count;
    int capacity;
} RenderBatch;

typedef struct {
    RenderBatch* batches;
    int batch_count;
} BatchRenderer;

void batch_renderer_submit(BatchRenderer* renderer, Entity e, Material mat, Mesh mesh, Transform* t) {
    // 查找或创建批次
    RenderBatch* batch = find_or_create_batch(renderer, mat, mesh);

    if (batch->count >= batch->capacity) {
        batch->capacity *= 2;
        batch->transforms = realloc(batch->transforms, sizeof(Transform) * batch->capacity);
    }

    batch->transforms[batch->count++] = *t;
}

void batch_renderer_flush(BatchRenderer* renderer) {
    for (int i = 0; i < renderer->batch_count; i++) {
        RenderBatch* batch = &renderer->batches[i];
        if (batch->count == 0) continue;

        // 一次性提交整个批次
        graphics_draw_instanced(
            batch->mesh,
            batch->material,
            batch->transforms,
            batch->count
        );

        batch->count = 0;  // 重置
    }
}

// 物理批处理：SIMD友好的碰撞检测
void broadphase_batch(BroadPhase* bp, AABB* boxes, int count) {
    // 排序（如SAH）
    sort_by_morton_code(boxes, count);

    // 批量检测
    for (int i = 0; i < count; i += 4) {
        __m128 x_min = _mm_loadu_ps(&boxes[i].min_x);
        __m128 x_max = _mm_loadu_ps(&boxes[i].max_x);
        // ... SIMD批量AABB测试
    }
}
