//
// Auto-generated from: Zig\Zig_Distributed_Systems.md
// Line: 765
// Language: zig
// Block ID: aeea9fa2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 基于 Raft 的分布式 KV 存储
pub const DistributedKV = struct {
    raft: RaftNode,
    storage: std.StringHashMap([]const u8),

    pub fn init(allocator: std.mem.Allocator, id: u64, peers: []const u64) !DistributedKV {
        return .{
            .raft = try RaftNode.init(allocator, id, peers),
            .storage = std.StringHashMap([]const u8).init(allocator),
        };
    }

    pub fn put(self: *DistributedKV, key: []const u8, value: []const u8) !void {
        // 只有 Leader 能处理写请求
        if (self.raft.state != .Leader) {
            return error.NotLeader;
        }

        // 创建日志条目
        const entry = RaftLogEntry{
            .term = self.raft.current_term.load(.SeqCst),
            .command = try std.fmt.allocPrint(std.heap.page_allocator, "PUT {s} {s}", .{ key, value }),
        };

        // 追加到本地日志
        try self.raft.log.append(entry);

        // 复制到其他节点 (异步)
        // 等待多数节点确认
        // 应用到状态机
        try self.storage.put(key, value);
    }

    pub fn get(self: *DistributedKV, key: []const u8) ?[]const u8 {
        // 读可以直接从本地读取
        return self.storage.get(key);
    }

    pub fn delete(self: *DistributedKV, key: []const u8) !void {
        if (self.raft.state != .Leader) {
            return error.NotLeader;
        }

        // 类似 put，但发送 DELETE 命令
        _ = key;
    }
};
