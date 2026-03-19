//
// Auto-generated from: Zig\Zig_Distributed_Systems.md
// Line: 675
// Language: zig
// Block ID: fb8051b6
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

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
