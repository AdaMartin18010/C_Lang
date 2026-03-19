//
// Auto-generated from: Zig\Zig_GUI_Development.md
// Line: 246
// Language: zig
// Block ID: 3bb657b1
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 在 ImGui 中自定义绘制
fn drawCustomWidget(self: *App) void {
    const draw_list = zgui.getWindowDrawList();

    // 获取窗口位置
    const window_pos = zgui.getWindowPos();

    // 绘制矩形
    draw_list.addRect(
        .{ window_pos.x + 10, window_pos.y + 50 },
        .{ window_pos.x + 110, window_pos.y + 150 },
        0xFF00FF00,  // 颜色 (RGBA)
    );

    // 绘制圆形
    draw_list.addCircleFilled(
        .{ window_pos.x + 200, window_pos.y + 100 },
        50,
        0xFFFF0000,
    );

    // 绘制文本
    draw_list.addText(
        .{ window_pos.x + 10, window_pos.y + 200 },
        0xFFFFFFFF,
        "Custom Drawing",
        .{},
    );

    // 绘制线条
    draw_list.addLine(
        .{ window_pos.x + 10, window_pos.y + 250 },
        .{ window_pos.x + 200, window_pos.y + 300 },
        0xFF00FFFF,
        2.0,  // 线宽
    );

    _ = self;
}

// 自定义控件: 旋钮
fn knob(label: [:0]const u8, value: *f32, v_min: f32, v_max: f32) bool {
    const size = 50;
    const center = zgui.getCursorScreenPos();
    center.x += size / 2;
    center.y += size / 2;

    const draw_list = zgui.getWindowDrawList();

    // 背景圆
    draw_list.addCircleFilled(center, size / 2, 0xFF444444);
    draw_list.addCircle(center, size / 2, 0xFF666666);

    // 计算角度
    const t = (value.* - v_min) / (v_max - v_min);
    const angle = -std.math.pi * 0.75 + t * std.math.pi * 1.5;

    // 指示器
    const end_x = center.x + @cos(angle) * (size / 2 - 5);
    const end_y = center.y + @sin(angle) * (size / 2 - 5);

    draw_list.addLine(center, .{ end_x, end_y }, 0xFF00FF00, 3);

    // 标签
    zgui.text(label, .{});

    // 处理输入 (简化)
    if (zgui.isItemActive()) {
        const mouse = zgui.getMousePos();
        const dx = mouse.x - center.x;
        const dy = mouse.y - center.y;
        const new_angle = std.math.atan2(dy, dx);
        const new_t = (new_angle + std.math.pi * 0.75) / (std.math.pi * 1.5);
        value.* = std.math.clamp(v_min + new_t * (v_max - v_min), v_min, v_max);
        return true;
    }

    return false;
}
