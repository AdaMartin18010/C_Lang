//
// Auto-generated from: Zig\README.md
// Line: 1651
// Language: zig
// Block ID: d90e688a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 实体组件系统示例
const Entity = u32;
const MAX_ENTITIES = 10000;

fn ComponentStorage(comptime T: type) type {
    return struct {
        data: [MAX_ENTITIES]?T,

        pub fn init() @This() {
            return .{ .data = [_]?T{null} ** MAX_ENTITIES };
        }

        pub fn set(self: *@This(), entity: Entity, component: T) void {
            self.data[entity] = component;
        }

        pub fn get(self: *@This(), entity: Entity) ?*T {
            return if (self.data[entity]) |*c| c else null;
        }

        pub fn has(self: *@This(), entity: Entity) bool {
            return self.data[entity] != null;
        }
    };
}

const Position = struct { x: f32, y: f32 };
const Velocity = struct { vx: f32, vy: f32 };

var positions = ComponentStorage(Position).init();
var velocities = ComponentStorage(Velocity).init();

fn updateMovement(dt: f32) void {
    for (0..MAX_ENTITIES) |entity| {
        if (velocities.has(@intCast(entity)) and
            positions.has(@intCast(entity))) {
            const vel = velocities.get(@intCast(entity)).?;
            const pos = positions.get(@intCast(entity)).?;
            pos.x += vel.vx * dt;
            pos.y += vel.vy * dt;
        }
    }
}
