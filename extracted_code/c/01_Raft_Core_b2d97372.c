/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\01_Raft_Core.md
 * Line: 127
 * Language: c
 * Block ID: b2d97372
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// AppendEntries RPC（领导者→跟随者）
typedef struct {
    uint64_t term;              // 领导者任期
    int leader_id;              // 领导者ID
    uint64_t prev_log_index;    // 前一个日志索引
    uint64_t prev_log_term;     // 前一个日志任期
    LogEntry *entries;          // 日志条目（空为心跳）
    size_t entry_count;
    uint64_t leader_commit;     // 领导者已提交索引
} AppendEntriesReq;

typedef struct {
    uint64_t term;              // 当前任期（用于领导者更新）
    bool success;               // 是否成功
    uint64_t conflict_index;    // 冲突索引（优化）
    uint64_t conflict_term;     // 冲突任期
} AppendEntriesRsp;

// RequestVote RPC（候选人→所有节点）
typedef struct {
    uint64_t term;              // 候选人任期
    int candidate_id;           // 候选人ID
    uint64_t last_log_index;    // 最后日志索引
    uint64_t last_log_term;     // 最后日志任期
} RequestVoteReq;

typedef struct {
    uint64_t term;              // 当前任期
    bool vote_granted;          // 是否投票
} RequestVoteRsp;
