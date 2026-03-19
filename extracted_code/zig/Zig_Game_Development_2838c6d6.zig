//
// Auto-generated from: Zig\Zig_Game_Development.md
// Line: 953
// Language: zig
// Block ID: 2838c6d6
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 简单的太空射击游戏
const std = @import("std");

pub const Game = struct {
    window: Window,
    renderer: Renderer,
    input: *InputManager,

    player: Player,
    bullets: std.ArrayList(Bullet),
    enemies: std.ArrayList(Enemy),

    score: u32,
    game_over: bool,

    last_shot_time: f64,
    enemy_spawn_timer: f64,

    pub fn init(allocator: std.mem.Allocator) !Game {
        const window = try Window.init(800, 600, "Space Shooter");

        return .{
            .window = window,
            .renderer = try Renderer.init(),
            .input = InputManager.getInstance(),
            .player = Player.init(400, 500),
            .bullets = std.ArrayList(Bullet).init(allocator),
            .enemies = std.ArrayList(Enemy).init(allocator),
            .score = 0,
            .game_over = false,
            .last_shot_time = 0,
            .enemy_spawn_timer = 0,
        };
    }

    pub fn run(self: *Game) void {
        var last_time = Window.getTime();

        while (!self.window.shouldClose() and !self.game_over) {
            const current_time = Window.getTime();
            const delta_time = @as(f32, @floatCast(current_time - last_time));
            last_time = current_time;

            self.processInput();
            self.update(delta_time);
            self.render();

            self.window.swapBuffers();
            Window.pollEvents();
        }
    }

    fn processInput(self: *Game) void {
        const input = self.input;

        if (input.isKeyDown(c.GLFW_KEY_A)) {
            self.player.x -= 300 * 0.016;
        }
        if (input.isKeyDown(c.GLFW_KEY_D)) {
            self.player.x += 300 * 0.016;
        }
        if (input.isKeyDown(c.GLFW_KEY_SPACE)) {
            const current_time = Window.getTime();
            if (current_time - self.last_shot_time > 0.2) {
                self.shoot();
                self.last_shot_time = current_time;
            }
        }

        input.update();
    }

    fn shoot(self: *Game) void {
        const bullet = Bullet{
            .x = self.player.x,
            .y = self.player.y - 20,
            .active = true,
        };
        self.bullets.append(bullet) catch {};
    }

    fn update(self: *Game, dt: f32) void {
        // 更新子弹
        for (self.bullets.items) |*bullet| {
            if (!bullet.active) continue;
            bullet.y -= 500 * dt;
            if (bullet.y < 0) bullet.active = false;
        }

        // 生成敌人
        self.enemy_spawn_timer += dt;
        if (self.enemy_spawn_timer > 2.0) {
            const enemy = Enemy{
                .x = @as(f32, @floatFromInt(std.crypto.random.int(u32) % 700)) + 50,
                .y = -30,
                .active = true,
                .hp = 3,
            };
            self.enemies.append(enemy) catch {};
            self.enemy_spawn_timer = 0;
        }

        // 更新敌人
        for (self.enemies.items) |*enemy| {
            if (!enemy.active) continue;
            enemy.y += 100 * dt;

            if (enemy.y > 600) {
                enemy.active = false;
            }

            // 碰撞检测
            for (self.bullets.items) |*bullet| {
                if (!bullet.active) continue;

                const dx = bullet.x - enemy.x;
                const dy = bullet.y - enemy.y;
                const dist = std.math.sqrt(dx * dx + dy * dy);

                if (dist < 30) {
                    enemy.hp -= 1;
                    bullet.active = false;

                    if (enemy.hp <= 0) {
                        enemy.active = false;
                        self.score += 100;
                    }
                }
            }
        }
    }

    fn render(self: Game) void {
        self.renderer.clear(0.1, 0.1, 0.2);

        // 渲染玩家
        self.renderer.drawSprite(self.player.x, self.player.y, 40, 40, .{ .r = 0, .g = 1, .b = 0 });

        // 渲染子弹
        for (self.bullets.items) |bullet| {
            if (!bullet.active) continue;
            self.renderer.drawSprite(bullet.x, bullet.y, 4, 10, .{ .r = 1, .g = 1, .b = 0 });
        }

        // 渲染敌人
        for (self.enemies.items) |enemy| {
            if (!enemy.active) continue;
            self.renderer.drawSprite(enemy.x, enemy.y, 40, 40, .{ .r = 1, .g = 0, .b = 0 });
        }
    }
};

const Player = struct {
    x: f32, y: f32,

    fn init(x: f32, y: f32) Player {
        return .{ .x = x, .y = y };
    }
};

const Bullet = struct {
    x: f32, y: f32,
    active: bool,
};

const Enemy = struct {
    x: f32, y: f32,
    active: bool,
    hp: i32,
};
