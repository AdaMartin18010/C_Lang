//
// Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\01_Zig_C_Interoperability.md
// Line: 969
// Language: zig
// Block ID: c82c037e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// gl.zig - OpenGL Zig 绑定

const std = @import("std");
const c = @cImport({
    @cInclude("GL/glew.h");
    @cInclude("GLFW/glfw3.h");
});

// 类型定义
pub const GLenum = c_uint;
pub const GLuint = c_uint;
pub const GLint = c_int;
pub const GLfloat = f32;
pub const GLbitfield = c_uint;
pub const GLsizei = c_int;

// 常量
pub const GL_TRIANGLES = 0x0004;
pub const GL_VERTEX_SHADER = 0x8B31;
pub const GL_FRAGMENT_SHADER = 0x8B30;
pub const GL_COMPILE_STATUS = 0x8B81;
pub const GL_LINK_STATUS = 0x8B82;
pub const GL_COLOR_BUFFER_BIT = 0x00004000;

// 窗口管理
pub const Window = struct {
    handle: *c.GLFWwindow,

    pub fn create(width: i32, height: i32, title: [*:0]const u8) !Window {
        _ = c.glfwInit();

        c.glfwWindowHint(c.GLFW_CONTEXT_VERSION_MAJOR, 3);
        c.glfwWindowHint(c.GLFW_CONTEXT_VERSION_MINOR, 3);
        c.glfwWindowHint(c.GLFW_OPENGL_PROFILE, c.GLFW_OPENGL_CORE_PROFILE);

        const handle = c.glfwCreateWindow(width, height, title, null, null) orelse {
            return error.WindowCreationFailed;
        };

        c.glfwMakeContextCurrent(handle);
        _ = c.glewInit();

        return .{ .handle = handle };
    }

    pub fn destroy(self: Window) void {
        c.glfwDestroyWindow(self.handle);
        c.glfwTerminate();
    }

    pub fn shouldClose(self: Window) bool {
        return c.glfwWindowShouldClose(self.handle) != 0;
    }

    pub fn swapBuffers(self: Window) void {
        c.glfwSwapBuffers(self.handle);
    }

    pub fn pollEvents() void {
        c.glfwPollEvents();
    }

    pub fn setKeyCallback(self: Window, callback: fn (*c.GLFWwindow, c_int, c_int, c_int, c_int) callconv(.C) void) void {
        c.glfwSetKeyCallback(self.handle, callback);
    }
};

// 着色器程序
pub const Shader = struct {
    id: GLuint,

    pub fn createVertex(source: []const u8) !Shader {
        return create(GL_VERTEX_SHADER, source);
    }

    pub fn createFragment(source: []const u8) !Shader {
        return create(GL_FRAGMENT_SHADER, source);
    }

    fn create(shader_type: GLenum, source: []const u8) !Shader {
        const id = c.glCreateShader(shader_type);

        const source_ptr: [*c]const u8 = source.ptr;
        const len: c_int = @intCast(source.len);
        c.glShaderSource(id, 1, &source_ptr, &len);
        c.glCompileShader(id);

        var success: c_int = 0;
        c.glGetShaderiv(id, GL_COMPILE_STATUS, &success);

        if (success == 0) {
            var log: [512]u8 = undefined;
            c.glGetShaderInfoLog(id, 512, null, &log);
            std.log.err("Shader compilation failed: {s}", .{std.mem.sliceTo(&log, 0)});
            return error.ShaderCompilationFailed;
        }

        return .{ .id = id };
    }

    pub fn destroy(self: Shader) void {
        c.glDeleteShader(self.id);
    }
};

pub const ShaderProgram = struct {
    id: GLuint,

    pub fn create(vertex: Shader, fragment: Shader) !ShaderProgram {
        const id = c.glCreateProgram();
        c.glAttachShader(id, vertex.id);
        c.glAttachShader(id, fragment.id);
        c.glLinkProgram(id);

        var success: c_int = 0;
        c.glGetProgramiv(id, GL_LINK_STATUS, &success);

        if (success == 0) {
            var log: [512]u8 = undefined;
            c.glGetProgramInfoLog(id, 512, null, &log);
            std.log.err("Program linking failed: {s}", .{std.mem.sliceTo(&log, 0)});
            return error.ProgramLinkingFailed;
        }

        return .{ .id = id };
    }

    pub fn use(self: ShaderProgram) void {
        c.glUseProgram(self.id);
    }

    pub fn destroy(self: ShaderProgram) void {
        c.glDeleteProgram(self.id);
    }

    pub fn setFloat(self: ShaderProgram, name: [*:0]const u8, value: f32) void {
        const location = c.glGetUniformLocation(self.id, name);
        c.glUniform1f(location, value);
    }
};

// 缓冲区
pub const VBO = struct {
    id: GLuint,

    pub fn create() VBO {
        var id: GLuint = 0;
        c.glGenBuffers(1, &id);
        return .{ .id = id };
    }

    pub fn bind(self: VBO) void {
        c.glBindBuffer(c.GL_ARRAY_BUFFER, self.id);
    }

    pub fn unbind() void {
        c.glBindBuffer(c.GL_ARRAY_BUFFER, 0);
    }

    pub fn bufferData(self: VBO, data: []const f32, usage: GLenum) void {
        self.bind();
        c.glBufferData(c.GL_ARRAY_BUFFER, @intCast(data.len * @sizeOf(f32)), data.ptr, usage);
    }

    pub fn destroy(self: VBO) void {
        c.glDeleteBuffers(1, &self.id);
    }
};

// 渲染函数
pub fn clearColor(r: f32, g: f32, b: f32, a: f32) void {
    c.glClearColor(r, g, b, a);
}

pub fn clear(mask: GLbitfield) void {
    c.glClear(mask);
}

pub fn drawArrays(mode: GLenum, first: GLint, count: GLsizei) void {
    c.glDrawArrays(mode, first, count);
}

// 渲染循环示例
pub fn render_loop() !void {
    const window = try Window.create(800, 600, "Zig OpenGL");
    defer window.destroy();

    const vertex_shader = try Shader.createVertex(
        \\#version 330 core
        \\layout (location = 0) in vec3 aPos;
        \\void main() {
        \\    gl_Position = vec4(aPos, 1.0);
        \\}
    );
    defer vertex_shader.destroy();

    const fragment_shader = try Shader.createFragment(
        \\#version 330 core
        \\out vec4 FragColor;
        \\void main() {
        \\    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
        \\}
    );
    defer fragment_shader.destroy();

    const program = try ShaderProgram.create(vertex_shader, fragment_shader);
    defer program.destroy();

    // 顶点数据
    const vertices = [_]f32{
        -0.5, -0.5, 0.0,
         0.5, -0.5, 0.0,
         0.0,  0.5, 0.0,
    };

    const vbo = VBO.create();
    defer vbo.destroy();
    vbo.bufferData(&vertices, c.GL_STATIC_DRAW);

    // 渲染循环
    while (!window.shouldClose()) {
        clearColor(0.2, 0.3, 0.3, 1.0);
        clear(GL_COLOR_BUFFER_BIT);

        program.use();
        c.glBindVertexArray(vbo.id);
        drawArrays(GL_TRIANGLES, 0, 3);

        window.swapBuffers();
        Window.pollEvents();
    }
}
