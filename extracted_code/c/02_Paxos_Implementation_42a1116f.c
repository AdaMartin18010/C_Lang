/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\02_Paxos_Implementation.md
 * Line: 66
 * Language: c
 * Block ID: 42a1116f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// paxos_types.h
#ifndef PAXOS_TYPES_H
#define PAXOS_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <netinet/in.h>

#define PAXOS_MAX_NODES 16
#define PAXOS_MAX_VALUE_SIZE 1024
#define PAXOS_MAX_LOG_SIZE 10000
#define PAXOS_MAX_PROPOSALS 256

// 提案编号：高32位为轮次，低32位为节点ID
typedef uint64_t proposal_number_t;
#define MAKE_PROPOSAL_NUM(round, node_id) (((uint64_t)(round) << 32) | (node_id))
#define GET_ROUND(num) ((uint32_t)((num) >> 32))
#define GET_NODE_ID(num) ((uint32_t)((num) & 0xFFFFFFFF))

// 节点地址
typedef struct {
    uint32_t node_id;
    char ip[16];
    uint16_t port;
} NodeAddress;

// 提案值
typedef struct {
    uint32_t value_len;
    uint8_t data[PAXOS_MAX_VALUE_SIZE];
} PaxosValue;

// 提案
typedef struct {
    proposal_number_t proposal_num;
    PaxosValue value;
} Proposal;

// 承诺状态（Acceptor端）
typedef struct {
    proposal_number_t promised_num;      // 已承诺的最大编号
    proposal_number_t accepted_num;      // 已接受的最大编号
    PaxosValue accepted_value;           // 已接受的值
    pthread_mutex_t lock;
} AcceptorState;

// 提议状态（Proposer端）
typedef struct {
    proposal_number_t current_num;       // 当前提案编号
    PaxosValue proposed_value;           // 提议的值
    uint32_t prepare_ok_count;           // 收到的Promise数量
    uint32_t accept_ok_count;            // 收到的Accepted数量
    Proposal last_promised_value;        // 收到的最大承诺值
    bool value_chosen;                   // 值是否已被选定
    pthread_mutex_t lock;
    pthread_cond_t cond;
} ProposerState;

// 共识实例（一个Paxos Round）
typedef struct {
    uint64_t instance_id;                // 实例ID
    AcceptorState acceptor;              // Acceptor状态
    ProposerState proposer;              // Proposer状态
    bool is_chosen;                      // 该实例是否已确定值
    PaxosValue chosen_value;             // 最终确定的值
} PaxosInstance;

// Paxos节点配置
typedef struct {
    uint32_t node_id;                    // 本节点ID
    NodeAddress nodes[PAXOS_MAX_NODES];  // 集群节点列表
    uint32_t node_count;                 // 节点总数
    uint32_t quorum_size;                // 多数派大小 (N/2 + 1)
    PaxosInstance instances[PAXOS_MAX_LOG_SIZE];
    pthread_mutex_t log_lock;
} PaxosConfig;

// 消息类型
typedef enum {
    MSG_PREPARE = 1,                     // Phase 1a
    MSG_PROMISE,                         // Phase 1b
    MSG_ACCEPT,                          // Phase 2a
    MSG_ACCEPTED,                        // Phase 2b
    MSG_LEARN,                           // 通知Learner
    MSG_HEARTBEAT                        // 心跳
} MessageType;

// Paxos消息
typedef struct {
    MessageType type;
    uint32_t from_node;
    uint64_t instance_id;
    proposal_number_t proposal_num;
    proposal_number_t accepted_num;
    PaxosValue value;
} PaxosMessage;

#endif
