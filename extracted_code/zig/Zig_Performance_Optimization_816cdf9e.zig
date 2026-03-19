//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 252
// Language: zig
// Block ID: 816cdf9e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 面向数据设计: Structure of Arrays (SoA)
// 优于 Array of Structures (AoS)

// ❌ AoS - 缓存不友好
const ParticleAoS = struct {
    x: f32,
    y: f32,
    z: f32,
    vx: f32,
    vy: f32,
    vz: f32,
    mass: f32,
    active: bool,
};

// ✅ SoA - 缓存友好
const ParticleSoA = struct {
    x: []f32,
    y: []f32,
    z: []f32,
    vx: []f32,
    vy: []f32,
    vz: []f32,
    mass: []f32,
    active: []bool,

    pub fn updatePositions(self: ParticleSoA, dt: f32) void {
        // 连续内存访问，缓存命中率高
        for (self.x, self.y, self.z, self.vx, self.vy, self.vz, self.active) |*x, *y, *z, vx, vy, vz, active| {
            if (!active) continue;
            x.* += vx * dt;
            y.* += vy * dt;
            z.* += vz * dt;
        }
    }
};
