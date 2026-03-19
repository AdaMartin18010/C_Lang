/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\03_Zigbee_Stack.md
 * Line: 612
 * Language: c
 * Block ID: 28e18b44
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 初始化Zigbee节点
 */
void zigbee_node_init(zigbee_node_t *node,
                       device_type_t type,
                       const ieee_addr_t *addr)
{
    memset(node, 0, sizeof(*node));

    node->ieee_addr = *addr;
    node->device_type = type;
    node->network_state = NETWORK_DOWN;
    node->channel = DEFAULT_CHANNEL;
    node->short_addr = 0xFFFF;  // 未分配

    // 协调器分配0x0000
    if (type == DEVICE_COORDINATOR) {
        node->short_addr = 0x0000;
        node->pan_id = 0x1234;  // 示例PAN ID
        node->network_state = NETWORK_JOINED;
    }

    // 初始化序列号
    node->mac_seq_num = 0;
    node->nwk_seq_num = 0;
    node->aps_counter = 0;

    printf("[Zigbee] 节点初始化完成, 类型=%s\n",
           type == DEVICE_COORDINATOR ? "协调器" :
           type == DEVICE_ROUTER ? "路由器" : "终端设备");
}

/**
 * 网络加入过程 (终端设备/路由器)
 */
int zigbee_join_network(zigbee_node_t *node,
                         panid_t pan_id,
                         uint8_t channel)
{
    if (node->device_type == DEVICE_COORDINATOR) {
        printf("[Zigbee] 协调器已创建网络\n");
        return 0;
    }

    node->network_state = NETWORK_JOINING;
    node->pan_id = pan_id;
    node->channel = channel;

    printf("[Zigbee] 开始加入网络 PAN=0x%04X, 信道=%d\n", pan_id, channel);

    // 步骤1: 扫描网络
    printf("[Zigbee] 执行主动扫描...\n");
    // 模拟发现协调器

    // 步骤2: 发送关联请求
    printf("[Zigbee] 发送关联请求...\n");

    // 步骤3: 等待关联响应
    // 模拟接收到父节点地址
    node->parent_short_addr = 0x0000;
    node->parent_ieee_addr = (ieee_addr_t){{0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77}};
    node->short_addr = 0x0001;  // 父节点分配
    node->depth = 1;

    node->network_state = NETWORK_JOINED;
    printf("[Zigbee] 成功加入网络, 短地址=0x%04X\n", node->short_addr);

    return 0;
}

/**
 * 发送应用数据
 */
int zigbee_send_data(zigbee_node_t *node,
                      short_addr_t dest_addr,
                      uint8_t dest_ep,
                      uint16_t cluster_id,
                      uint16_t profile_id,
                      const uint8_t *data,
                      uint8_t len)
{
    if (node->network_state != NETWORK_JOINED) {
        fprintf(stderr, "[Zigbee] 网络未连接, 无法发送数据\n");
        return -1;
    }

    // 构建APS帧
    uint8_t aps_frame[128];
    uint8_t pos = 0;

    // APS帧控制
    aps_frame[pos++] = 0x00;  // 数据帧, 无安全
    aps_frame[pos++] = dest_ep;
    aps_frame[pos++] = cluster_id & 0xFF;
    aps_frame[pos++] = (cluster_id >> 8) & 0xFF;
    aps_frame[pos++] = profile_id & 0xFF;
    aps_frame[pos++] = (profile_id >> 8) & 0xFF;
    aps_frame[pos++] = 1;  // 源端点
    aps_frame[pos++] = node->aps_counter++;

    // 组地址 (如果适用)
    // 序列号
    aps_frame[pos++] = 0;

    // Payload
    memcpy(aps_frame + pos, data, len);
    pos += len;

    printf("[Zigbee] 发送数据到 0x%04X:%d, Cluster=0x%04X, 长度=%d\n",
           dest_addr, dest_ep, cluster_id, len);

    // 通过网络层发送
    return nwk_send_data(node, dest_addr, 30, aps_frame, pos);
}

/**
 * 打印节点信息
 */
void zigbee_print_node_info(const zigbee_node_t *node)
{
    const char *state_str[] = {"DOWN", "JOINING", "JOINED", "LEAVING", "REJOINING"};
    const char *type_str[] = {"协调器", "路由器", "终端设备"};

    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║                 Zigbee 节点信息                          ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ IEEE地址:  %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X                    ║\n",
           node->ieee_addr.addr[0], node->ieee_addr.addr[1],
           node->ieee_addr.addr[2], node->ieee_addr.addr[3],
           node->ieee_addr.addr[4], node->ieee_addr.addr[5],
           node->ieee_addr.addr[6], node->ieee_addr.addr[7]);
    printf("║ 短地址:    0x%04X                                        ║\n",
           node->short_addr);
    printf("║ PAN ID:    0x%04X                                        ║\n",
           node->pan_id);
    printf("║ 设备类型:  %-12s                                  ║\n",
           type_str[node->device_type]);
    printf("║ 网络状态:  %-12s                                  ║\n",
           state_str[node->network_state]);
    printf("║ 信道:      %d                                             ║\n",
           node->channel);
    printf("║ 网络深度:  %d                                             ║\n",
           node->depth);
    printf("║ 邻居数量:  %d/%d                                          ║\n",
           node->neighbors.count, MAX_DEVICES);
    printf("║ 子节点数:  %d/%d                                          ║\n",
           node->child_count, MAX_CHILDREN);
    printf("╚══════════════════════════════════════════════════════════╝\n");
}

/**
 * 主函数示例
 */
int main(void)
{
    // 创建协调器
    zigbee_node_t coordinator;
    ieee_addr_t coord_addr = {{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}};
    zigbee_node_init(&coordinator, DEVICE_COORDINATOR, &coord_addr);
    zigbee_print_node_info(&coordinator);

    // 创建终端设备
    zigbee_node_t end_device;
    ieee_addr_t ed_addr = {{0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11}};
    zigbee_node_init(&end_device, DEVICE_END_DEVICE, &ed_addr);

    // 终端设备加入网络
    zigbee_join_network(&end_device, 0x1234, 11);
    zigbee_print_node_info(&end_device);

    // 模拟设备加入处理
    nwk_handle_join_request(&coordinator, &ed_addr,
                            DEVICE_END_DEVICE, 0x8E);

    // 发送数据
    uint8_t payload[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    zigbee_send_data(&end_device, 0x0000, 1, 0x0006, 0x0104,
                     payload, sizeof(payload));

    return 0;
}
