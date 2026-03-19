//
// Auto-generated from: Zig\Zig_WebAssembly.md
// Line: 486
// Language: zig
// Block ID: 4455e47b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// image.zig - 图像处理 WASM 库

const std = @import("std");

pub const Image = extern struct {
    width: u32,
    height: u32,
    data: [*]u8,  // RGBA 数据

    pub fn get_pixel(self: Image, x: u32, y: u32) [4]u8 {
        const idx = (y * self.width + x) * 4;
        return .{
            self.data[idx],
            self.data[idx + 1],
            self.data[idx + 2],
            self.data[idx + 3],
        };
    }

    pub fn set_pixel(self: Image, x: u32, y: u32, r: u8, g: u8, b: u8, a: u8) void {
        const idx = (y * self.width + x) * 4;
        self.data[idx] = r;
        self.data[idx + 1] = g;
        self.data[idx + 2] = b;
        self.data[idx + 3] = a;
    }
};

// 灰度转换
export fn grayscale(image: Image) void {
    var y: u32 = 0;
    while (y < image.height) : (y += 1) {
        var x: u32 = 0;
        while (x < image.width) : (x += 1) {
            const pixel = image.get_pixel(x, y);
            const gray = @as(u8, @intFromFloat(
                0.299 * @as(f32, @floatFromInt(pixel[0])) +
                0.587 * @as(f32, @floatFromInt(pixel[1])) +
                0.114 * @as(f32, @floatFromInt(pixel[2]))
            ));
            image.set_pixel(x, y, gray, gray, gray, pixel[3]);
        }
    }
}

// 亮度调整
export fn adjust_brightness(image: Image, factor: f32) void {
    var y: u32 = 0;
    while (y < image.height) : (y += 1) {
        var x: u32 = 0;
        while (x < image.width) : (x += 1) {
            const pixel = image.get_pixel(x, y);
            const r = @min(255, @as(u32, @intFromFloat(@as(f32, @floatFromInt(pixel[0])) * factor)));
            const g = @min(255, @as(u32, @intFromFloat(@as(f32, @floatFromInt(pixel[1])) * factor)));
            const b = @min(255, @as(u32, @intFromFloat(@as(f32, @floatFromInt(pixel[2])) * factor)));
            image.set_pixel(x, y, @intCast(r), @intCast(g), @intCast(b), pixel[3]);
        }
    }
}
