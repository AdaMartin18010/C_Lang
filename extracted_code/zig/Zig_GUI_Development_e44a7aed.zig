//
// Auto-generated from: Zig\Zig_GUI_Development.md
// Line: 500
// Language: zig
// Block ID: e44a7aed
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 自定义按钮控件
pub const CustomButton = struct {
    x: f32, y: f32,
    width: f32, height: f32,
    text: []const u8,

    normal_color: Color,
    hover_color: Color,
    pressed_color: Color,

    is_hovered: bool = false,
    is_pressed: bool = false,
    was_pressed: bool = false,

    pub fn draw(self: *CustomButton, draw_list: *DrawList, mouse_pos: Vec2, mouse_down: bool) void {
        // 检测悬停
        self.is_hovered = mouse_pos.x >= self.x and mouse_pos.x <= self.x + self.width and
                         mouse_pos.y >= self.y and mouse_pos.y <= self.y + self.height;

        // 检测按下
        self.is_pressed = self.is_hovered and mouse_down;

        // 确定颜色
        const color = if (self.is_pressed) self.pressed_color
                     else if (self.is_hovered) self.hover_color
                     else self.normal_color;

        // 绘制背景
        draw_list.addRectFilled(
            .{ self.x, self.y },
            .{ self.x + self.width, self.y + self.height },
            color.toU32(),
            4.0,  // 圆角
        );

        // 绘制边框
        draw_list.addRect(
            .{ self.x, self.y },
            .{ self.x + self.width, self.y + self.height },
            0xFFFFFFFF,
            4.0,
            1.0,
        );

        // 绘制文本
        const text_x = self.x + self.width / 2;
        const text_y = self.y + self.height / 2;
        draw_list.addTextCentered(
            .{ text_x, text_y },
            0xFFFFFFFF,
            self.text,
        );

        // 检测点击
        if (self.was_pressed and !self.is_pressed and self.is_hovered) {
            self.onClick();
        }
        self.was_pressed = self.is_pressed;
    }

    fn onClick(self: *CustomButton) void {
        std.log.info("Button clicked: {s}", .{self.text});
    }
};

// 进度条
pub const ProgressBar = struct {
    x: f32, y: f32,
    width: f32, height: f32,
    progress: f32,  // 0.0 - 1.0

    bg_color: Color = .{ .r = 0.2, .g = 0.2, .b = 0.2, .a = 1 },
    fill_color: Color = .{ .r = 0.2, .g = 0.6, .b = 1, .a = 1 },

    pub fn draw(self: ProgressBar, draw_list: *DrawList) void {
        // 背景
        draw_list.addRectFilled(
            .{ self.x, self.y },
            .{ self.x + self.width, self.y + self.height },
            self.bg_color.toU32(),
            4.0,
        );

        // 填充
        const fill_width = self.width * self.progress;
        draw_list.addRectFilled(
            .{ self.x, self.y },
            .{ self.x + fill_width, self.y + self.height },
            self.fill_color.toU32(),
            4.0,
        );

        // 百分比文本
        const text = std.fmt.bufPrint(&buf, "{d:.0}%", .{self.progress * 100}) catch "";
        draw_list.addTextCentered(
            .{ self.x + self.width / 2, self.y + self.height / 2 },
            0xFFFFFFFF,
            text,
        );
    }
};

var buf: [32]u8 = undefined;
