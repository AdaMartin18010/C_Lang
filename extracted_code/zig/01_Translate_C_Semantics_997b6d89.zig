//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 1539
// Language: zig
// Block ID: 997b6d89
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub const GFX_API = {};  // 属性宏通常被简化

pub const Color = extern union {
    value: u32,
    components: extern struct {
        r: u8,
        g: u8,
        b: u8,
        a: u8,
    },
};

pub const Vertex = packed struct {
    x: f32,
    y: f32,
    z: f32,
    u: f32,
    v: f32,
    color: Color,
};

pub const Matrix4x4 = extern struct {
    m: [4][4]f32,
};

pub const RenderCommand = u8;
pub const CMD_DRAW_TRIANGLES: RenderCommand = 0;
pub const CMD_DRAW_LINES: RenderCommand = 1;
pub const CMD_CLEAR: RenderCommand = 2;
pub const CMD_SET_VIEWPORT: RenderCommand = 3;
pub const CMD_MAX: RenderCommand = 4;

pub const RenderState = extern struct {
    flags: u32,
    viewport_x: u16,
    viewport_y: u16,
    viewport_width: u16,
    viewport_height: u16,
    clear_color: [4]f32,
    projection: Matrix4x4,
    view: Matrix4x4,
    model: Matrix4x4,
};

pub const TextureDesc = extern struct {
    width: u32,
    height: u32,
    format: u32,
    mip_levels: u32,
    data: ?*const anyopaque,
};

pub const GraphicsContext = opaque {};

pub extern fn gfx_create_context() ?*GraphicsContext;
pub extern fn gfx_destroy_context(ctx: ?*GraphicsContext) void;
pub extern fn gfx_submit_command(
    ctx: ?*GraphicsContext,
    cmd: RenderCommand,
    data: ?*const anyopaque,
    data_size: usize
) c_int;
