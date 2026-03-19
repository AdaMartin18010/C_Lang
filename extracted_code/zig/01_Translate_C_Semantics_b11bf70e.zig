//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 1712
// Language: zig
// Block ID: b11bf70e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码

pub const EventType = c_int;
pub const EVENT_NONE: EventType = 0;
pub const EVENT_KEY_PRESS: EventType = 1;
pub const EVENT_KEY_RELEASE: EventType = 2;
pub const EVENT_MOUSE_MOVE: EventType = 3;
pub const EVENT_MOUSE_CLICK: EventType = 4;
pub const EVENT_WINDOW_RESIZE: EventType = 5;
pub const EVENT_CUSTOM: EventType = 100;

pub const EventData = extern union {
    key: extern struct {
        keycode: c_int,
        modifiers: c_int,
    },
    mouse: extern struct {
        x: c_int,
        y: c_int,
        button: c_int,
    },
    window: extern struct {
        width: c_int,
        height: c_int,
    },
    custom: extern struct {
        id: u32,
        data: ?*anyopaque,
    },
};

pub const Event = extern struct {
    type: EventType,
    timestamp: u64,
    data: EventData,
};

pub const EventHandler = ?*const fn ([*c]const Event, ?*anyopaque) callconv(.C) bool;
pub const EventFilter = ?*const fn ([*c]const Event) callconv(.C) bool;

pub const EventSystem = opaque {};

pub extern fn event_system_create() ?*EventSystem;
pub extern fn event_system_destroy(system: ?*EventSystem) void;

pub extern fn event_system_register_handler(
    system: ?*EventSystem,
    event_type: EventType,
    handler: EventHandler,
    user_data: ?*anyopaque,
    priority: c_int
) c_int;

pub extern fn event_system_unregister_handler(
    system: ?*EventSystem,
    handler_id: c_int
) void;

pub extern fn event_system_set_filter(
    system: ?*EventSystem,
    filter: EventFilter
) void;

pub extern fn event_system_dispatch(
    system: ?*EventSystem,
    event: [*c]const Event
) void;

pub extern fn event_system_process_events(system: ?*EventSystem) void;

pub const EventIteratorCallback = ?*const fn ([*c]const Event, c_int, ?*anyopaque) callconv(.C) void;

pub extern fn event_system_foreach_pending(
    system: ?*EventSystem,
    callback: EventIteratorCallback,
    ctx: ?*anyopaque
) void;

pub const HandlerChainNext = ?*const fn (EventHandler, ?*anyopaque) callconv(.C) EventHandler;

pub extern fn event_system_create_handler_chain(
    system: ?*EventSystem,
    next_fn: HandlerChainNext,
    ctx: ?*anyopaque
) EventHandler;

// Zig 友好的包装示例
pub const EventHandlerWrapper = *const fn (*const Event, ?*anyopaque) bool;

pub fn registerHandlerWrapper(
    system: *EventSystem,
    event_type: EventType,
    handler: EventHandlerWrapper,
    user_data: ?*anyopaque,
    priority: i32,
) !i32 {
    const wrapped = struct {
        fn callback(event: [*c]const Event, ctx: ?*anyopaque) callconv(.C) bool {
            const h: EventHandlerWrapper = @ptrCast(@alignCast(ctx));
            return h(&event.*, ctx);
        }
    }.callback;

    const id = event_system_register_handler(
        system,
        event_type,
        wrapped,
        user_data,
        priority
    );
    if (id < 0) return error.RegistrationFailed;
    return id;
}
