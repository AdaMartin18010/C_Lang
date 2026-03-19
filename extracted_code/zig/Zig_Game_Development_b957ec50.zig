//
// Auto-generated from: Zig\Zig_Game_Development.md
// Line: 809
// Language: zig
// Block ID: b957ec50
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 简化的 ECS 实现
const std = @import("std");

pub const Entity = u32;
pub const MAX_ENTITIES = 10000;
pub const MAX_COMPONENTS = 32;

// 组件类型 ID
pub var next_component_id: u8 = 0;

pub fn ComponentId(comptime T: type) u8 {
    comptime {
        const id = next_component_id;
        next_component_id += 1;
        return id;
    }
}

// 签名类型
pub const Signature = std.bit_set.IntegerBitSet(MAX_COMPONENTS);

pub const EntityManager = struct {
    available_entities: std.ArrayList(Entity),
    signatures: [MAX_ENTITIES]Signature,
    living_entity_count: u32,

    pub fn init(allocator: std.mem.Allocator) EntityManager {
        var available = std.ArrayList(Entity).init(allocator);

        // 初始化可用实体池
        var i: u32 = MAX_ENTITIES - 1;
        while (i >= 0) : (i -= 1) {
            available.append(i) catch break;
            if (i == 0) break;
        }

        return .{
            .available_entities = available,
            .signatures = [_]Signature{Signature.initEmpty()} ** MAX_ENTITIES,
            .living_entity_count = 0,
        };
    }

    pub fn createEntity(self: *EntityManager) !Entity {
        if (self.living_entity_count >= MAX_ENTITIES) return error.TooManyEntities;

        const id = self.available_entities.pop();
        self.living_entity_count += 1;
        return id;
    }

    pub fn destroyEntity(self: *EntityManager, entity: Entity) !void {
        if (entity >= MAX_ENTITIES) return error.InvalidEntity;

        self.signatures[entity] = Signature.initEmpty();
        try self.available_entities.append(entity);
        self.living_entity_count -= 1;
    }

    pub fn setSignature(self: *EntityManager, entity: Entity, signature: Signature) void {
        self.signatures[entity] = signature;
    }

    pub fn getSignature(self: EntityManager, entity: Entity) Signature {
        return self.signatures[entity];
    }
};

// 组件数组 (稀疏集)
pub fn ComponentArray(comptime T: type) type {
    return struct {
        component_data: [MAX_ENTITIES]T,
        entity_to_index: [MAX_ENTITIES]u32,
        index_to_entity: [MAX_ENTITIES]Entity,
        size: u32,

        pub fn init() @This() {
            return .{
                .component_data = undefined,
                .entity_to_index = [_]u32{0} ** MAX_ENTITIES,
                .index_to_entity = [_]Entity{0} ** MAX_ENTITIES,
                .size = 0,
            };
        }

        pub fn insert(self: *@This(), entity: Entity, component: T) void {
            const new_index = self.size;
            self.entity_to_index[entity] = new_index;
            self.index_to_entity[new_index] = entity;
            self.component_data[new_index] = component;
            self.size += 1;
        }

        pub fn remove(self: *@This(), entity: Entity) void {
            const index_of_removed = self.entity_to_index[entity];
            const index_of_last = self.size - 1;

            self.component_data[index_of_removed] = self.component_data[index_of_last];
            const entity_of_last = self.index_to_entity[index_of_last];
            self.entity_to_index[entity_of_last] = index_of_removed;
            self.index_to_entity[index_of_removed] = entity_of_last;

            self.entity_to_index[entity] = 0;
            self.size -= 1;
        }

        pub fn get(self: *@This(), entity: Entity) *T {
            return &self.component_data[self.entity_to_index[entity]];
        }
    };
}

// 系统接口
pub const System = struct {
    entities: std.ArrayList(Entity),
    signature: Signature,

    pub fn init(allocator: std.mem.Allocator, signature: Signature) System {
        return .{
            .entities = std.ArrayList(Entity).init(allocator),
            .signature = signature,
        };
    }

    pub fn onEntityAdded(self: *System, entity: Entity) !void {
        try self.entities.append(entity);
    }

    pub fn onEntityRemoved(self: *System, entity: Entity) void {
        for (self.entities.items, 0..) |e, i| {
            if (e == entity) {
                _ = self.entities.swapRemove(i);
                return;
            }
        }
    }
};
