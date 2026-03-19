/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\01_Microkernel_Architecture.md
 * Line: 552
 * Language: c
 * Block ID: 84b5d031
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 驱动管理器 */

#define MAX_DRIVERS 32

typedef struct {
    pid_t pid;
    char name[32];
    uint32_t irq_vector;
    capability_t device_cap;
    bool active;
} driver_info_t;

typedef struct {
    driver_info_t drivers[MAX_DRIVERS];
    int count;
    spinlock_t lock;
} driver_manager_t;

static driver_manager_t g_driver_mgr;

/* 注册驱动 */
int register_driver(const char* name, capability_t* cap) {
    spin_lock(&g_driver_mgr.lock);

    if (g_driver_mgr.count >= MAX_DRIVERS) {
        spin_unlock(&g_driver_mgr.lock);
        return -ENOMEM;
    }

    driver_info_t* drv = &g_driver_mgr.drivers[g_driver_mgr.count];
    drv->pid = getpid();
    strncpy(drv->name, name, sizeof(drv->name) - 1);
    drv->device_cap = *cap;
    drv->active = true;

    g_driver_mgr.count++;

    spin_unlock(&g_driver_mgr.lock);
    return 0;
}

/* 网络驱动示例 */
void network_driver_main(void) {
    message_t msg;
    int client;

    /* 注册到驱动管理器 */
    register_driver("net0", &g_net_cap);

    /* 初始化硬件 (通过能力) */
    if (init_nic(&g_net_cap) < 0) {
        log_error("Failed to init NIC");
        exit(1);
    }

    /* 注册中断处理 */
    register_irq_handler(NIC_IRQ_VECTOR, nic_interrupt);

    while (1) {
        /* 接收请求 */
        receive(ANY, &msg);
        client = msg.source;

        switch (msg.type) {
            case NET_SEND_PACKET:
                handle_send_packet(&msg.data, client);
                break;

            case NET_RECEIVE_PACKET:
                handle_receive_packet(&msg.data, client);
                break;

            case NET_CONFIG:
                handle_config(&msg.data, client);
                break;

            case NET_GET_STATS:
                handle_get_stats(client);
                break;

            default:
                send_error(client, -EINVAL);
        }
    }
}

/* 中断处理 - 内核只转发 */
void nic_interrupt(int vector) {
    /* 简单的通知 */
    notify(NET_DRIVER_PID, IRQ_NOTIFY);
}

/* 驱动内部处理中断 */
void handle_interrupt(void) {
    uint32_t status = read_nic_reg(INT_STATUS);

    if (status & RX_COMPLETE) {
        /* 包到达 */
        enqueue_received_packet();

        /* 通知等待的客户端 */
        notify_waiting_clients();
    }

    if (status & TX_COMPLETE) {
        /* 发送完成 */
        complete_pending_sends();
    }

    /* 清除中断 */
    write_nic_reg(INT_STATUS, status);
}
