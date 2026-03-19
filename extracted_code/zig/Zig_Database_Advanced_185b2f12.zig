//
// Auto-generated from: Zig\Zig_Database_Advanced.md
// Line: 410
// Language: zig
// Block ID: 185b2f12
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

pub const BPlusTree = struct {
    const ORDER = 100;  // 每个节点的最大键数
    const NodeType = enum { INTERNAL, LEAF };

    const Node = struct {
        type: NodeType,
        num_keys: u32,
        keys: [ORDER]u64,

        // 内部节点
        children: [ORDER + 1]u32,

        // 叶子节点
        values: [ORDER]u64,
        next: ?u32,
    };

    page_manager: *PageManager,
    root_page_id: u32,

    pub fn init(pm: *PageManager) !BPlusTree {
        const root = try pm.allocatePage();
        const node = @as(*Node, @ptrCast(&root.data));
        node.* = .{
            .type = .LEAF,
            .num_keys = 0,
            .keys = undefined,
            .values = undefined,
            .next = null,
            .children = undefined,
        };
        pm.unpinPage(root);

        return .{
            .page_manager = pm,
            .root_page_id = root.id,
        };
    }

    pub fn insert(self: *BPlusTree, key: u64, value: u64) !void {
        const root = try self.page_manager.getPage(self.root_page_id);
        defer self.page_manager.unpinPage(root);

        const root_node = @as(*Node, @ptrCast(&root.data));

        if (root_node.num_keys == ORDER) {
            // 根节点分裂
            // ... 实现分裂逻辑
        } else {
            try self.insertNonFull(root, key, value);
        }
    }

    fn insertNonFull(self: *BPlusTree, page: *Page, key: u64, value: u64) !void {
        const node = @as(*Node, @ptrCast(&page.data));

        if (node.type == .LEAF) {
            // 找到插入位置
            var i: u32 = node.num_keys;
            while (i > 0 and node.keys[i - 1] > key) {
                node.keys[i] = node.keys[i - 1];
                node.values[i] = node.values[i - 1];
                i -= 1;
            }

            node.keys[i] = key;
            node.values[i] = value;
            node.num_keys += 1;
            self.page_manager.markDirty(page);
        } else {
            // 内部节点
            var i: u32 = node.num_keys;
            while (i > 0 and node.keys[i - 1] > key) {
                i -= 1;
            }

            const child_page = try self.page_manager.getPage(node.children[i]);
            defer self.page_manager.unpinPage(child_page);

            const child = @as(*Node, @ptrCast(&child_page.data));
            if (child.num_keys == ORDER) {
                // 子节点分裂
                // ...
            } else {
                try self.insertNonFull(child_page, key, value);
            }
        }
    }

    pub fn search(self: *BPlusTree, key: u64) !?u64 {
        var page_id = self.root_page_id;

        while (true) {
            const page = try self.page_manager.getPage(page_id);
            defer self.page_manager.unpinPage(page);

            const node = @as(*Node, @ptrCast(&page.data));

            if (node.type == .LEAF) {
                for (0..node.num_keys) |i| {
                    if (node.keys[i] == key) {
                        return node.values[i];
                    }
                }
                return null;
            } else {
                // 找到合适的子节点
                var i: u32 = node.num_keys;
                while (i > 0 and node.keys[i - 1] > key) {
                    i -= 1;
                }
                page_id = node.children[i];
            }
        }
    }
};
