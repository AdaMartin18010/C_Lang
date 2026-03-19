//
// Auto-generated from: Zig\Zig_Game_Development.md
// Line: 93
// Language: zig
// Block ID: 8f92a422
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

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
