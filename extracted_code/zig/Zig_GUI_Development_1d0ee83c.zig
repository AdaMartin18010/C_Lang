//
// Auto-generated from: Zig\Zig_GUI_Development.md
// Line: 754
// Language: zig
// Block ID: 1d0ee83c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 简单的文本编辑器
pub const TextEditor = struct {
    allocator: std.mem.Allocator,
    buffer: std.ArrayList(u8),
    cursor_pos: usize,

    // 显示
    scroll_x: f32 = 0,
    scroll_y: f32 = 0,
    line_height: f32 = 20,
    char_width: f32 = 10,

    pub fn init(allocator: std.mem.Allocator) TextEditor {
        return .{
            .allocator = allocator,
            .buffer = std.ArrayList(u8).init(allocator),
            .cursor_pos = 0,
        };
    }

    pub fn insert(self: *TextEditor, text: []const u8) !void {
        try self.buffer.insertSlice(self.cursor_pos, text);
        self.cursor_pos += text.len;
    }

    pub fn delete(self: *TextEditor) void {
        if (self.cursor_pos < self.buffer.items.len) {
            _ = self.buffer.orderedRemove(self.cursor_pos);
        }
    }

    pub fn backspace(self: *TextEditor) void {
        if (self.cursor_pos > 0) {
            self.cursor_pos -= 1;
            _ = self.buffer.orderedRemove(self.cursor_pos);
        }
    }

    pub fn moveCursor(self: *TextEditor, delta: i32) void {
        const new_pos = @as(i32, @intCast(self.cursor_pos)) + delta;
        self.cursor_pos = @intCast(std.math.clamp(new_pos, 0, @as(i32, @intCast(self.buffer.items.len))));
    }

    pub fn render(self: TextEditor, draw_list: *DrawList, x: f32, y: f32, width: f32, height: f32) void {
        // 背景
        draw_list.addRectFilled(.{ x, y }, .{ x + width, y + height }, 0xFF1E1E1E, 0);

        // 行号背景
        const line_number_width = 50;
        draw_list.addRectFilled(.{ x, y }, .{ x + line_number_width, y + height }, 0xFF2D2D2D, 0);

        // 计算可见行
        const start_line = @as(usize, @intFromFloat(@floor(self.scroll_y / self.line_height)));
        const visible_lines = @as(usize, @intFromFloat(@ceil(height / self.line_height))) + 1;

        // 渲染文本行
        var line_start: usize = 0;
        var line_idx: usize = 0;

        for (self.buffer.items, 0..) |char, i| {
            if (char == '\n' or i == self.buffer.items.len - 1) {
                if (line_idx >= start_line and line_idx < start_line + visible_lines) {
                    const line_y = y + @as(f32, @floatFromInt(line_idx - start_line)) * self.line_height;

                    // 行号
                    const line_num_text = std.fmt.bufPrint(&buf, "{}", .{line_idx + 1}) catch "";
                    draw_list.addText(
                        .{ x + 5, line_y },
                        0xFF858585,
                        line_num_text,
                    );

                    // 文本内容
                    const line_content = self.buffer.items[line_start..i];
                    draw_list.addText(
                        .{ x + line_number_width + 10, line_y },
                        0xFFD4D4D4,
                        line_content,
                    );

                    // 光标
                    if (self.cursor_pos >= line_start and self.cursor_pos <= i) {
                        const cursor_x = x + line_number_width + 10 +
                            @as(f32, @floatFromInt(self.cursor_pos - line_start)) * self.char_width;
                        draw_list.addRectFilled(
                            .{ cursor_x, line_y },
                            .{ cursor_x + 2, line_y + self.line_height },
                            0xFF007ACC,
                            0,
                        );
                    }
                }

                line_start = i + 1;
                line_idx += 1;
            }
        }
    }
};

var buf: [16]u8 = undefined;
