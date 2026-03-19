//
// Auto-generated from: Zig\Zig_Distributed_Systems.md
// Line: 408
// Language: zig
// Block ID: 84749a5a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

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
