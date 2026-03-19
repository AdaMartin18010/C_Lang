//
// Auto-generated from: Zig\Zig_GUI_Development.md
// Line: 334
// Language: zig
// Block ID: 6c85d02f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const c = @cImport({
    @cInclude("gtk/gtk.h");
});

pub const GtkApp = struct {
    app: *c.GtkApplication,
    window: ?*c.GtkWidget = null,

    pub fn init() !GtkApp {
        const app = c.gtk_application_new("org.zig.app", c.G_APPLICATION_FLAGS_NONE) orelse {
            return error.AppCreationFailed;
        };

        _ = c.g_signal_connect(app, "activate", @ptrCast(&activate), null);

        return .{ .app = app };
    }

    pub fn run(self: GtkApp) void {
        _ = c.g_application_run(@ptrCast(self.app), 0, null);
    }

    fn activate(app: *c.GtkApplication, user_data: ?*anyopaque) callconv(.C) void {
        _ = user_data;

        // 创建窗口
        const window = c.gtk_application_window_new(app);
        c.gtk_window_set_title(@ptrCast(window), "Zig GTK App");
        c.gtk_window_set_default_size(@ptrCast(window), 800, 600);

        // 创建布局
        const box = c.gtk_box_new(c.GTK_ORIENTATION_VERTICAL, 5);
        c.gtk_window_set_child(@ptrCast(window), box);

        // 菜单栏
        const menu_bar = createMenuBar();
        c.gtk_box_append(@ptrCast(box), menu_bar);

        // 工具栏
        const toolbar = createToolbar();
        c.gtk_box_append(@ptrCast(box), toolbar);

        // 分割面板
        const paned = c.gtk_paned_new(c.GTK_ORIENTATION_HORIZONTAL);
        c.gtk_box_append(@ptrCast(box), paned);
        c.gtk_widget_set_vexpand(paned, 1);

        // 左侧树形视图
        const tree_view = createTreeView();
        const scrolled_left = c.gtk_scrolled_window_new();
        c.gtk_scrolled_window_set_child(@ptrCast(scrolled_left), tree_view);
        c.gtk_paned_set_start_child(@ptrCast(paned), scrolled_left);
        c.gtk_paned_set_position(@ptrCast(paned), 200);

        // 右侧编辑器
        const text_view = c.gtk_text_view_new();
        const scrolled_right = c.gtk_scrolled_window_new();
        c.gtk_scrolled_window_set_child(@ptrCast(scrolled_right), text_view);
        c.gtk_paned_set_end_child(@ptrCast(paned), scrolled_right);

        // 状态栏
        const statusbar = c.gtk_statusbar_new();
        c.gtk_box_append(@ptrCast(box), statusbar);

        c.gtk_widget_show(window);
    }

    fn createMenuBar() *c.GtkWidget {
        const menu_bar = c.gtk_menu_bar_new();

        // File 菜单
        const file_menu = c.gtk_menu_new();
        const file_item = c.gtk_menu_item_new_with_label("File");
        c.gtk_menu_item_set_submenu(@ptrCast(file_item), file_menu);
        c.gtk_menu_shell_append(@ptrCast(menu_bar), file_item);

        // File > New
        const new_item = c.gtk_menu_item_new_with_label("New");
        _ = c.g_signal_connect(new_item, "activate", @ptrCast(&onNew), null);
        c.gtk_menu_shell_append(@ptrCast(file_menu), new_item);

        // File > Open
        const open_item = c.gtk_menu_item_new_with_label("Open");
        _ = c.g_signal_connect(open_item, "activate", @ptrCast(&onOpen), null);
        c.gtk_menu_shell_append(@ptrCast(file_menu), open_item);

        c.gtk_menu_shell_append(@ptrCast(file_menu), c.gtk_separator_menu_item_new());

        // File > Exit
        const exit_item = c.gtk_menu_item_new_with_label("Exit");
        _ = c.g_signal_connect(exit_item, "activate", @ptrCast(&onExit), null);
        c.gtk_menu_shell_append(@ptrCast(file_menu), exit_item);

        return menu_bar;
    }

    fn createToolbar() *c.GtkWidget {
        const toolbar = c.gtk_toolbar_new();

        // 新建按钮
        const new_btn = c.gtk_tool_button_new(c.gtk_image_new_from_icon_name("document-new"), "New");
        _ = c.g_signal_connect(new_btn, "clicked", @ptrCast(&onNew), null);
        c.gtk_toolbar_insert(@ptrCast(toolbar), new_btn, -1);

        // 打开按钮
        const open_btn = c.gtk_tool_button_new(c.gtk_image_new_from_icon_name("document-open"), "Open");
        _ = c.g_signal_connect(open_btn, "clicked", @ptrCast(&onOpen), null);
        c.gtk_toolbar_insert(@ptrCast(toolbar), open_btn, -1);

        c.gtk_toolbar_insert(@ptrCast(toolbar), c.gtk_separator_tool_item_new(), -1);

        // 保存按钮
        const save_btn = c.gtk_tool_button_new(c.gtk_image_new_from_icon_name("document-save"), "Save");
        _ = c.g_signal_connect(save_btn, "clicked", @ptrCast(&onSave), null);
        c.gtk_toolbar_insert(@ptrCast(toolbar), save_btn, -1);

        return toolbar;
    }

    fn createTreeView() *c.GtkWidget {
        const store = c.gtk_tree_store_new(1, c.G_TYPE_STRING);

        // 添加数据
        var iter: c.GtkTreeIter = undefined;
        c.gtk_tree_store_append(store, &iter, null);
        c.gtk_tree_store_set(store, &iter, 0, "Project", -1);

        var child: c.GtkTreeIter = undefined;
        c.gtk_tree_store_append(store, &child, &iter);
        c.gtk_tree_store_set(store, &child, 0, "src/main.zig", -1);

        c.gtk_tree_store_append(store, &child, &iter);
        c.gtk_tree_store_set(store, &child, 0, "build.zig", -1);

        const tree_view = c.gtk_tree_view_new_with_model(@ptrCast(store));

        const renderer = c.gtk_cell_renderer_text_new();
        const column = c.gtk_tree_view_column_new_with_attributes("Files", renderer, "text", 0, null);
        c.gtk_tree_view_append_column(@ptrCast(tree_view), column);

        return tree_view;
    }

    fn onNew() callconv(.C) void {
        std.log.info("New file", .{});
    }

    fn onOpen() callconv(.C) void {
        std.log.info("Open file", .{});
    }

    fn onSave() callconv(.C) void {
        std.log.info("Save file", .{});
    }

    fn onExit() callconv(.C) void {
        c.g_application_quit(@ptrCast(app));
    }
};
