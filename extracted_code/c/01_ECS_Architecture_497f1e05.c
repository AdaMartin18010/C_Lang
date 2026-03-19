/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 1561
 * Language: c
 * Block ID: 497f1e05
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：虚函数导致分支预测失败和间接跳转
class Component {
public:
    virtual void update(float dt) = 0;  // 虚函数表查找
    virtual ~Component() {}
};

class PositionComponent : public Component {
public:
    void update(float dt) override { x += vx * dt; }
    float x, y, vx, vy;
};

void update_all(std::vector<Component*>& components, float dt) {
    for (auto* c : components) {
        c->update(dt);  // 虚函数调用：
                        // 1. 加载虚表指针 (可能缓存未命中)
                        // 2. 加载函数地址
                        // 3. 间接跳转 (分支预测失败)
    }
}

// ✅ 正确：直接函数调用，SIMD友好
void update_positions_batch(Position* pos, Velocity* vel, int count, float dt) {
    // 编译器可内联、向量化
    for (int i = 0; i < count; i++) {
        pos[i].x += vel[i].vx * dt;
        pos[i].y += vel[i].vy * dt;
    }
}
