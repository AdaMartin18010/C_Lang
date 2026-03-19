# Zig 分布式系统：从 RPC 到共识算法

> **定位**: Zig 应用 / 分布式系统 | **难度**: ⭐⭐⭐⭐⭐ | **目标**: 掌握 Zig 分布式编程

---

## 📋 目录

- [Zig 分布式系统：从 RPC 到共识算法](#zig-分布式系统从-rpc-到共识算法)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、分布式系统基础](#一分布式系统基础)
  - [二、RPC 框架](#二rpc-框架)
    - [2.1 基础 RPC](#21-基础-rpc)
    - [2.2 gRPC 风格](#22-grpc-风格)
  - [三、服务发现](#三服务发现)
    - [3.1 基于 Consul 的服务发现](#31-基于-consul-的服务发现)
    - [3.2 客户端负载均衡](#32-客户端负载均衡)
  - [四、一致性协议](#四一致性协议)
    - [4.1 Raft 共识算法](#41-raft-共识算法)
  - [五、分布式事务](#五分布式事务)
  - [六、消息队列](#六消息队列)
  - [七、实战：分布式 KV](#七实战分布式-kv)

---


---

## 📑 目录

- [Zig 分布式系统：从 RPC 到共识算法](#zig-分布式系统从-rpc-到共识算法)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、分布式系统基础](#一分布式系统基础)
  - [二、RPC 框架](#二rpc-框架)
    - [2.1 基础 RPC](#21-基础-rpc)
    - [2.2 gRPC 风格](#22-grpc-风格)
  - [三、服务发现](#三服务发现)
    - [3.1 基于 Consul 的服务发现](#31-基于-consul-的服务发现)
    - [3.2 客户端负载均衡](#32-客户端负载均衡)
  - [四、一致性协议](#四一致性协议)
    - [4.1 Raft 共识算法](#41-raft-共识算法)
  - [五、分布式事务](#五分布式事务)
  - [六、消息队列](#六消息队列)
  - [七、实战：分布式 KV](#七实战分布式-kv)


---

## 一、分布式系统基础

```
┌─────────────────────────────────────────────────────────────────────┐
│                    分布式系统挑战                                    │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  网络分区 (Network Partition)                                       │
│  ├── 检测分区                                                        │
│  ├── 脑裂处理                                                        │
│  └── 分区恢复                                                        │
│                                                                      │
│  一致性 (Consistency)                                               │
│  ├── 强一致性: 线性化、顺序一致性                                     │
│  ├── 弱一致性: 最终一致性                                             │
│  └── CAP 定理                                                        │
│                                                                      │
│  可用性 (Availability)                                              │
│  ├── 故障检测                                                        │
│  ├── 故障转移                                                        │
│  └── 负载均衡                                                        │
│                                                                      │
│  容错 (Fault Tolerance)                                             │
│  ├── 超时与重试                                                      │
│  ├── 断路器                                                          │
│  └── 限流与降级                                                      │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 二、RPC 框架

### 2.1 基础 RPC

```zig
const std = @import("std");
const net = std.net;

// RPC 消息格式
pub const RpcMessage = struct {
    id: u64,
    method: []const u8,
    payload: []const u8,
};

pub const RpcResponse = struct {
    id: u64,
    success: bool,
    result: []const u8,
    error_message: ?[]const u8,
};

// 序列化/反序列化
pub const Serializer = struct {
    pub fn serialize(allocator: std.mem.Allocator, msg: RpcMessage) ![]u8 {
        // 简单 TLV 格式
        const method_len = msg.method.len;
        const payload_len = msg.payload.len;
        const total_len = 8 + 4 + method_len + 4 + payload_len;

        var buffer = try allocator.alloc(u8, total_len);
        var pos: usize = 0;

        // ID (8 bytes)
        std.mem.writeIntLittle(u64, buffer[pos..][0..8], msg.id);
        pos += 8;

        // Method length + data
        std.mem.writeIntLittle(u32, buffer[pos..][0..4], @intCast(method_len));
        pos += 4;
        @memcpy(buffer[pos..][0..method_len], msg.method);
        pos += method_len;

        // Payload length + data
        std.mem.writeIntLittle(u32, buffer[pos..][0..4], @intCast(payload_len));
        pos += 4;
        @memcpy(buffer[pos..][0..payload_len], msg.payload);

        return buffer;
    }

    pub fn deserialize(allocator: std.mem.Allocator, data: []const u8) !RpcMessage {
        var pos: usize = 0;

        const id = std.mem.readIntLittle(u64, data[pos..][0..8]);
        pos += 8;

        const method_len = std.mem.readIntLittle(u32, data[pos..][0..4]);
        pos += 4;
        const method = try allocator.dupe(u8, data[pos..][0..method_len]);
        pos += method_len;

        const payload_len = std.mem.readIntLittle(u32, data[pos..][0..4]);
        pos += 4;
        const payload = try allocator.dupe(u8, data[pos..][0..payload_len]);

        return .{
            .id = id,
            .method = method,
            .payload = payload,
        };
    }
};

// RPC 服务器
pub const RpcServer = struct {
    allocator: std.mem.Allocator,
    handlers: std.StringHashMap(Handler),
    listener: net.tcp.Listener,

    const Handler = *const fn ([]const u8) anyerror![]u8;

    pub fn init(allocator: std.mem.Allocator, port: u16) !RpcServer {
        const address = try net.Address.parseIp("0.0.0.0", port);
        var listener = try net.tcp.Listener.init(address);
        try listener.listen(128);

        return .{
            .allocator = allocator,
            .handlers = std.StringHashMap(Handler).init(allocator),
            .listener = listener,
        };
    }

    pub fn register(self: *RpcServer, method: []const u8, handler: Handler) !void {
        try self.handlers.put(method, handler);
    }

    pub fn run(self: *RpcServer) !void {
        while (true) {
            const conn = try self.listener.accept();
            const thread = try std.Thread.spawn(.{}, handleConnection, .{ self, conn });
            thread.detach();
        }
    }

    fn handleConnection(self: *RpcServer, conn: net.tcp.Connection) void {
        defer conn.close();

        var buffer: [4096]u8 = undefined;

        while (true) {
            const bytes = conn.read(&buffer) catch break;
            if (bytes == 0) break;

            // 解析请求
            const request = Serializer.deserialize(self.allocator, buffer[0..bytes]) catch continue;
            defer {
                self.allocator.free(request.method);
                self.allocator.free(request.payload);
            }

            // 查找处理器
            const handler = self.handlers.get(request.method) orelse {
                const error_response = RpcResponse{
                    .id = request.id,
                    .success = false,
                    .result = &[_]u8{},
                    .error_message = "Method not found",
                };
                const response_data = Serializer.serializeResponse(self.allocator, error_response) catch continue;
                defer self.allocator.free(response_data);
                _ = conn.write(response_data) catch {};
                continue;
            };

            // 执行处理
            const result = handler(request.payload) catch |err| {
                const error_response = RpcResponse{
                    .id = request.id,
                    .success = false,
                    .result = &[_]u8{},
                    .error_message = @errorName(err),
                };
                const response_data = Serializer.serializeResponse(self.allocator, error_response) catch continue;
                defer self.allocator.free(response_data);
                _ = conn.write(response_data) catch {};
                continue;
            };
            defer self.allocator.free(result);

            // 发送响应
            const response = RpcResponse{
                .id = request.id,
                .success = true,
                .result = result,
                .error_message = null,
            };
            const response_data = Serializer.serializeResponse(self.allocator, response) catch continue;
            defer self.allocator.free(response_data);
            _ = conn.write(response_data) catch {};
        }
    }
};
```

### 2.2 gRPC 风格

```zig
// Protocol Buffers 风格的定义
pub const GreeterService = struct {
    pub const HelloRequest = struct {
        name: []const u8,
    };

    pub const HelloReply = struct {
        message: []const u8,
    };

    pub fn sayHello(request: HelloRequest) !HelloReply {
        const message = try std.fmt.allocPrint(std.heap.page_allocator, "Hello, {s}!", .{request.name});
        return .{ .message = message };
    }
};

// 代码生成器 (简化)
pub fn generateService(comptime Service: type) type {
    return struct {
        pub fn serve(allocator: std.mem.Allocator, port: u16) !void {
            var server = try RpcServer.init(allocator, port);

            // 为每个方法生成处理器
            inline for (@typeInfo(Service).Struct.decls) |decl| {
                const method = @field(Service, decl.name);
                if (@TypeOf(method) == fn (anytype) anyerror!anytype) {
                    try server.register(decl.name, struct {
                        fn handler(payload: []const u8) ![]u8 {
                            // 反序列化请求
                            // 调用方法
                            // 序列化响应
                            _ = payload;
                            return &[_]u8{};
                        }
                    }.handler);
                }
            }

            try server.run();
        }
    };
}
```

---

## 三、服务发现

### 3.1 基于 Consul 的服务发现

```zig
pub const ServiceDiscovery = struct {
    consul_addr: []const u8,
    service_name: []const u8,
    service_id: []const u8,
    service_port: u16,

    pub fn register(self: ServiceDiscovery) !void {
        const payload = try std.fmt.allocPrint(std.heap.page_allocator,
            \\{{
            \\  "ID": "{s}",
            \\  "Name": "{s}",
            \\  "Port": {d},
            \\  "Check": {{
            \\    "HTTP": "http://localhost:{d}/health",
            \\    "Interval": "10s"
            \\  }}
            \\}}
        , .{ self.service_id, self.service_name, self.service_port, self.service_port });

        // HTTP PUT 到 Consul
        _ = payload;
        // ... HTTP 请求
    }

    pub fn discover(self: ServiceDiscovery, service: []const u8) ![]ServiceInstance {
        // 查询 Consul
        _ = self;
        _ = service;
        return &[_]ServiceInstance{};
    }

    pub fn deregister(self: ServiceDiscovery) !void {
        // 从 Consul 注销
        _ = self;
    }
};

pub const ServiceInstance = struct {
    id: []const u8,
    address: []const u8,
    port: u16,
    healthy: bool,
};
```

### 3.2 客户端负载均衡

```zig
pub const LoadBalancer = struct {
    instances: []ServiceInstance,
    strategy: Strategy,
    current_index: Atomic(usize),

    pub const Strategy = enum {
        round_robin,
        random,
        least_connections,
    };

    pub fn pick(self: *LoadBalancer) ?ServiceInstance {
        switch (self.strategy) {
            .round_robin => {
                const idx = self.current_index.fetchAdd(1, .Monotonic) % self.instances.len;
                return self.instances[idx];
            },
            .random => {
                const idx = std.crypto.random.uintLessThan(usize, self.instances.len);
                return self.instances[idx];
            },
            .least_connections => {
                // 选择连接数最少的实例
                var min_connections: usize = std.math.maxInt(usize);
                var selected: ?ServiceInstance = null;

                for (self.instances) |instance| {
                    if (instance.healthy) {
                        const conns = getConnectionCount(instance);
                        if (conns < min_connections) {
                            min_connections = conns;
                            selected = instance;
                        }
                    }
                }

                return selected;
            },
        }
    }

    fn getConnectionCount(instance: ServiceInstance) usize {
        _ = instance;
        return 0;
    }
};

const Atomic = std.atomic.Atomic;
const std = @import("std");
```

---

## 四、一致性协议

### 4.1 Raft 共识算法

```zig
pub const RaftNode = struct {
    const State = enum { Follower, Candidate, Leader };

    id: u64,
    state: State,

    // 持久状态
    current_term: Atomic(u64),
    voted_for: ?u64,
    log: std.ArrayList(LogEntry),

    // 易失状态
    commit_index: u64,
    last_applied: u64,

    // Leader 状态
    next_index: std.AutoHashMap(u64, u64),
    match_index: std.AutoHashMap(u64, u64),

    // 计时器
    election_timer: Timer,
    heartbeat_timer: Timer,

    pub fn init(allocator: std.mem.Allocator, id: u64, peers: []const u64) !RaftNode {
        return .{
            .id = id,
            .state = .Follower,
            .current_term = Atomic(u64).init(0),
            .voted_for = null,
            .log = std.ArrayList(LogEntry).init(allocator),
            .commit_index = 0,
            .last_applied = 0,
            .next_index = std.AutoHashMap(u64, u64).init(allocator),
            .match_index = std.AutoHashMap(u64, u64).init(allocator),
            .election_timer = Timer.init(randomTimeout()),
            .heartbeat_timer = Timer.init(100),  // 100ms
        };
    }

    pub fn run(self: *RaftNode) !void {
        while (true) {
            switch (self.state) {
                .Follower => try self.runFollower(),
                .Candidate => try self.runCandidate(),
                .Leader => try self.runLeader(),
            }
        }
    }

    fn runFollower(self: *RaftNode) !void {
        // 等待心跳或开始选举
        if (self.election_timer.expired()) {
            self.state = .Candidate;
        }
    }

    fn runCandidate(self: *RaftNode) !void {
        // 递增任期，投票给自己
        const term = self.current_term.fetchAdd(1, .SeqCst) + 1;
        self.voted_for = self.id;

        var votes_received: usize = 1;

        // 向所有节点发送 RequestVote
        // 如果获得多数票，成为 Leader
        // 如果收到更高任期，变回 Follower

        if (votes_received > self.peers.len / 2 + 1) {
            self.state = .Leader;
        }

        _ = term;
    }

    fn runLeader(self: *RaftNode) !void {
        // 发送心跳
        if (self.heartbeat_timer.expired()) {
            try self.sendHeartbeats();
            self.heartbeat_timer.reset();
        }

        // 处理客户端请求
        // 复制日志到所有节点
        // 更新 commit_index
    }

    fn sendHeartbeats(self: RaftNode) !void {
        _ = self;
    }

    // RPC 处理器
    pub fn handleRequestVote(self: *RaftNode, args: RequestVoteArgs) RequestVoteReply {
        var reply = RequestVoteReply{ .term = self.current_term.load(.SeqCst), .vote_granted = false };

        if (args.term < self.current_term.load(.SeqCst)) {
            return reply;
        }

        if (args.term > self.current_term.load(.SeqCst)) {
            self.current_term.store(args.term, .SeqCst);
            self.voted_for = null;
            self.state = .Follower;
        }

        if ((self.voted_for == null or self.voted_for == args.candidate_id) and
            self.isLogUpToDate(args.last_log_index, args.last_log_term)) {
            self.voted_for = args.candidate_id;
            reply.vote_granted = true;
            self.election_timer.reset();
        }

        return reply;
    }

    pub fn handleAppendEntries(self: *RaftNode, args: AppendEntriesArgs) AppendEntriesReply {
        var reply = AppendEntriesReply{ .term = self.current_term.load(.SeqCst), .success = false };

        if (args.term < self.current_term.load(.SeqCst)) {
            return reply;
        }

        self.election_timer.reset();
        self.current_term.store(args.term, .SeqCst);
        self.state = .Follower;

        // 日志一致性检查
        // 追加条目
        // 更新 commit_index

        reply.success = true;
        return reply;
    }

    fn isLogUpToDate(self: RaftNode, last_index: u64, last_term: u64) bool {
        const my_last_index = self.log.items.len;
        const my_last_term = if (my_last_index > 0) self.log.items[my_last_index - 1].term else 0;

        if (last_term != my_last_term) {
            return last_term > my_last_term;
        }
        return last_index >= my_last_index;
    }
};

const LogEntry = struct {
    term: u64,
    index: u64,
    command: []const u8,
};

const RequestVoteArgs = struct {
    term: u64,
    candidate_id: u64,
    last_log_index: u64,
    last_log_term: u64,
};

const RequestVoteReply = struct {
    term: u64,
    vote_granted: bool,
};

const AppendEntriesArgs = struct {
    term: u64,
    leader_id: u64,
    prev_log_index: u64,
    prev_log_term: u64,
    entries: []LogEntry,
    leader_commit: u64,
};

const AppendEntriesReply = struct {
    term: u64,
    success: bool,
};

const Timer = struct {
    timeout_ms: u64,
    start_time: i64,

    fn init(timeout_ms: u64) Timer {
        return .{
            .timeout_ms = timeout_ms,
            .start_time = std.time.milliTimestamp(),
        };
    }

    fn expired(self: Timer) bool {
        return std.time.milliTimestamp() - self.start_time > self.timeout_ms;
    }

    fn reset(self: *Timer) void {
        self.start_time = std.time.milliTimestamp();
    }
};

fn randomTimeout() u64 {
    return 150 + std.crypto.random.uintLessThan(u64, 150);  // 150-300ms
}
```

---

## 五、分布式事务

```zig
// 两阶段提交 (2PC)
pub const TwoPhaseCommit = struct {
    const TransactionState = enum { Prepare, Commit, Abort };

    coordinator: Coordinator,
    participants: []Participant,

    pub const Coordinator = struct {
        pub fn commit(self: Coordinator, txn_id: u64) !void {
            // Phase 1: Prepare
            var all_prepared = true;
            for (self.participants) |p| {
                if (!try p.prepare(txn_id)) {
                    all_prepared = false;
                    break;
                }
            }

            // Phase 2: Commit or Abort
            if (all_prepared) {
                for (self.participants) |p| {
                    try p.commit(txn_id);
                }
            } else {
                for (self.participants) |p| {
                    try p.abort(txn_id);
                }
            }
        }
    };

    pub const Participant = struct {
        pub fn prepare(self: Participant, txn_id: u64) !bool {
            // 写入 prepare 日志
            // 锁定资源
            // 返回是否可以提交
            _ = self;
            _ = txn_id;
            return true;
        }

        pub fn commit(self: Participant, txn_id: u64) !void {
            // 实际提交
            _ = self;
            _ = txn_id;
        }

        pub fn abort(self: Participant, txn_id: u64) !void {
            // 回滚
            _ = self;
            _ = txn_id;
        }
    };
};
```

---

## 六、消息队列

```zig
// 简单的消息队列实现
pub const MessageQueue = struct {
    const Message = struct {
        id: u64,
        topic: []const u8,
        payload: []const u8,
        timestamp: i64,
    };

    allocator: std.mem.Allocator,
    topics: std.StringHashMap(*Topic),

    pub fn init(allocator: std.mem.Allocator) MessageQueue {
        return .{
            .allocator = allocator,
            .topics = std.StringHashMap(*Topic).init(allocator),
        };
    }

    pub fn createTopic(self: *MessageQueue, name: []const u8) !void {
        const topic = try self.allocator.create(Topic);
        topic.* = Topic.init(self.allocator);
        try self.topics.put(name, topic);
    }

    pub fn publish(self: *MessageQueue, topic_name: []const u8, payload: []const u8) !void {
        const topic = self.topics.get(topic_name) orelse return error.TopicNotFound;

        const msg = Message{
            .id = std.crypto.random.int(u64),
            .topic = topic_name,
            .payload = try self.allocator.dupe(u8, payload),
            .timestamp = std.time.milliTimestamp(),
        };

        try topic.publish(msg);
    }

    pub fn subscribe(self: *MessageQueue, topic_name: []const u8, consumer: *Consumer) !void {
        const topic = self.topics.get(topic_name) orelse return error.TopicNotFound;
        try topic.subscribe(consumer);
    }
};

const Topic = struct {
    messages: std.ArrayList(MessageQueue.Message),
    consumers: std.ArrayList(*Consumer),
    mutex: std.Thread.Mutex,

    fn init(allocator: std.mem.Allocator) Topic {
        return .{
            .messages = std.ArrayList(MessageQueue.Message).init(allocator),
            .consumers = std.ArrayList(*Consumer).init(allocator),
            .mutex = .{},
        };
    }

    fn publish(self: *Topic, msg: MessageQueue.Message) !void {
        self.mutex.lock();
        defer self.mutex.unlock();

        try self.messages.append(msg);

        // 通知所有消费者
        for (self.consumers.items) |consumer| {
            try consumer.notify(msg);
        }
    }

    fn subscribe(self: *Topic, consumer: *Consumer) !void {
        self.mutex.lock();
        defer self.mutex.unlock();
        try self.consumers.append(consumer);
    }
};

const Consumer = struct {
    queue: std.ArrayList(MessageQueue.Message),

    fn notify(self: *Consumer, msg: MessageQueue.Message) !void {
        try self.queue.append(msg);
    }
};
```

---

## 七、实战：分布式 KV

```zig
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
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
