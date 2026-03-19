//
// Auto-generated from: Zig\Zig_Game_Development.md
// Line: 599
// Language: zig
// Block ID: 2f60fbb8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Box2D C API 绑定
const c = @cImport({
    @cInclude("box2d/box2d.h");
});

pub const World = struct {
    world: *c.b2World,

    pub fn init(gravity: Vec2) World {
        const world = c.b2WorldCreate(&c.b2Vec2{ .x = gravity.x, .y = gravity.y });
        return .{ .world = world };
    }

    pub fn deinit(self: World) void {
        c.b2WorldDestroy(self.world);
    }

    pub fn step(self: World, time_step: f32, velocity_iterations: i32, position_iterations: i32) void {
        c.b2WorldStep(self.world, time_step, velocity_iterations, position_iterations);
    }

    pub fn createBody(self: World, def: BodyDef) Body {
        var b2_def: c.b2BodyDef = .{
            .type = switch (def.body_type) {
                .static => c.b2_staticBody,
                .kinematic => c.b2_kinematicBody,
                .dynamic => c.b2_dynamicBody,
            },
            .position = .{ .x = def.position.x, .y = def.position.y },
            .angle = def.angle,
        };

        const body = c.b2WorldCreateBody(self.world, &b2_def);
        return .{ .body = body };
    }
};

pub const Body = struct {
    body: *c.b2Body,

    pub fn createFixture(self: Body, def: FixtureDef) void {
        var shape: c.b2Shape = undefined;

        switch (def.shape) {
            .box => |box| {
                shape = .{
                    .type = c.b2_polygonShape,
                    .polygon = .{
                        .m_vertices = .{
                            .{ .x = -box.width / 2, .y = -box.height / 2 },
                            .{ .x = box.width / 2, .y = -box.height / 2 },
                            .{ .x = box.width / 2, .y = box.height / 2 },
                            .{ .x = -box.width / 2, .y = box.height / 2 },
                        },
                        .m_count = 4,
                    },
                };
            },
            .circle => |circle| {
                shape = .{
                    .type = c.b2_circleShape,
                    .circle = .{
                        .m_radius = circle.radius,
                        .m_p = .{ .x = 0, .y = 0 },
                    },
                };
            },
        }

        var fixture_def: c.b2FixtureDef = .{
            .shape = &shape,
            .density = def.density,
            .friction = def.friction,
            .restitution = def.restitution,
        };

        _ = c.b2BodyCreateFixture(self.body, &fixture_def);
    }

    pub fn getPosition(self: Body) Vec2 {
        const pos = c.b2BodyGetPosition(self.body);
        return .{ .x = pos.x, .y = pos.y };
    }

    pub fn setLinearVelocity(self: Body, velocity: Vec2) void {
        c.b2BodySetLinearVelocity(self.body, &c.b2Vec2{ .x = velocity.x, .y = velocity.y });
    }
};

pub const BodyDef = struct {
    body_type: BodyType = .static,
    position: Vec2 = .{ .x = 0, .y = 0 },
    angle: f32 = 0,
};

pub const BodyType = enum {
    static,
    kinematic,
    dynamic,
};

pub const FixtureDef = struct {
    shape: Shape,
    density: f32 = 1.0,
    friction: f32 = 0.3,
    restitution: f32 = 0.0,
};

pub const Shape = union(enum) {
    box: BoxShape,
    circle: CircleShape,
};

pub const BoxShape = struct {
    width: f32,
    height: f32,
};

pub const CircleShape = struct {
    radius: f32,
};

pub const Vec2 = struct {
    x: f32, y: f32,
};
