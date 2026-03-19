/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 1830
 * Language: c
 * Block ID: 1a3c1949
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：系统顺序导致一帧延迟
void game_loop_bad(World* world) {
    render_system(world);       // 渲染旧位置
    movement_system(world);     // 更新位置（下一帧才可见）
}

// ❌ 错误：破坏因果关系
game_loop() {
    collision_system();    // 检测到碰撞，标记待删除
    health_system();       // 根据碰撞减少生命值
    spawn_system();        // 可能生成在即将被删除的位置
    destroy_system();      // 实际删除实体
}

// ✅ 正确：逻辑顺序
enum SystemPhase {
    PHASE_INPUT,        // 读取输入
    PHASE_PRE_UPDATE,   // 预处理
    PHASE_UPDATE,       // 主要逻辑
    PHASE_PHYSICS,      // 物理模拟
    PHASE_COLLISION,    // 碰撞检测
    PHASE_POST_PHYSICS, // 物理后处理
    PHASE_RENDER,       // 渲染准备
    PHASE_CLEANUP       // 清理删除
};

game_loop() {
    input_system();        // 1. 读取输入
    ai_system();           // 2. AI决策
    movement_system();     // 3. 计算移动
    physics_system();      // 4. 物理模拟
    collision_system();    // 5. 碰撞检测
    damage_system();       // 6. 伤害计算
    health_system();       // 7. 生命更新
    death_system();        // 8. 死亡处理（标记删除）
    cleanup_system();      // 9. 清理实体
    render_system();       // 10. 渲染
}
