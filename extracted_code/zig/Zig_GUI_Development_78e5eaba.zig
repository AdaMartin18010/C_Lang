//
// Auto-generated from: Zig\Zig_GUI_Development.md
// Line: 83
// Language: zig
// Block ID: 78e5eaba
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const zgui = @import("zgui");
const glfw = @import("glfw");

pub const App = struct {
    window: *glfw.Window,

    // 应用状态
    counter: i32 = 0,
    text_buffer: [256]u8 = [_]u8{0} ** 256,
    show_demo: bool = false,
    color: [4]f32 = .{ 1, 0, 0, 1 },

    pub fn init() !App {
        // 初始化 GLFW
        _ = glfw.init(.{});

        const window = glfw.Window.create(1280, 720, "Zig GUI App", null, null, .{
            .context_version_major = 3,
            .context_version_minor = 3,
            .opengl_profile = .opengl_core_profile,
        }).?;

        glfw.makeContextCurrent(window);
        glfw.swapInterval(1);

        // 初始化 ImGui
        zgui.init();
        zgui.backend.init(window);

        // 加载字体
        _ = zgui.io.addFontFromFile("assets/Roboto-Regular.ttf", 16.0);

        return .{ .window = window };
    }

    pub fn run(self: *App) void {
        while (!self.window.shouldClose()) {
            glfw.pollEvents();

            zgui.backend.newFrame(self.window);

            self.drawUI();

            const io = zgui.io;
            const clear_color = [4]f32{ 0.45, 0.55, 0.60, 1.00 };

            glfw.makeContextCurrent(self.window);
            glfw.backend.clear(clear_color);
            zgui.render();
            zgui.backend.render();

            self.window.swapBuffers();
        }
    }

    fn drawUI(self: *App) void {
        // 主窗口
        if (zgui.begin("Hello, Zig!", .{ .popen = &true })) {
            zgui.text("This is some useful text.", .{});

            // 按钮
            if (zgui.button("Counter: {d}", .{self.counter})) {
                self.counter += 1;
            }

            zgui.sameLine(.{});
            if (zgui.button("Reset", .{})) {
                self.counter = 0;
            }

            // 输入框
            _ = zgui.inputText("Input", .{
                .buf = &self.text_buffer,
            });

            // 颜色选择
            _ = zgui.colorEdit4("Color", .{ .col = &self.color });

            // 滑块
            _ = zgui.sliderInt("Value", .{
                .v = &self.counter,
                .v_min = 0,
                .v_max = 100,
            });

            zgui.end();
        }

        // 菜单栏
        if (zgui.beginMainMenuBar()) {
            if (zgui.beginMenu("File", true)) {
                if (zgui.menuItem("New", .{})) {
                    self.newFile();
                }
                if (zgui.menuItem("Open", .{})) {
                    self.openFile();
                }
                zgui.separator();
                if (zgui.menuItem("Exit", .{})) {
                    self.window.setShouldClose(true);
                }
                zgui.endMenu();
            }

            if (zgui.beginMenu("View", true)) {
                _ = zgui.menuItem("Demo Window", .{ .selected = &self.show_demo });
                zgui.endMenu();
            }

            zgui.endMainMenuBar();
        }

        // Demo 窗口
        if (self.show_demo) {
            zgui.showDemoWindow(&self.show_demo);
        }

        // 浮动窗口
        if (zgui.begin("Tools", .{})) {
            if (zgui.collapsingHeader("Transform", .{})) {
                // 变换控制
                var position = [3]f32{ 0, 0, 0 };
                _ = zgui.dragFloat3("Position", .{ .v = &position });

                var rotation = [3]f32{ 0, 0, 0 };
                _ = zgui.dragFloat3("Rotation", .{ .v = &rotation });

                var scale = [3]f32{ 1, 1, 1 };
                _ = zgui.dragFloat3("Scale", .{ .v = &scale });
            }

            if (zgui.collapsingHeader("Settings", .{})) {
                var check = true;
                _ = zgui.checkbox("Enable Feature", &check);

                var selected: i32 = 0;
                const items = [_][:0]const u8{ "Option 1", "Option 2", "Option 3" };
                _ = zgui.combo("Select", .{
                    .current_item = &selected,
                    .items_separated_by_zeros = &items,
                });
            }

            zgui.end();
        }
    }

    fn newFile(self: *App) void {
        _ = self;
        std.log.info("New file", .{});
    }

    fn openFile(self: *App) void {
        _ = self;
        std.log.info("Open file", .{});
    }
};
