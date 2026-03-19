/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 1632
 * Language: c
 * Block ID: 96642675
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：过度泛化的通用组件系统
typedef struct {
    void* data;
    size_t size;
    ComponentType type;
    void (*serialize)(void* data, FILE* f);
    void (*deserialize)(void* data, FILE* f);
    void (*copy)(void* dst, const void* src);
    // ... 更多虚函数
} GenericComponent;

// 问题：
// 1. 每个组件都有函数指针开销
// 2. 无法内联优化
// 3. 数据分散，缓存不友好
// 4. 调试困难

// ✅ 正确：明确类型，编译期确定
struct Position { float x, y, z; };
struct Velocity { float vx, vy, vz; };

// 序列化函数显式定义
void serialize_position(const Position* p, FILE* f) {
    fwrite(p, sizeof(Position), 1, f);  // 可内联，可优化
}

// 模板/宏生成重复代码，但性能最优
#define DEFINE_COMPONENT_STORAGE(TYPE) \
    struct TYPE##Storage { \
        TYPE* data; \
        Entity* entities; \
        size_t count; \
    }
