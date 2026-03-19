//
// Auto-generated from: Zig\Zig_GUI_Development.md
// Line: 688
// Language: zig
// Block ID: f8e18eed
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 2D 绘图 API
pub const DrawList = struct {
    vertices: std.ArrayList(Vertex),
    indices: std.ArrayList(u32),

    pub const Vertex = struct {
        pos: [2]f32,
        uv: [2]f32,
        color: u32,
    };

    pub fn addRectFilled(self: *DrawList, min: Vec2, max: Vec2, color: u32, rounding: f32) void {
        // 添加四个顶点
        const base_idx = self.vertices.items.len;

        try self.vertices.append(.{
            .pos = .{ min.x, min.y },
            .uv = .{ 0, 0 },
            .color = color,
        });
        try self.vertices.append(.{
            .pos = .{ max.x, min.y },
            .uv = .{ 1, 0 },
            .color = color,
        });
        try self.vertices.append(.{
            .pos = .{ max.x, max.y },
            .uv = .{ 1, 1 },
            .color = color,
        });
        try self.vertices.append(.{
            .pos = .{ min.x, max.y },
            .uv = .{ 0, 1 },
            .color = color,
        });

        // 添加两个三角形
        try self.indices.append(@intCast(base_idx + 0));
        try self.indices.append(@intCast(base_idx + 1));
        try self.indices.append(@intCast(base_idx + 2));
        try self.indices.append(@intCast(base_idx + 0));
        try self.indices.append(@intCast(base_idx + 2));
        try self.indices.append(@intCast(base_idx + 3));
    }

    pub fn addText(self: *DrawList, pos: Vec2, color: u32, text: []const u8) void {
        // 简化实现: 使用纹理字体
        var x = pos.x;
        for (text) |c| {
            // 每个字符一个四边形
            // ...
            x += 8;  // 字符宽度
        }
    }
};

pub const Vec2 = struct {
    x: f32, y: f32,
};
