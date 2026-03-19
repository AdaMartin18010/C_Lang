/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\README.md
 * Line: 967
 * Language: c
 * Block ID: 30cc2f89
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 嵌入式Raft优化实现 */

/* 内存优化：固定大小的日志缓冲区 */
#define EMBEDDED_LOG_SIZE 256       /* 最大日志条目数 */
#define EMBEDDED_ENTRY_SIZE 64      /* 每条目最大64字节 */
#define EMBEDDED_NODE_COUNT 5       /* 最大节点数 */

typedef struct {
    uint64_t term;
    uint32_t index;
    uint8_t data[EMBEDDED_ENTRY_SIZE];
    uint8_t len;
} embedded_log_entry_t;

typedef struct {
    /* 固定大小数组避免动态分配 */
    embedded_log_entry_t log_buffer[EMBEDDED_LOG_SIZE];
    uint32_t log_start;     /* 环形缓冲区起始 */
    uint32_t log_count;     /* 当前条目数 */

    /* 压缩状态 */
    uint64_t last_snapshot_index;
    uint64_t last_snapshot_term;

    /* 网络优化 */
    uint8_t send_buffer[512];   /* 预分配发送缓冲区 */
    uint8_t recv_buffer[512];   /* 预分配接收缓冲区 */

    /* 超时使用tick计数而非绝对时间 */
    uint32_t election_timeout_ticks;
    uint32_t heartbeat_timeout_ticks;
    uint32_t last_activity_ticks;
} embedded_raft_state_t;

/* 增量日志复制 - 只发送差异 */
typedef struct {
    uint64_t prev_index;        /* 前一条索引 */
    uint8_t entry_count;
    struct {
        uint64_t term;
        uint8_t len;
        uint8_t data[EMBEDDED_ENTRY_SIZE];
    } entries[8];               /* 一批最多8条 */
} incremental_append_req_t;

/* 轻量级心跳 */
typedef struct {
    uint64_t term;
    uint64_t commit_index;
    uint16_t checksum;          /* 简单校验和 */
} lightweight_heartbeat_t;

/* 错误恢复优化 */
void embedded_raft_log_compaction(embedded_raft_state_t *state,
                                  uint64_t snapshot_index) {
    /* 将已应用日志转换为快照，释放空间 */
    uint32_t entries_to_remove = snapshot_index - state->last_snapshot_index;

    if (entries_to_remove >= EMBEDDED_LOG_SIZE) {
        /* 完全环绕 */
        state->log_start = 0;
        state->log_count = 0;
    } else {
        state->log_start = (state->log_start + entries_to_remove)
                           % EMBEDDED_LOG_SIZE;
        state->log_count -= entries_to_remove;
    }

    state->last_snapshot_index = snapshot_index;
    state->last_snapshot_term = state->log_buffer[state->log_start].term;

    /* 持久化快照 */
    persist_snapshot(state->last_snapshot_index,
                     state->last_snapshot_term);
}

/* CAN总线优化 - 使用广播 */
void embedded_raft_send_can_broadcast(embedded_raft_state_t *state,
                                      raft_msg_type_t type,
                                      const void *data, size_t len) {
    /* CAN帧格式：仲裁ID包含消息类型和节点ID */
    uint32_t can_id = (type << 8) | NODE_ID;

    /* 分段发送长消息 */
    const uint8_t *ptr = data;
    size_t remaining = len;
    uint8_t seq = 0;

    while (remaining > 0) {
        uint8_t frame[8];
        frame[0] = seq++;           /* 序列号 */
        frame[1] = (remaining > 6) ? 0 : remaining; /* 最后帧标记 */

        size_t chunk = (remaining > 6) ? 6 : remaining;
        memcpy(&frame[2], ptr, chunk);

        can_send(can_id, frame, 8);
        ptr += chunk;
        remaining -= chunk;
    }
}
