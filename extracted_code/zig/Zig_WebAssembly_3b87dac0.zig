//
// Auto-generated from: Zig\Zig_WebAssembly.md
// Line: 552
// Language: zig
// Block ID: 3b87dac0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// game.zig - 简单 WASM 游戏引擎

const std = @import("std");

// 游戏状态
var player_x: f32 = 400;
var player_y: f32 = 300;

const Bullet = struct {
    x: f32,
    y: f32,
    active: bool,
};

var bullets: [100]Bullet = undefined;

// 输入状态 (由 JS 更新)
export var input_up: bool = false;
export var input_down: bool = false;
export var input_left: bool = false;
export var input_right: bool = false;

export fn init() void {
    player_x = 400;
    player_y = 300;
    for (&bullets) |*b| b.active = false;
}

export fn update(dt: f32) void {
    const speed = 300;

    if (input_left) player_x -= speed * dt;
    if (input_right) player_x += speed * dt;
    if (input_up) player_y -= speed * dt;
    if (input_down) player_y += speed * dt;

    player_x = std.math.clamp(player_x, 20, 780);
    player_y = std.math.clamp(player_y, 20, 580);
}

export fn get_player_x() f32 { return player_x; }
export fn get_player_y() f32 { return player_y; }
