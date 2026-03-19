/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\02_Paxos_Implementation.md
 * Line: 589
 * Language: c
 * Block ID: dd0a835c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
