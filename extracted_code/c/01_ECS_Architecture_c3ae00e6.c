/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 167
 * Language: c
 * Block ID: c3ae00e6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ==================== OOP 方式 ====================
// 问题：深度继承，钻石继承，数据分散

class GameObject {
public:
    virtual void update(float dt) = 0;
    virtual ~GameObject() {}
    Transform transform;
};

class PhysicsObject : public GameObject {
public:
    RigidBody body;  // 不是所有对象都需要物理
    void update(float dt) override { /* 物理更新 */ }
};

class Renderable : public PhysicsObject {
public:
    Mesh mesh;       // 不是所有物理对象都需要渲染
    Material mat;
    void update(float dt) override { /* 渲染+物理 */ }
};

// 问题1: 僵尸对象 - 不需要物理的对象仍继承PhysicsObject
// 问题2: 缓存未命中 - 虚表查找 + 数据分散
// 问题3: 无法处理复杂组合 - 既是粒子又是触发器？

// ==================== ECS 方式 ====================
// 解决：扁平组合，数据集中，缓存友好

// 实体只是ID
typedef uint32_t Entity;

// 组件是纯数据结构
struct Position { float x, y, z; };
struct Velocity { float vx, vy, vz; };
struct RigidBody { float mass, drag; };
struct MeshRenderer { MeshHandle mesh; MaterialHandle mat; };

// 系统是纯函数
void physics_system(World* world, float dt) {
    // 只处理有Position+Velocity+RigidBody的实体
    // 数据连续存储，SIMD友好
}

void render_system(World* world, float dt) {
    // 只处理有Position+MeshRenderer的实体
}

// 组合示例：
// Entity e1 = spawn();  // 空实体
// add_component(e1, Position{...});
// add_component(e1, Velocity{...});      // 物理实体
//
// Entity e2 = spawn();
// add_component(e2, Position{...});
// add_component(e2, MeshRenderer{...});  // 静态渲染实体
//
// Entity e3 = spawn();
// add_component(e3, Position{...});
// add_component(e3, Velocity{...});
// add_component(e3, RigidBody{...});
// add_component(e3, MeshRenderer{...});  // 完整物理对象
