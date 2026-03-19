/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\02_Paxos_Implementation.md
 * Line: 280
 * Language: c
 * Block ID: f65e40fc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
