/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\README.md
 * Line: 93
 * Language: c
 * Block ID: 7fcca413
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Raft 节点状态定义
typedef enum {
    STATE_FOLLOWER,   // 跟随者：被动响应请求
    STATE_CANDIDATE,  // 候选者：发起选举
    STATE_LEADER      // 领导者：处理客户端请求
} raft_state_t;

typedef struct {
    // 持久状态
    uint64_t current_term;    // 当前任期
    int voted_for;            // 当前任期投票给谁
    log_entry_t* log;         // 日志条目数组
    size_t log_count;

    // 易失状态
    raft_state_t state;
    int commit_index;         // 已知提交的最高日志索引
    int last_applied;         // 已应用到状态机的最高索引

    // 领导者易失状态 (重新选举后重置)
    int* next_index;          // 每个节点的下一个日志索引
    int* match_index;         // 每个节点已复制的最高索引

    // 定时器
    struct timeval election_timeout;
    struct timeval heartbeat_interval;
} raft_node_t;
