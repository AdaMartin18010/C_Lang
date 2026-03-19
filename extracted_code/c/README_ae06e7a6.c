/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\README.md
 * Line: 1073
 * Language: c
 * Block ID: ae06e7a6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 时间触发协议 - 适用于确定性嵌入式系统 */

/* 时间槽分配 */
#define TTP_CYCLE_TIME_MS 10        /* 周期时间 */
#define TTP_SLOT_COUNT 5            /* 每周期槽位数 */
#define TTP_SLOT_TIME_MS (TTP_CYCLE_TIME_MS / TTP_SLOT_COUNT)

typedef enum {
    TTP_STATE_INIT = 0,
    TTP_STATE_ACTIVE,
    TTP_STATE_PASSIVE,
    TTP_STATE_COLDSTART
} ttp_state_t;

typedef struct {
    uint32_t slot_id;               /* 本节点槽位ID */
    uint32_t cycle_count;           /* 当前周期计数 */
    uint32_t membership;            /* 活跃节点位图 */

    ttp_state_t state;
    uint32_t last_received_mask;    /* 本周期收到消息的节点 */

    /* 时钟同步 */
    int32_t clock_correction;       /* 时钟修正值（微秒） */
    uint32_t local_clock_us;

    /* 应用数据 */
    uint8_t app_data[TTP_SLOT_TIME_MS - HEADER_SIZE];
} ttp_node_t;

/* TTP主循环 */
void ttp_main_loop(ttp_node_t *node) {
    while (1) {
        uint32_t cycle_start = get_micros();
        node->cycle_count++;
        node->last_received_mask = 0;

        for (uint32_t slot = 0; slot < TTP_SLOT_COUNT; slot++) {
            uint32_t slot_start = cycle_start + slot * TTP_SLOT_TIME_MS * 1000;

            if (slot == node->slot_id) {
                /* 本节点的发送槽位 */
                wait_until(slot_start);
                ttp_send_frame(node);
            } else {
                /* 接收槽位 */
                wait_until(slot_start);
                ttp_receive_frame(node, slot);
            }

            /* 时钟同步计算 */
            if (slot == 0) {
                ttp_update_clock_sync(node);
            }
        }

        /* 周期结束处理 */
        ttp_execute_agreement(node);

        /* 等待下一个周期 */
        uint32_t next_cycle = cycle_start + TTP_CYCLE_TIME_MS * 1000
                              + node->clock_correction;
        wait_until(next_cycle);
    }
}

/* TTP共识：基于时间槽的隐式投票 */
void ttp_execute_agreement(ttp_node_t *node) {
    /* 如果一个节点在本周期发送了消息，视为对上一周期的数据同意 */
    /* 收集所有节点的应用数据 */

    for (uint32_t slot = 0; slot < TTP_SLOT_COUNT; slot++) {
        if (!(node->membership & (1 << slot)))
            continue;  /* 非成员节点 */

        if (node->last_received_mask & (1 << slot)) {
            /* 处理来自该槽位节点的数据 */
            process_slot_data(node, slot);
        } else {
            /* 节点未发送 - 可能故障 */
            handle_node_silence(node, slot);
        }
    }
}
