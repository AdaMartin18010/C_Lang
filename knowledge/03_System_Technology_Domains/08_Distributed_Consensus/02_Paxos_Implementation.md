# Paxos算法C语言实现


---

## 📑 目录

- [Paxos算法C语言实现](#paxos算法c语言实现)
  - [📑 目录](#-目录)
  - [1. Paxos算法核心概念](#1-paxos算法核心概念)
    - [1.1 什么是Paxos](#11-什么是paxos)
    - [1.2 Paxos基本角色](#12-paxos基本角色)
    - [1.3 Paxos两阶段流程](#13-paxos两阶段流程)
  - [2. 核心数据结构](#2-核心数据结构)
  - [3. Acceptor实现](#3-acceptor实现)
  - [4. Proposer实现](#4-proposer实现)
  - [5. 网络层与Learner实现](#5-网络层与learner实现)
  - [6. 完整的Paxos演示程序](#6-完整的paxos演示程序)
  - [7. Paxos算法特性总结](#7-paxos算法特性总结)
    - [7.1 安全性（Safety）保证](#71-安全性safety保证)
    - [7.2 活性（Liveness）条件](#72-活性liveness条件)
    - [7.3 生产环境扩展](#73-生产环境扩展)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 1. Paxos算法核心概念

### 1.1 什么是Paxos

Paxos算法由Leslie Lamport于1990年提出，是分布式系统领域最著名的共识算法之一。
它解决的是在异步系统中，多个节点如何就一个值达成一致的难题，即使在部分节点故障的情况下也能保证一致性。

### 1.2 Paxos基本角色

| 角色 | 职责 | 说明 |
|------|------|------|
| **Proposer** | 提议者 | 提出提案（Proposal），驱动共识过程 |
| **Acceptor** | 接受者 | 对提案进行投票，存储接受的值 |
| **Learner** | 学习者 | 学习已达成共识的值，不参与投票 |

### 1.3 Paxos两阶段流程

```text
Phase 1: Prepare/Promise
-------------------------
Proposer                Acceptor
   |    1. Prepare(n)      |
   | --------------------> |
   |    2. Promise(n, v)   |
   | <-------------------- |

Phase 2: Accept/Accepted
-------------------------
Proposer                Acceptor
   |    3. Accept(n, v)    |
   | --------------------> |
   |    4. Accepted(n, v)  |
   | <-------------------- |

编号规则：n 必须是全局唯一的提案编号，通常由(轮次, ServerID)组成
```

## 2. 核心数据结构

```c
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
```

## 3. Acceptor实现

```c
// paxos_acceptor.c
#include "paxos_types.h"
#include <string.h>
#include <stdio.h>

// 初始化Acceptor状态
void acceptor_init(AcceptorState *state) {
    memset(state, 0, sizeof(AcceptorState));
    state->promised_num = 0;
    state->accepted_num = 0;
    pthread_mutex_init(&state->lock, NULL);
}

// 处理Prepare请求
// 返回: true表示承诺，false表示拒绝
bool acceptor_handle_prepare(AcceptorState *state,
                              const PaxosMessage *request,
                              PaxosMessage *response) {
    pthread_mutex_lock(&state->lock);

    printf("[Acceptor] Received Prepare(n=%lu, round=%u)\n",
           request->proposal_num, GET_ROUND(request->proposal_num));

    // 只接受编号更大的提案
    if (request->proposal_num > state->promised_num) {
        state->promised_num = request->proposal_num;

        // 构造Promise响应
        response->type = MSG_PROMISE;
        response->proposal_num = request->proposal_num;
        response->accepted_num = state->accepted_num;

        // 如果已经接受过值，告知Proposer
        if (state->accepted_num > 0) {
            memcpy(&response->value, &state->accepted_value, sizeof(PaxosValue));
            printf("[Acceptor] Promise with previously accepted value (n=%lu)\n",
                   state->accepted_num);
        } else {
            memset(&response->value, 0, sizeof(PaxosValue));
            printf("[Acceptor] Promise (no previous value)\n");
        }

        pthread_mutex_unlock(&state->lock);
        return true;
    }

    printf("[Acceptor] Reject Prepare (promised n=%lu)\n",
           state->promised_num);
    pthread_mutex_unlock(&state->lock);
    return false;
}

// 处理Accept请求
// 返回: true表示接受，false表示拒绝
bool acceptor_handle_accept(AcceptorState *state,
                            const PaxosMessage *request,
                            PaxosMessage *response) {
    pthread_mutex_lock(&state->lock);

    printf("[Acceptor] Received Accept(n=%lu, v=%.*s)\n",
           request->proposal_num,
           (int)request->value.value_len,
           request->value.data);

    // 必须不小于已承诺的编号（允许等于，因为可能是自己的提案）
    if (request->proposal_num >= state->promised_num) {
        // 更新状态
        state->promised_num = request->proposal_num;
        state->accepted_num = request->proposal_num;
        memcpy(&state->accepted_value, &request->value, sizeof(PaxosValue));

        // 构造Accepted响应
        response->type = MSG_ACCEPTED;
        response->proposal_num = request->proposal_num;
        response->accepted_num = request->proposal_num;
        memcpy(&response->value, &request->value, sizeof(PaxosValue));

        printf("[Acceptor] Accepted value (n=%lu)\n", request->proposal_num);

        pthread_mutex_unlock(&state->lock);
        return true;
    }

    printf("[Acceptor] Reject Accept (promised n=%lu)\n",
           state->promised_num);
    pthread_mutex_unlock(&state->lock);
    return false;
}

// 获取已接受的值
bool acceptor_get_accepted(AcceptorState *state,
                           proposal_number_t *num,
                           PaxosValue *value) {
    pthread_mutex_lock(&state->lock);

    if (state->accepted_num > 0) {
        *num = state->accepted_num;
        memcpy(value, &state->accepted_value, sizeof(PaxosValue));
        pthread_mutex_unlock(&state->lock);
        return true;
    }

    pthread_mutex_unlock(&state->lock);
    return false;
}
```

## 4. Proposer实现

```c
// paxos_proposer.c
#include "paxos_types.h"
#include <stdlib.h>
#include <unistd.h>

// 初始化Proposer状态
void proposer_init(ProposerState *state) {
    memset(state, 0, sizeof(ProposerState));
    pthread_mutex_init(&state->lock, NULL);
    pthread_cond_init(&state->cond, NULL);
}

// 生成新的提案编号（增加轮次）
proposal_number_t proposer_next_number(ProposerState *state,
                                        uint32_t node_id) {
    static uint32_t round = 0;
    round++;
    return MAKE_PROPOSAL_NUM(round, node_id);
}

// Phase 1: 发送Prepare请求
void proposer_phase1_send(PaxosConfig *config,
                          uint64_t instance_id,
                          proposal_number_t proposal_num) {
    PaxosMessage msg = {
        .type = MSG_PREPARE,
        .from_node = config->node_id,
        .instance_id = instance_id,
        .proposal_num = proposal_num
    };

    printf("[Proposer] Phase 1: Sending Prepare(n=%lu) to all acceptors\n",
           proposal_num);

    // 广播Prepare消息（简化：直接调用消息处理器）
    for (uint32_t i = 0; i < config->node_count; i++) {
        if (config->nodes[i].node_id != config->node_id) {
            network_send_message(config, &config->nodes[i], &msg);
        }
    }
}

// Phase 1: 处理Promise响应
bool proposer_phase1_handle(PaxosConfig *config,
                            uint64_t instance_id,
                            const PaxosMessage *promise) {
    PaxosInstance *inst = &config->instances[instance_id % PAXOS_MAX_LOG_SIZE];
    ProposerState *state = &inst->proposer;

    pthread_mutex_lock(&state->lock);

    // 只处理当前实例的响应
    if (promise->proposal_num != state->current_num) {
        pthread_mutex_unlock(&state->lock);
        return false;
    }

    state->prepare_ok_count++;
    printf("[Proposer] Received Promise %u/%u\n",
           state->prepare_ok_count, config->quorum_size);

    // 检查是否有已接受的值
    if (promise->accepted_num > 0 &&
        promise->accepted_num > state->last_promised_value.proposal_num) {
        memcpy(&state->last_promised_value, promise, sizeof(Proposal));
    }

    // 达到多数派
    if (state->prepare_ok_count >= config->quorum_size) {
        pthread_cond_signal(&state->cond);
        pthread_mutex_unlock(&state->lock);
        return true;
    }

    pthread_mutex_unlock(&state->lock);
    return false;
}

// Phase 2: 发送Accept请求
void proposer_phase2_send(PaxosConfig *config,
                          uint64_t instance_id,
                          const PaxosValue *value) {
    PaxosInstance *inst = &config->instances[instance_id % PAXOS_MAX_LOG_SIZE];
    ProposerState *state = &inst->proposer;

    PaxosMessage msg = {
        .type = MSG_ACCEPT,
        .from_node = config->node_id,
        .instance_id = instance_id,
        .proposal_num = state->current_num
    };
    memcpy(&msg.value, value, sizeof(PaxosValue));

    printf("[Proposer] Phase 2: Sending Accept(n=%lu, v=%.*s)\n",
           state->current_num, (int)value->value_len, value->data);

    // 广播Accept消息
    for (uint32_t i = 0; i < config->node_count; i++) {
        if (config->nodes[i].node_id != config->node_id) {
            network_send_message(config, &config->nodes[i], &msg);
        }
    }
}

// Phase 2: 处理Accepted响应
bool proposer_phase2_handle(PaxosConfig *config,
                            uint64_t instance_id,
                            const PaxosMessage *accepted) {
    PaxosInstance *inst = &config->instances[instance_id % PAXOS_MAX_LOG_SIZE];
    ProposerState *state = &inst->proposer;

    pthread_mutex_lock(&state->lock);

    if (accepted->proposal_num != state->current_num) {
        pthread_mutex_unlock(&state->lock);
        return false;
    }

    state->accept_ok_count++;
    printf("[Proposer] Received Accepted %u/%u\n",
           state->accept_ok_count, config->quorum_size);

    // 达到多数派，值已被选定
    if (state->accept_ok_count >= config->quorum_size) {
        state->value_chosen = true;
        pthread_cond_signal(&state->cond);
        pthread_mutex_unlock(&state->lock);
        return true;
    }

    pthread_mutex_unlock(&state->lock);
    return false;
}

// 执行完整的Paxos提议流程
bool proposer_run(PaxosConfig *config, uint64_t instance_id,
                  const PaxosValue *value, int timeout_ms) {
    PaxosInstance *inst = &config->instances[instance_id % PAXOS_MAX_LOG_SIZE];
    ProposerState *state = &inst->proposer;

    pthread_mutex_lock(&state->lock);

    // 重置状态
    state->prepare_ok_count = 0;
    state->accept_ok_count = 0;
    state->value_chosen = false;
    memset(&state->last_promised_value, 0, sizeof(Proposal));
    memcpy(&state->proposed_value, value, sizeof(PaxosValue));

    // 生成新的提案编号
    state->current_num = proposer_next_number(state, config->node_id);
    proposal_number_t my_num = state->current_num;

    pthread_mutex_unlock(&state->lock);

    int retries = 0;
    const int MAX_RETRIES = 3;

    while (retries < MAX_RETRIES) {
        // ===== Phase 1 =====
        proposer_phase1_send(config, instance_id, my_num);

        // 等待Promise达到多数派
        pthread_mutex_lock(&state->lock);
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;
        ts.tv_sec += timeout_ms / 1000 + ts.tv_nsec / 1000000000;
        ts.tv_nsec %= 1000000000;

        int wait_result = 0;
        while (state->prepare_ok_count < config->quorum_size && wait_result == 0) {
            wait_result = pthread_cond_timedwait(&state->cond, &state->lock, &ts);
        }

        if (state->prepare_ok_count < config->quorum_size) {
            printf("[Proposer] Phase 1 timeout, retrying...\n");
            pthread_mutex_unlock(&state->lock);
            retries++;
            my_num = proposer_next_number(state, config->node_id);
            pthread_mutex_lock(&state->lock);
            state->current_num = my_num;
            state->prepare_ok_count = 0;
            pthread_mutex_unlock(&state->lock);
            continue;
        }

        // 确定要提议的值
        PaxosValue *value_to_propose = &state->proposed_value;
        if (state->last_promised_value.proposal_num > 0) {
            // 使用已接受的值（Paxos安全性保证）
            value_to_propose = &state->last_promised_value.value;
            printf("[Proposer] Using previously accepted value\n");
        }
        pthread_mutex_unlock(&state->lock);

        // ===== Phase 2 =====
        state->accept_ok_count = 0;
        proposer_phase2_send(config, instance_id, value_to_propose);

        // 等待Accepted达到多数派
        pthread_mutex_lock(&state->lock);
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;
        ts.tv_sec += timeout_ms / 1000 + ts.tv_nsec / 1000000000;
        ts.tv_nsec %= 1000000000;

        wait_result = 0;
        while (!state->value_chosen && wait_result == 0) {
            wait_result = pthread_cond_timedwait(&state->cond, &state->lock, &ts);
        }

        bool chosen = state->value_chosen;
        pthread_mutex_unlock(&state->lock);

        if (chosen) {
            printf("[Proposer] Value chosen successfully!\n");
            return true;
        }

        printf("[Proposer] Phase 2 timeout, retrying with higher number...\n");
        retries++;
        pthread_mutex_lock(&state->lock);
        my_num = proposer_next_number(state, config->node_id);
        state->current_num = my_num;
        pthread_mutex_unlock(&state->lock);
    }

    printf("[Proposer] Failed after %d retries\n", MAX_RETRIES);
    return false;
}
```

## 5. 网络层与Learner实现

```c
// paxos_network.c
#include "paxos_types.h"
#include <sys/socket.h>
#include <arpa/inet.h>

// 模拟网络发送（实际实现应使用UDP/TCP）
void network_send_message(PaxosConfig *config,
                          const NodeAddress *dest,
                          const PaxosMessage *msg) {
    // 在实际实现中，这里会序列化消息并通过socket发送
    // 为演示目的，我们直接调用消息处理器
    (void)config;
    (void)dest;
    (void)msg;
    // 实际代码: sendto(socket, serialized_msg, ...);
}

// Learner处理Accepted消息
void learner_handle_accepted(PaxosConfig *config,
                             uint64_t instance_id,
                             const PaxosMessage *accepted) {
    PaxosInstance *inst = &config->instances[instance_id % PAXOS_MAX_LOG_SIZE];
    static uint32_t accepted_count[PAXOS_MAX_LOG_SIZE] = {0};
    static PaxosValue learned_values[PAXOS_MAX_LOG_SIZE];
    static proposal_number_t learned_nums[PAXOS_MAX_LOG_SIZE] = {0};

    if (inst->is_chosen) {
        return; // 已经学习过
    }

    // 使用简单的计数（实际应使用更精确的去重）
    if (learned_nums[instance_id] != accepted->proposal_num) {
        learned_nums[instance_id] = accepted->proposal_num;
        accepted_count[instance_id] = 0;
        memcpy(&learned_values[instance_id],
               &accepted->value, sizeof(PaxosValue));
    }

    accepted_count[instance_id]++;

    if (accepted_count[instance_id] >= config->quorum_size) {
        pthread_mutex_lock(&config->log_lock);
        inst->is_chosen = true;
        memcpy(&inst->chosen_value,
               &learned_values[instance_id], sizeof(PaxosValue));
        pthread_mutex_unlock(&config->log_lock);

        printf("[Learner] Instance %lu CHOSEN: %.*s\n",
               instance_id,
               (int)inst->chosen_value.value_len,
               inst->chosen_value.data);
    }
}

// 获取已学习的值
bool learner_get_chosen(PaxosConfig *config, uint64_t instance_id,
                        PaxosValue *value) {
    PaxosInstance *inst = &config->instances[instance_id % PAXOS_MAX_LOG_SIZE];

    pthread_mutex_lock(&config->log_lock);
    if (inst->is_chosen) {
        memcpy(value, &inst->chosen_value, sizeof(PaxosValue));
        pthread_mutex_unlock(&config->log_lock);
        return true;
    }
    pthread_mutex_unlock(&config->log_lock);
    return false;
}
```

## 6. 完整的Paxos演示程序

```c
// paxos_demo.c
#include "paxos_types.h"
#include <stdio.h>
#include <string.h>

// 模拟集群配置
PaxosConfig g_config;

// 初始化演示集群
void init_demo_cluster(void) {
    memset(&g_config, 0, sizeof(PaxosConfig));

    g_config.node_id = 1;  // 本节点ID
    g_config.node_count = 3;  // 3节点集群
    g_config.quorum_size = 2;  // 多数派: 3/2 + 1 = 2

    // 配置节点
    for (uint32_t i = 0; i < g_config.node_count; i++) {
        g_config.nodes[i].node_id = i + 1;
        snprintf(g_config.nodes[i].ip, 16, "127.0.0.1");
        g_config.nodes[i].port = 8000 + i + 1;
    }

    pthread_mutex_init(&g_config.log_lock, NULL);

    // 初始化所有实例
    for (int i = 0; i < PAXOS_MAX_LOG_SIZE; i++) {
        g_config.instances[i].instance_id = i;
        acceptor_init(&g_config.instances[i].acceptor);
        proposer_init(&g_config.instances[i].proposer);
    }
}

// 模拟消息处理（用于单机演示）
void simulate_message_handling(PaxosConfig *config,
                               PaxosMessage *msg) {
    uint64_t inst_id = msg->instance_id;
    PaxosInstance *inst = &config->instances[inst_id % PAXOS_MAX_LOG_SIZE];

    switch (msg->type) {
        case MSG_PREPARE: {
            PaxosMessage response;
            if (acceptor_handle_prepare(&inst->acceptor, msg, &response)) {
                // 模拟发送Promise响应
                response.instance_id = inst_id;
                response.from_node = config->node_id;
                proposer_phase1_handle(config, inst_id, &response);
            }
            break;
        }

        case MSG_ACCEPT: {
            PaxosMessage response;
            if (acceptor_handle_accept(&inst->acceptor, msg, &response)) {
                // 模拟发送Accepted响应
                response.instance_id = inst_id;
                response.from_node = config->node_id;
                proposer_phase2_handle(config, inst_id, &response);
                learner_handle_accepted(config, inst_id, &response);
            }
            break;
        }

        default:
            break;
    }
}

// 重写网络发送函数，使用本地处理
void network_send_message(PaxosConfig *config,
                          const NodeAddress *dest,
                          const PaxosMessage *msg) {
    (void)dest;  // 在演示中忽略目标

    PaxosMessage msg_copy = *msg;
    simulate_message_handling(config, &msg_copy);

    // 模拟第二个节点的响应（达到多数派）
    if (msg->type == MSG_PREPARE || msg->type == MSG_ACCEPT) {
        simulate_message_handling(config, &msg_copy);
    }
}

int main(void) {
    printf("=== Paxos Consensus Demo ===\n\n");

    init_demo_cluster();

    printf("Cluster: %u nodes, quorum: %u\n\n",
           g_config.node_count, g_config.quorum_size);

    // 测试1: 基本提议
    printf("--- Test 1: Basic Proposal ---\n");
    PaxosValue val1 = {.value_len = 10};
    memcpy(val1.data, "HelloPaxos", 10);

    bool success = proposer_run(&g_config, 0, &val1, 5000);
    printf("Result: %s\n\n", success ? "SUCCESS" : "FAILED");

    // 验证值
    PaxosValue learned;
    if (learner_get_chosen(&g_config, 0, &learned)) {
        printf("Learned value: %.*s\n\n",
               (int)learned.value_len, learned.data);
    }

    // 测试2: 多个实例
    printf("--- Test 2: Multiple Instances ---\n");
    const char *commands[] = {"SET x=10", "SET y=20", "ADD x,y"};

    for (int i = 0; i < 3; i++) {
        PaxosValue cmd;
        cmd.value_len = strlen(commands[i]);
        memcpy(cmd.data, commands[i], cmd.value_len);

        printf("\nProposing instance %d: %s\n", i + 1, commands[i]);
        success = proposer_run(&g_config, i + 1, &cmd, 5000);

        if (success && learner_get_chosen(&g_config, i + 1, &learned)) {
            printf("Instance %d chosen: %.*s\n",
                   i + 1, (int)learned.value_len, learned.data);
        }
    }

    // 测试3: 显示所有选择的值
    printf("\n--- All Chosen Values ---\n");
    for (int i = 0; i < 5; i++) {
        if (learner_get_chosen(&g_config, i, &learned)) {
            printf("Instance %d: %.*s\n",
                   i, (int)learned.value_len, learned.data);
        }
    }

    printf("\n=== Demo Complete ===\n");
    return 0;
}

// 编译命令:
// gcc -o paxos_demo paxos_demo.c paxos_acceptor.c paxos_proposer.c \
//     paxos_network.c -lpthread -std=c17 -O2
```

## 7. Paxos算法特性总结

### 7.1 安全性（Safety）保证

1. **一致性**：所有节点对同一个实例只能选择一个值
2. **有效性**：只有被提议的值才能被选择
3. **不可变性**：一旦值被选择，不可更改

### 7.2 活性（Liveness）条件

- 当大多数Acceptor可用，且网络延迟有界时，最终会有值被选择
- 需要避免活锁：多个Proposer竞争时可能无限重试

### 7.3 生产环境扩展

1. **Multi-Paxos**：选出一个Leader来批量处理请求
2. **磁盘持久化**：Acceptor需要持久化accepted值
3. **快照机制**：避免日志无限增长
4. **成员变更**：支持动态添加/删除节点


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
