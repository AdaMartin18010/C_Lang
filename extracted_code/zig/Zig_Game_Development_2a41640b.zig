//
// Auto-generated from: Zig\Zig_Game_Development.md
// Line: 450
// Language: zig
// Block ID: 2a41640b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const math = std.math;

pub const Camera = struct {
    position: Vec3,
    front: Vec3,
    up: Vec3,
    right: Vec3,
    world_up: Vec3,

    yaw: f32,
    pitch: f32,

    movement_speed: f32,
    mouse_sensitivity: f32,
    zoom: f32,

    pub fn init(position: Vec3) Camera {
        const cam = Camera{
            .position = position,
            .front = .{ .x = 0, .y = 0, .z = -1 },
            .up = .{ .x = 0, .y = 1, .z = 0 },
            .right = undefined,
            .world_up = .{ .x = 0, .y = 1, .z = 0 },
            .yaw = -90,
            .pitch = 0,
            .movement_speed = 2.5,
            .mouse_sensitivity = 0.1,
            .zoom = 45,
        };
        return cam.updateCameraVectors();
    }

    pub fn getViewMatrix(self: Camera) Mat4 {
        return lookAt(self.position, self.position.add(self.front), self.up);
    }

    pub fn processKeyboard(self: *Camera, direction: CameraMovement, delta_time: f32) void {
        const velocity = self.movement_speed * delta_time;

        switch (direction) {
            .FORWARD => self.position = self.position.add(self.front.scale(velocity)),
            .BACKWARD => self.position = self.position.sub(self.front.scale(velocity)),
            .LEFT => self.position = self.position.sub(self.right.scale(velocity)),
            .RIGHT => self.position = self.position.add(self.right.scale(velocity)),
        }
    }

    pub fn processMouseMovement(self: *Camera, xoffset: f32, yoffset: f32, constrain_pitch: bool) void {
        const xo = xoffset * self.mouse_sensitivity;
        const yo = yoffset * self.mouse_sensitivity;

        self.yaw += xo;
        self.pitch += yo;

        if (constrain_pitch) {
            if (self.pitch > 89) self.pitch = 89;
            if (self.pitch < -89) self.pitch = -89;
        }

        self.updateCameraVectors();
    }

    fn updateCameraVectors(self: Camera) Camera {
        var cam = self;

        // 计算新的 front 向量
        const yaw_rad = toRadians(cam.yaw);
        const pitch_rad = toRadians(cam.pitch);

        cam.front.x = math.cos(yaw_rad) * math.cos(pitch_rad);
        cam.front.y = math.sin(pitch_rad);
        cam.front.z = math.sin(yaw_rad) * math.cos(pitch_rad);
        cam.front = cam.front.normalize();

        // 重新计算 right 和 up
        cam.right = cam.front.cross(cam.world_up).normalize();
        cam.up = cam.right.cross(cam.front).normalize();

        return cam;
    }
};

pub const CameraMovement = enum {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

pub const Vec3 = struct {
    x: f32, y: f32, z: f32,

    pub fn add(self: Vec3, other: Vec3) Vec3 {
        return .{ .x = self.x + other.x, .y = self.y + other.y, .z = self.z + other.z };
    }

    pub fn sub(self: Vec3, other: Vec3) Vec3 {
        return .{ .x = self.x - other.x, .y = self.y - other.y, .z = self.z - other.z };
    }

    pub fn scale(self: Vec3, s: f32) Vec3 {
        return .{ .x = self.x * s, .y = self.y * s, .z = self.z * s };
    }

    pub fn cross(self: Vec3, other: Vec3) Vec3 {
        return .{
            .x = self.y * other.z - self.z * other.y,
            .y = self.z * other.x - self.x * other.z,
            .z = self.x * other.y - self.y * other.x,
        };
    }

    pub fn normalize(self: Vec3) Vec3 {
        const len = math.sqrt(self.x * self.x + self.y * self.y + self.z * self.z);
        return .{ .x = self.x / len, .y = self.y / len, .z = self.z / len };
    }
};

pub const Mat4 = [16]f32;

fn lookAt(eye: Vec3, center: Vec3, up: Vec3) Mat4 {
    const f = center.sub(eye).normalize();
    const s = f.cross(up).normalize();
    const u = s.cross(f);

    return .{
        s.x,  u.x,  -f.x, 0,
        s.y,  u.y,  -f.y, 0,
        s.z,  u.z,  -f.z, 0,
        -s.x * eye.x - s.y * eye.y - s.z * eye.z,
        -u.x * eye.x - u.y * eye.y - u.z * eye.z,
        f.x * eye.x + f.y * eye.y + f.z * eye.z,
        1,
    };
}

fn toRadians(degrees: f32) f32 {
    return degrees * math.pi / 180.0;
}
