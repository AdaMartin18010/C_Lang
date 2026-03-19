/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\README.md
 * Line: 133
 * Language: c
 * Block ID: 81b3fd59
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Raft 节点状态定义 */
typedef enum {
    RAFT_STATE_FOLLOWER = 0,
    RAFT_STATE_CANDIDATE,
    RAFT_STATE_LEADER
} raft_state_t;

/* 日志条目结构 */
typedef struct {
    uint64_t term;          /* 创建此条目的领导者任期 */
    uint64_t index;         /* 日志索引（单调递增） */
    uint8_t data[];         /* 实际命令数据（柔性数组） */
} raft_log_entry_t;

/* Raft 节点状态 */
typedef struct {
    /* 持久化状态（所有服务器） */
    uint64_t current_term;      /* 当前任期 */
    int voted_for;              /* 当前任期投票给谁(-1表示未投票) */
    raft_log_entry_t **log;     /* 日志条目数组 */
    uint64_t log_count;         /* 日志条目数量 */

    /* 易失性状态（所有服务器） */
    uint64_t commit_index;      /* 已知的最高提交索引 */
    uint64_t last_applied;      /* 已应用到状态机的最高索引 */
    raft_state_t state;         /* 当前状态 */

    /* 易失性状态（仅领导者） */
    uint64_t *next_index;       /* 每个跟随者的下一个日志索引 */
    uint64_t *match_index;      /* 每个跟随者已复制的最高索引 */

    /* 配置 */
    int node_id;                /* 本节点ID */
    int node_count;             /* 集群节点总数 */
    int *peer_ids;              /* 其他节点ID数组 */

    /* 超时配置 */
    uint32_t election_timeout;      /* 选举超时（随机150-300ms） */
    uint32_t heartbeat_interval;    /* 心跳间隔（通常50ms） */
    uint64_t last_heartbeat;        /* 上次收到心跳时间 */

    /* 网络和存储 */
    raft_send_rpc_fn send_rpc;      /* RPC发送回调 */
    raft_apply_fn apply;            /* 应用日志回调 */
    raft_persist_fn persist;        /* 持久化回调 */
} raft_node_t;

/* RPC消息类型 */
typedef enum {
    RAFT_MSG_REQUEST_VOTE = 1,
    RAFT_MSG_REQUEST_VOTE_RESPONSE,
    RAFT_MSG_APPEND_ENTRIES,
    RAFT_MSG_APPEND_ENTRIES_RESPONSE
} raft_msg_type_t;

/* RequestVote RPC参数 */
typedef struct {
    uint64_t term;              /* 候选人任期 */
    int candidate_id;           /* 候选人ID */
    uint64_t last_log_index;    /* 候选人最后日志索引 */
    uint64_t last_log_term;     /* 候选人最后日志任期 */
} request_vote_req_t;

typedef struct {
    uint64_t term;              /* 当前任期（用于候选人更新自己） */
    bool vote_granted;          /* 是否投票 */
} request_vote_resp_t;

/* AppendEntries RPC参数 */
typedef struct {
    uint64_t term;              /* 领导者任期 */
    int leader_id;              /* 领导者ID */
    uint64_t prev_log_index;    /* 新日志条目之前的那条索引 */
    uint64_t prev_log_term;     /* prev_log_index条目的任期 */
    raft_log_entry_t **entries; /* 要存储的日志条目（可以为空，仅心跳） */
    uint64_t entry_count;       /* 条目数量 */
    uint64_t leader_commit;     /* 领导者的commit_index */
} append_entries_req_t;

typedef struct {
    uint64_t term;              /* 当前任期 */
    bool success;               /* 如果跟随者包含prev_log条目则为true */
    uint64_t match_index;       /* 跟随者匹配的最高索引 */
    uint64_t conflict_term;     /* 冲突任期（用于加速回退） */
    uint64_t conflict_index;    /* 冲突索引 */
} append_entries_resp_t;
