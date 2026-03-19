/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\02_Paxos_Implementation.md
 * Line: 516
 * Language: c
 * Block ID: c586cdb3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
