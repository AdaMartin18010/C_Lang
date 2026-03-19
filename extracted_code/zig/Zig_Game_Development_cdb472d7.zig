//
// Auto-generated from: Zig\Zig_Game_Development.md
// Line: 231
// Language: zig
// Block ID: cdb472d7
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

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
