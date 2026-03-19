//
// Auto-generated from: Zig\Zig_Design_Patterns.md
// Line: 43
// Language: zig
// Block ID: fed2224b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 传统 OOP: 对象包含方法和数据
// DoD: 数据连续存储，逻辑分离

// 实体组件系统 (ECS)
const Entity = u32;

const Position = struct {
    x: f32,
    y: f32,
    z: f32,
};

const Velocity = struct {
    vx: f32,
    vy: f32,
    vz: f32,
};

const World = struct {
    positions: std.ArrayList(Position),
    velocities: std.ArrayList(Velocity),
    active: std.ArrayList(bool),

    pub fn init(allocator: std.mem.Allocator) World {
        return .{
            .positions = std.ArrayList(Position).init(allocator),
            .velocities = std.ArrayList(Velocity).init(allocator),
            .active = std.ArrayList(bool).init(allocator),
        };
    }

    pub fn deinit(self: *World) void {
        self.positions.deinit();
        self.velocities.deinit();
        self.active.deinit();
    }

    pub fn createEntity(self: *World, pos: Position, vel: Velocity) !Entity {
        const id = @as(Entity, @intCast(self.positions.items.len));
        try self.positions.append(pos);
        try self.velocities.append(vel);
        try self.active.append(true);
        return id;
    }

    pub fn update(self: *World, dt: f32) void {
        // 连续内存访问，缓存友好
        for (self.positions.items, self.velocities.items, self.active.items) |*pos, vel, active| {
            if (!active) continue;

            pos.x += vel.vx * dt;
            pos.y += vel.vy * dt;
            pos.z += vel.vz * dt;
        }
    }
};
