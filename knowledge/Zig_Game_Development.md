# Zig 游戏开发：从零构建游戏引擎

> **定位**: Zig 应用 / 游戏开发 | **难度**: ⭐⭐⭐⭐⭐ | **目标**: 掌握 Zig 游戏编程

---

## 📋 目录

- [Zig 游戏开发：从零构建游戏引擎](#zig-游戏开发从零构建游戏引擎)
  - [📋 目录](#-目录)
  - [一、游戏架构概述](#一游戏架构概述)
  - [二、窗口与输入](#二窗口与输入)
    - [2.1 GLFW 窗口](#21-glfw-窗口)
  - [三、2D 渲染](#三2d-渲染)
    - [3.1 Sprite 批处理](#31-sprite-批处理)
    - [3.2 纹理管理](#32-纹理管理)
  - [四、3D 图形基础](#四3d-图形基础)
    - [4.1 相机系统](#41-相机系统)
  - [五、物理引擎集成](#五物理引擎集成)
    - [5.1 Box2D 包装](#51-box2d-包装)
  - [六、音频系统](#六音频系统)
    - [6.1 OpenAL 音频](#61-openal-音频)
  - [七、ECS 架构](#七ecs-架构)
  - [八、实战：完整游戏](#八实战完整游戏)

---

## 一、游戏架构概述

```
┌─────────────────────────────────────────────────────────────────────┐
│                    游戏引擎架构                                       │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  游戏逻辑层                                                           │
│  ├── 游戏状态机 (Menu/Playing/Paused)                                │
│  ├── 关卡管理                                                        │
│  └── 存档系统                                                        │
│                                                                      │
│  实体组件系统 (ECS)                                                   │
│  ├── Entity (ID)                                                     │
│  ├── Components (Position/Sprite/Physics)                            │
│  └── Systems (Movement/Render/Collision)                             │
│                                                                      │
│  核心系统层                                                           │
│  ├── 渲染系统 (OpenGL/Vulkan)                                        │
│  ├── 音频系统 (OpenAL)                                               │
│  ├── 物理引擎 (Box2D/Chipmunk)                                       │
│  ├── 输入处理 (键盘/鼠标/手柄)                                        │
│  └── 资源管理 (纹理/模型/音效)                                        │
│                                                                      │
│  平台抽象层                                                           │
│  ├── 窗口管理 (GLFW/SDL)                                             │
│  ├── 文件系统                                                        │
│  └── 网络 (多人游戏)                                                  │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 二、窗口与输入

### 2.1 GLFW 窗口

```zig
const std = @import("std");
const c = @cImport({
    @cInclude("GLFW/glfw3.h");
});

pub const Window = struct {
    handle: *c.GLFWwindow,
    width: i32,
    height: i32,

    pub fn init(width: i32, height: i32, title: [*:0]const u8) !Window {
        if (c.glfwInit() == c.GLFW_FALSE) {
            return error.GlfwInitFailed;
        }

        c.glfwWindowHint(c.GLFW_CONTEXT_VERSION_MAJOR, 3);
        c.glfwWindowHint(c.GLFW_CONTEXT_VERSION_MINOR, 3);
        c.glfwWindowHint(c.GLFW_OPENGL_PROFILE, c.GLFW_OPENGL_CORE_PROFILE);

        const handle = c.glfwCreateWindow(width, height, title, null, null) orelse {
            return error.WindowCreationFailed;
        };

        c.glfwMakeContextCurrent(handle);
        c.glfwSwapInterval(1);  // VSync

        // 设置回调
        c.glfwSetKeyCallback(handle, keyCallback);
        c.glfwSetCursorPosCallback(handle, mouseCallback);

        return .{
            .handle = handle,
            .width = width,
            .height = height,
        };
    }

    pub fn deinit(self: Window) void {
        c.glfwDestroyWindow(self.handle);
        c.glfwTerminate();
    }

    pub fn shouldClose(self: Window) bool {
        return c.glfwWindowShouldClose(self.handle) == c.GLFW_TRUE;
    }

    pub fn swapBuffers(self: Window) void {
        c.glfwSwapBuffers(self.handle);
    }

    pub fn pollEvents() void {
        c.glfwPollEvents();
    }

    pub fn getTime() f64 {
        return c.glfwGetTime();
    }
};

// 输入回调
export fn keyCallback(window: ?*c.GLFWwindow, key: c_int, scancode: c_int, action: c_int, mods: c_int) void {
    _ = window;
    _ = scancode;
    _ = mods;

    const input = InputManager.getInstance();

    if (action == c.GLFW_PRESS) {
        input.setKeyDown(@intCast(key));
    } else if (action == c.GLFW_RELEASE) {
        input.setKeyUp(@intCast(key));
    }
}

export fn mouseCallback(window: ?*c.GLFWwindow, xpos: f64, ypos: f64) void {
    _ = window;

    const input = InputManager.getInstance();
    input.setMousePosition(xpos, ypos);
}

// 输入管理器
pub const InputManager = struct {
    keys: [512]bool,
    prev_keys: [512]bool,
    mouse_x: f64,
    mouse_y: f64,

    var instance: InputManager = .{
        .keys = [_]bool{false} ** 512,
        .prev_keys = [_]bool{false} ** 512,
        .mouse_x = 0,
        .mouse_y = 0,
    };

    pub fn getInstance() *InputManager {
        return &instance;
    }

    pub fn setKeyDown(self: *InputManager, key: usize) void {
        if (key < 512) self.keys[key] = true;
    }

    pub fn setKeyUp(self: *InputManager, key: usize) void {
        if (key < 512) self.keys[key] = false;
    }

    pub fn isKeyDown(self: InputManager, key: c_int) bool {
        const idx = @as(usize, @intCast(key));
        return if (idx < 512) self.keys[idx] else false;
    }

    pub fn isKeyPressed(self: *InputManager, key: c_int) bool {
        const idx = @as(usize, @intCast(key));
        if (idx >= 512) return false;

        const pressed = self.keys[idx] and !self.prev_keys[idx];
        return pressed;
    }

    pub fn update(self: *InputManager) void {
        @memcpy(&self.prev_keys, &self.keys);
    }

    pub fn setMousePosition(self: *InputManager, x: f64, y: f64) void {
        self.mouse_x = x;
        self.mouse_y = y;
    }
};
```

---

## 三、2D 渲染

### 3.1 Sprite 批处理

```zig
const std = @import("std");
const c = @cImport({
    @cInclude("GL/glew.h");
});

pub const SpriteBatch = struct {
    const MAX_SPRITES = 1000;
    const VERTICES_PER_SPRITE = 4;
    const INDICES_PER_SPRITE = 6;

    vao: c_uint,
    vbo: c_uint,
    ebo: c_uint,
    shader: Shader,

    vertices: [MAX_SPRITES * VERTICES_PER_SPRITE * 4]f32,
    vertex_count: usize,

    texture_ids: [16]c_uint,
    texture_count: usize,

    pub fn init(shader: Shader) SpriteBatch {
        var vao: c_uint = 0;
        var vbo: c_uint = 0;
        var ebo: c_uint = 0;

        c.glGenVertexArrays(1, &vao);
        c.glGenBuffers(1, &vbo);
        c.glGenBuffers(1, &ebo);

        c.glBindVertexArray(vao);

        // 顶点缓冲
        c.glBindBuffer(c.GL_ARRAY_BUFFER, vbo);
        c.glBufferData(c.GL_ARRAY_BUFFER, @sizeOf(f32) * MAX_SPRITES * VERTICES_PER_SPRITE * 4, null, c.GL_DYNAMIC_DRAW);

        // 位置 (2 floats)
        c.glEnableVertexAttribArray(0);
        c.glVertexAttribPointer(0, 2, c.GL_FLOAT, c.GL_FALSE, 4 * @sizeOf(f32), null);

        // 纹理坐标 (2 floats)
        c.glEnableVertexAttribArray(1);
        c.glVertexAttribPointer(1, 2, c.GL_FLOAT, c.GL_FALSE, 4 * @sizeOf(f32), @ptrFromInt(2 * @sizeOf(f32)));

        // 索引缓冲
        var indices: [MAX_SPRITES * INDICES_PER_SPRITE]c_uint = undefined;
        var i: usize = 0;
        var offset: c_uint = 0;
        while (i < MAX_SPRITES) : (i += 1) {
            indices[i * 6 + 0] = offset + 0;
            indices[i * 6 + 1] = offset + 1;
            indices[i * 6 + 2] = offset + 2;
            indices[i * 6 + 3] = offset + 2;
            indices[i * 6 + 4] = offset + 3;
            indices[i * 6 + 5] = offset + 0;
            offset += 4;
        }

        c.glBindBuffer(c.GL_ELEMENT_ARRAY_BUFFER, ebo);
        c.glBufferData(c.GL_ELEMENT_ARRAY_BUFFER, @sizeOf(c_uint) * indices.len, &indices, c.GL_STATIC_DRAW);

        c.glBindVertexArray(0);

        return .{
            .vao = vao,
            .vbo = vbo,
            .ebo = ebo,
            .shader = shader,
            .vertices = undefined,
            .vertex_count = 0,
            .texture_ids = undefined,
            .texture_count = 0,
        };
    }

    pub fn begin(self: *SpriteBatch) void {
        self.vertex_count = 0;
        self.texture_count = 0;
    }

    pub fn draw(self: *SpriteBatch, region: TextureRegion, x: f32, y: f32, width: f32, height: f32) void {
        if (self.vertex_count >= MAX_SPRITES * VERTICES_PER_SPRITE * 4) {
            self.flush();
            self.begin();
        }

        const x1 = x;
        const y1 = y;
        const x2 = x + width;
        const y2 = y + height;

        const u1 = region.u;
        const v1 = region.v;
        const u2 = region.u + region.w;
        const v2 = region.v + region.h;

        // 4 个顶点 (x, y, u, v)
        const verts = [_]f32{
            x1, y1, u1, v1,  // 左上
            x2, y1, u2, v1,  // 右上
            x2, y2, u2, v2,  // 右下
            x1, y2, u1, v2,  // 左下
        };

        @memcpy(self.vertices[self.vertex_count..][0..verts.len], &verts);
        self.vertex_count += verts.len;
    }

    pub fn end(self: *SpriteBatch) void {
        self.flush();
    }

    fn flush(self: *SpriteBatch) void {
        if (self.vertex_count == 0) return;

        self.shader.use();

        // 绑定纹理
        for (0..self.texture_count) |i| {
            c.glActiveTexture(c.GL_TEXTURE0 + @as(c_uint, @intCast(i)));
            c.glBindTexture(c.GL_TEXTURE_2D, self.texture_ids[i]);
        }

        // 更新顶点数据
        c.glBindBuffer(c.GL_ARRAY_BUFFER, self.vbo);
        c.glBufferSubData(
            c.GL_ARRAY_BUFFER,
            0,
            @as(c_long, @intCast(@sizeOf(f32) * self.vertex_count)),
            &self.vertices,
        );

        // 绘制
        c.glBindVertexArray(self.vao);
        const sprite_count = self.vertex_count / (VERTICES_PER_SPRITE * 4);
        c.glDrawElements(c.GL_TRIANGLES, @intCast(sprite_count * INDICES_PER_SPRITE), c.GL_UNSIGNED_INT, null);
        c.glBindVertexArray(0);
    }
};

pub const TextureRegion = struct {
    u: f32, v: f32,
    w: f32, h: f32,
};

pub const Shader = struct {
    id: c_uint,

    pub fn use(self: Shader) void {
        c.glUseProgram(self.id);
    }
};
```

### 3.2 纹理管理

```zig
pub const Texture = struct {
    id: c_uint,
    width: i32,
    height: i32,

    pub fn loadFromFile(path: []const u8) !Texture {
        // 使用 stb_image 加载
        var width: c_int = 0;
        var height: c_int = 0;
        var channels: c_int = 0;

        const data = c.stbi_load(path.ptr, &width, &height, &channels, 4);
        defer c.stbi_image_free(data);

        if (data == null) return error.ImageLoadFailed;

        var id: c_uint = 0;
        c.glGenTextures(1, &id);
        c.glBindTexture(c.GL_TEXTURE_2D, id);

        // 设置纹理参数
        c.glTexParameteri(c.GL_TEXTURE_2D, c.GL_TEXTURE_WRAP_S, c.GL_REPEAT);
        c.glTexParameteri(c.GL_TEXTURE_2D, c.GL_TEXTURE_WRAP_T, c.GL_REPEAT);
        c.glTexParameteri(c.GL_TEXTURE_2D, c.GL_TEXTURE_MIN_FILTER, c.GL_LINEAR_MIPMAP_LINEAR);
        c.glTexParameteri(c.GL_TEXTURE_2D, c.GL_TEXTURE_MAG_FILTER, c.GL_LINEAR);

        c.glTexImage2D(c.GL_TEXTURE_2D, 0, c.GL_RGBA, width, height, 0, c.GL_RGBA, c.GL_UNSIGNED_BYTE, data);
        c.glGenerateMipmap(c.GL_TEXTURE_2D);

        return .{
            .id = id,
            .width = width,
            .height = height,
        };
    }

    pub fn bind(self: Texture, slot: c_uint) void {
        c.glActiveTexture(c.GL_TEXTURE0 + slot);
        c.glBindTexture(c.GL_TEXTURE_2D, self.id);
    }

    pub fn getRegion(self: Texture, x: i32, y: i32, w: i32, h: i32) TextureRegion {
        const fw = @as(f32, @floatFromInt(self.width));
        const fh = @as(f32, @floatFromInt(self.height));

        return .{
            .u = @as(f32, @floatFromInt(x)) / fw,
            .v = @as(f32, @floatFromInt(y)) / fh,
            .w = @as(f32, @floatFromInt(w)) / fw,
            .h = @as(f32, @floatFromInt(h)) / fh,
        };
    }
};
```

---

## 四、3D 图形基础

### 4.1 相机系统

```zig
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
```

---

## 五、物理引擎集成

### 5.1 Box2D 包装

```zig
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
```

---

## 六、音频系统

### 6.1 OpenAL 音频

```zig
const c = @cImport({
    @cInclude("AL/al.h");
    @cInclude("AL/alc.h");
});

pub const AudioSystem = struct {
    device: *c.ALCdevice,
    context: *c.ALCcontext,

    pub fn init() !AudioSystem {
        const device = c.alcOpenDevice(null) orelse return error.OpenDeviceFailed;
        const context = c.alcCreateContext(device, null) orelse return error.CreateContextFailed;

        c.alcMakeContextCurrent(context);

        return .{
            .device = device,
            .context = context,
        };
    }

    pub fn deinit(self: AudioSystem) void {
        c.alcMakeContextCurrent(null);
        c.alcDestroyContext(self.context);
        c.alcCloseDevice(self.device);
    }
};

pub const Sound = struct {
    buffer: c_uint,
    source: c_uint,

    pub fn loadFromFile(path: []const u8) !Sound {
        // 加载 WAV/OGG 文件
        // 简化示例
        var buffer: c_uint = 0;
        c.alGenBuffers(1, &buffer);

        // 解码音频数据并填充缓冲
        // ...

        var source: c_uint = 0;
        c.alGenSources(1, &source);
        c.alSourcei(source, c.AL_BUFFER, @intCast(buffer));

        return .{ .buffer = buffer, .source = source };
    }

    pub fn play(self: Sound) void {
        c.alSourcePlay(self.source);
    }

    pub fn stop(self: Sound) void {
        c.alSourceStop(self.source);
    }

    pub fn setVolume(self: Sound, volume: f32) void {
        c.alSourcef(self.source, c.AL_GAIN, volume);
    }

    pub fn setPosition(self: Sound, x: f32, y: f32, z: f32) void {
        c.alSource3f(self.source, c.AL_POSITION, x, y, z);
    }

    pub fn deinit(self: Sound) void {
        c.alDeleteSources(1, &self.source);
        c.alDeleteBuffers(1, &self.buffer);
    }
};
```

---

## 七、ECS 架构

```zig
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
```

---

## 八、实战：完整游戏

```zig
// 简单的太空射击游戏
const std = @import("std");

pub const Game = struct {
    window: Window,
    renderer: Renderer,
    input: *InputManager,

    player: Player,
    bullets: std.ArrayList(Bullet),
    enemies: std.ArrayList(Enemy),

    score: u32,
    game_over: bool,

    last_shot_time: f64,
    enemy_spawn_timer: f64,

    pub fn init(allocator: std.mem.Allocator) !Game {
        const window = try Window.init(800, 600, "Space Shooter");

        return .{
            .window = window,
            .renderer = try Renderer.init(),
            .input = InputManager.getInstance(),
            .player = Player.init(400, 500),
            .bullets = std.ArrayList(Bullet).init(allocator),
            .enemies = std.ArrayList(Enemy).init(allocator),
            .score = 0,
            .game_over = false,
            .last_shot_time = 0,
            .enemy_spawn_timer = 0,
        };
    }

    pub fn run(self: *Game) void {
        var last_time = Window.getTime();

        while (!self.window.shouldClose() and !self.game_over) {
            const current_time = Window.getTime();
            const delta_time = @as(f32, @floatCast(current_time - last_time));
            last_time = current_time;

            self.processInput();
            self.update(delta_time);
            self.render();

            self.window.swapBuffers();
            Window.pollEvents();
        }
    }

    fn processInput(self: *Game) void {
        const input = self.input;

        if (input.isKeyDown(c.GLFW_KEY_A)) {
            self.player.x -= 300 * 0.016;
        }
        if (input.isKeyDown(c.GLFW_KEY_D)) {
            self.player.x += 300 * 0.016;
        }
        if (input.isKeyDown(c.GLFW_KEY_SPACE)) {
            const current_time = Window.getTime();
            if (current_time - self.last_shot_time > 0.2) {
                self.shoot();
                self.last_shot_time = current_time;
            }
        }

        input.update();
    }

    fn shoot(self: *Game) void {
        const bullet = Bullet{
            .x = self.player.x,
            .y = self.player.y - 20,
            .active = true,
        };
        self.bullets.append(bullet) catch {};
    }

    fn update(self: *Game, dt: f32) void {
        // 更新子弹
        for (self.bullets.items) |*bullet| {
            if (!bullet.active) continue;
            bullet.y -= 500 * dt;
            if (bullet.y < 0) bullet.active = false;
        }

        // 生成敌人
        self.enemy_spawn_timer += dt;
        if (self.enemy_spawn_timer > 2.0) {
            const enemy = Enemy{
                .x = @as(f32, @floatFromInt(std.crypto.random.int(u32) % 700)) + 50,
                .y = -30,
                .active = true,
                .hp = 3,
            };
            self.enemies.append(enemy) catch {};
            self.enemy_spawn_timer = 0;
        }

        // 更新敌人
        for (self.enemies.items) |*enemy| {
            if (!enemy.active) continue;
            enemy.y += 100 * dt;

            if (enemy.y > 600) {
                enemy.active = false;
            }

            // 碰撞检测
            for (self.bullets.items) |*bullet| {
                if (!bullet.active) continue;

                const dx = bullet.x - enemy.x;
                const dy = bullet.y - enemy.y;
                const dist = std.math.sqrt(dx * dx + dy * dy);

                if (dist < 30) {
                    enemy.hp -= 1;
                    bullet.active = false;

                    if (enemy.hp <= 0) {
                        enemy.active = false;
                        self.score += 100;
                    }
                }
            }
        }
    }

    fn render(self: Game) void {
        self.renderer.clear(0.1, 0.1, 0.2);

        // 渲染玩家
        self.renderer.drawSprite(self.player.x, self.player.y, 40, 40, .{ .r = 0, .g = 1, .b = 0 });

        // 渲染子弹
        for (self.bullets.items) |bullet| {
            if (!bullet.active) continue;
            self.renderer.drawSprite(bullet.x, bullet.y, 4, 10, .{ .r = 1, .g = 1, .b = 0 });
        }

        // 渲染敌人
        for (self.enemies.items) |enemy| {
            if (!enemy.active) continue;
            self.renderer.drawSprite(enemy.x, enemy.y, 40, 40, .{ .r = 1, .g = 0, .b = 0 });
        }
    }
};

const Player = struct {
    x: f32, y: f32,

    fn init(x: f32, y: f32) Player {
        return .{ .x = x, .y = y };
    }
};

const Bullet = struct {
    x: f32, y: f32,
    active: bool,
};

const Enemy = struct {
    x: f32, y: f32,
    active: bool,
    hp: i32,
};
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10
> - 依赖: GLFW3, OpenGL, Box2D, OpenAL
