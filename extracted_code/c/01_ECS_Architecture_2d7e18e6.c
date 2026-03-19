/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 1934
 * Language: c
 * Block ID: 2d7e18e6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 技巧1: 数据对齐
#define ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))

struct Position {
    float x, y, z;
    float padding;  // 对齐到16字节，SIMD友好
} __attribute__((aligned(16)));

// 技巧2: 冷热数据分离
typedef struct {
    // 热数据（每帧访问）
    struct {
        float x, y, z;
    } position;

    struct {
        float vx, vy, vz;
    } velocity;
} TransformHot;

typedef struct {
    // 冷数据（偶尔访问）
    char name[64];
    uint64_t creation_time;
    void* user_data;
} TransformCold;

// 技巧3: 软件预取
void update_with_prefetch(Position* pos, Velocity* vel, int count) {
    for (int i = 0; i < count; i++) {
        // 预取未来8个迭代的数据
        if (i + 8 < count) {
            __builtin_prefetch(&pos[i + 8], 1, 3);  // 写，高时间局部性
            __builtin_prefetch(&vel[i + 8], 0, 3);  // 读，高时间局部性
        }

        pos[i].x += vel[i].vx * dt;
    }
}

// 技巧4: 循环展开
void update_unrolled(Position* pos, Velocity* vel, int count) {
    int i = 0;
    // 每次处理4个
    for (; i + 4 <= count; i += 4) {
        pos[i+0].x += vel[i+0].vx * dt;
        pos[i+1].x += vel[i+1].vx * dt;
        pos[i+2].x += vel[i+2].vx * dt;
        pos[i+3].x += vel[i+3].vx * dt;
    }
    // 处理剩余
    for (; i < count; i++) {
        pos[i].x += vel[i].vx * dt;
    }
}

// 技巧5: 分块处理（提高缓存命中率）
#define BLOCK_SIZE 1024  // 适合L1缓存大小

void update_blocked(World* world) {
    int entity_count = world->entity_count;

    for (int block_start = 0; block_start < entity_count; block_start += BLOCK_SIZE) {
        int block_end = (block_start + BLOCK_SIZE < entity_count)
                       ? block_start + BLOCK_SIZE
                       : entity_count;

        // 整个块的数据都在缓存中
        for (int i = block_start; i < block_end; i++) {
            process_entity(&world->entities[i]);
        }
    }
}
