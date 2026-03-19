//
// Auto-generated from: Zig\Zig_GUI_Development.md
// Line: 610
// Language: zig
// Block ID: 2fa799e8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 事件系统
pub const Event = union(enum) {
    mouse_move: MouseMoveEvent,
    mouse_button: MouseButtonEvent,
    key: KeyEvent,
    resize: ResizeEvent,

    pub const MouseMoveEvent = struct {
        x: f32, y: f32,
        dx: f32, dy: f32,
    };

    pub const MouseButtonEvent = struct {
        button: MouseButton,
        pressed: bool,
        x: f32, y: f32,
    };

    pub const KeyEvent = struct {
        key: KeyCode,
        pressed: bool,
        modifiers: Modifiers,
    };

    pub const ResizeEvent = struct {
        width: i32, height: i32,
    };
};

pub const EventDispatcher = struct {
    listeners: std.ArrayList(EventListener),

    pub fn init(allocator: std.mem.Allocator) EventDispatcher {
        return .{
            .listeners = std.ArrayList(EventListener).init(allocator),
        };
    }

    pub fn addListener(self: *EventDispatcher, listener: EventListener) !void {
        try self.listeners.append(listener);
    }

    pub fn dispatch(self: EventDispatcher, event: Event) void {
        for (self.listeners.items) |listener| {
            if (listener.filter(event)) {
                listener.callback(event);
            }
        }
    }
};

pub const EventListener = struct {
    callback: *const fn (Event) void,
    filter: *const fn (Event) bool,
};

// 使用示例
fn onMouseClick(event: Event) void {
    switch (event) {
        .mouse_button => |e| {
            if (e.pressed) {
                std.log.info("Clicked at {}, {}", .{ e.x, e.y });
            }
        },
        else => {},
    }
}

fn mouseFilter(event: Event) bool {
    return event == .mouse_button;
}
