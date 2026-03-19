/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\01_Raft_Core.md
 * Line: 69
 * Language: c
 * Block ID: c4a70b94
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Raft节点状态
typedef enum {
    STATE_FOLLOWER,     // 跟随者
    STATE_CANDIDATE,    // 候选人
    STATE_LEADER        // 领导者
} NodeState;

// Raft节点
typedef struct {
    // 节点标识
    int id;
    NodeState state;

    // 持久状态（所有节点）
    uint64_t current_term;      // 当前任期
    int voted_for;              // 投票给谁
    LogEntry *log;              // 日志条目
    size_t log_count;

    // 易失状态（所有节点）
    uint64_t commit_index;      // 已知提交的最高索引
    uint64_t last_applied;      // 应用到状态机的最高索引

    // 易失状态（仅领导者）
    uint64_t *next_index;       // 每个跟随者的下一个日志索引
    uint64_t *match_index;      // 每个跟随者已复制的最高索引

    // 定时器
    uint64_t election_timeout;  // 选举超时
    uint64_t heartbeat_interval; // 心跳间隔

    // 配置
    int *peers;                 // 其他节点ID
    int peer_count;

    // 状态机
    void *state_machine;
    void (*apply_fn)(void *sm, LogEntry *entry);
} RaftNode;

// 日志条目
typedef struct {
    uint64_t term;              // 创建时的任期
    uint64_t index;             // 日志索引
    Command cmd;                // 命令
} LogEntry;

// 命令结构
typedef struct {
    uint32_t type;
    uint32_t len;
    uint8_t data[];
} Command;
