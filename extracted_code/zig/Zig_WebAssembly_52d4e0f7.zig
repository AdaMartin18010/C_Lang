//
// Auto-generated from: Zig\Zig_WebAssembly.md
// Line: 173
// Language: zig
// Block ID: 52d4e0f7
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// dom.zig - Web API 绑定

// JavaScript 函数声明
extern "env" {
    fn js_console_log(ptr: [*]const u8, len: usize) void;
    fn js_document_get_element_by_id(ptr: [*]const u8, len: usize) u32;
    fn js_element_set_inner_html(element: u32, ptr: [*]const u8, len: usize) void;
    fn js_alert(ptr: [*]const u8, len: usize) void;
}

// Zig 包装函数
pub fn console_log(msg: []const u8) void {
    js_console_log(msg.ptr, msg.len);
}

pub fn alert(msg: []const u8) void {
    js_alert(msg.ptr, msg.len);
}

pub fn getElementById(id: []const u8) u32 {
    return js_document_get_element_by_id(id.ptr, id.len);
}

pub fn setInnerHtml(element: u32, html: []const u8) void {
    js_element_set_inner_html(element, html.ptr, html.len);
}

// 导出初始化函数
export fn init() void {
    console_log("Zig WASM initialized!");
}

export fn update_counter(value: i32) void {
    var buf: [256]u8 = undefined;
    const msg = std.fmt.bufPrint(&buf, "Count: {}", .{value}) catch return;

    const counter = getElementById("counter");
    setInnerHtml(counter, msg);
}
