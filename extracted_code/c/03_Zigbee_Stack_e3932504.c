/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\03_Zigbee_Stack.md
 * Line: 402
 * Language: c
 * Block ID: e3932504
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 网络层命令ID
typedef enum {
    NWK_CMD_ROUTE_REQUEST    = 0x01,
    NWK_CMD_ROUTE_REPLY      = 0x02,
    NWK_CMD_NETWORK_STATUS   = 0x03,
    NWK_CMD_LEAVE            = 0x04,
    NWK_CMD_ROUTE_RECORD     = 0x05,
    NWK_CMD_REJOIN_REQUEST   = 0x06,
    NWK_CMD_REJOIN_RESPONSE  = 0x07,
    NWK_CMD_LINK_STATUS      = 0x08,
    NWK_CMD_NETWORK_REPORT   = 0x09,
    NWK_CMD_NETWORK_UPDATE   = 0x0A,
    NWK_CMD_END_DEVICE_TIMEOUT_REQUEST  = 0x0B,
    NWK_CMD_END_DEVICE_TIMEOUT_RESPONSE = 0x0C,
} nwk_command_id_t;

/**
 * 构建网络层数据帧
 */
static uint8_t build_nwk_header(uint8_t *buffer,
                                 const nwk_header_t *hdr,
                                 uint8_t frame_type)
{
    uint8_t pos = 0;

    // 帧控制字段 (2字节)
    uint16_t fcf = 0;
    fcf |= (frame_type & 0x03);         // 帧类型
    fcf |= (0 << 2);                     // 协议版本
    fcf |= (0 << 5);                     // 发现路由
    fcf |= (0 << 6);                     // 多播标志
    fcf |= (0 << 7);                     // 安全
    fcf |= (0 << 8);                     // 源路由
    fcf |= (0 << 9);                     // 目的IEEE地址
    fcf |= (0 << 10);                    // 源IEEE地址

    buffer[pos++] = fcf & 0xFF;
    buffer[pos++] = (fcf >> 8) & 0xFF;

    // 目的地址
    buffer[pos++] = hdr->dest_addr[0];
    buffer[pos++] = hdr->dest_addr[1];

    // 源地址
    buffer[pos++] = hdr->src_addr[0];
    buffer[pos++] = hdr->src_addr[1];

    // 半径
    buffer[pos++] = hdr->radius;

    // 序列号
    buffer[pos++] = hdr->seq_num;

    return pos;
}

/**
 * 网络层加入请求处理
 */
int nwk_handle_join_request(zigbee_node_t *node,
                             const ieee_addr_t *device_addr,
                             device_type_t device_type,
                             uint8_t capability)
{
    if (node->device_type == DEVICE_END_DEVICE) {
        // 终端设备不能处理加入请求
        return -1;
    }

    if (node->child_count >= MAX_CHILDREN) {
        printf("[NWK] 子节点表已满\n");
        return -1;
    }

    // 分配短地址
    short_addr_t new_addr = 0x0001 + node->child_count;
    if (node->device_type == DEVICE_ROUTER) {
        new_addr |= (node->short_addr & 0xFF00);
    }

    // 添加到子节点表
    int idx = node->child_count++;
    node->children[idx].short_addr = new_addr;
    node->children[idx].ieee_addr = *device_addr;
    node->children[idx].type = device_type;
    node->children[idx].valid = true;

    // 添加到邻居表
    if (node->neighbors.count < MAX_DEVICES) {
        device_entry_t *entry = &node->neighbors.devices[node->neighbors.count++];
        entry->ieee_addr = *device_addr;
        entry->short_addr = new_addr;
        entry->device_type = device_type;
        entry->valid = true;
        entry->last_seen = 0;  // 应使用系统时间
    }

    printf("[NWK] 设备 %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X 加入网络, "
           "分配地址: 0x%04X\n",
           device_addr->addr[0], device_addr->addr[1],
           device_addr->addr[2], device_addr->addr[3],
           device_addr->addr[4], device_addr->addr[5],
           device_addr->addr[6], device_addr->addr[7],
           new_addr);

    return 0;
}

/**
 * 发送网络层数据
 */
int nwk_send_data(zigbee_node_t *node,
                   short_addr_t dest_addr,
                   uint8_t radius,
                   const uint8_t *payload,
                   uint8_t payload_len)
{
    if (node->network_state != NETWORK_JOINED) {
        fprintf(stderr, "[NWK] 网络未连接\n");
        return -1;
    }

    uint8_t nwk_frame[256];
    nwk_header_t hdr = {0};

    // 设置网络层头部
    hdr.dest_addr[0] = dest_addr & 0xFF;
    hdr.dest_addr[1] = (dest_addr >> 8) & 0xFF;
    hdr.src_addr[0] = node->short_addr & 0xFF;
    hdr.src_addr[1] = (node->short_addr >> 8) & 0xFF;
    hdr.radius = radius > 0 ? radius : 30;
    hdr.seq_num = node->nwk_seq_num++;

    // 构建网络层帧
    uint8_t hdr_len = build_nwk_header(nwk_frame, &hdr, 0);  // 数据帧
    memcpy(nwk_frame + hdr_len, payload, payload_len);

    // 确定下一跳地址
    short_addr_t next_hop;
    if (dest_addr == 0xFFFC || dest_addr == 0xFFFD || dest_addr == 0xFFFF) {
        // 广播地址
        next_hop = 0xFFFF;
    } else if (dest_addr == node->parent_short_addr) {
        next_hop = node->parent_short_addr;
    } else {
        // 查找路由表或沿树路由
        next_hop = node->parent_short_addr;  // 简化：发往父节点
    }

    // 调用MAC层发送
    return mac_data_request(node, next_hop, nwk_frame, hdr_len + payload_len);
}

/**
 * 处理接收到的网络层帧
 */
void nwk_process_incoming(zigbee_node_t *node,
                          const uint8_t *data,
                          uint8_t len)
{
    if (len < 8) return;

    uint8_t pos = 0;
    uint16_t fcf = data[pos] | (data[pos + 1] << 8);
    pos += 2;

    uint8_t frame_type = fcf & 0x03;

    // 解析地址
    short_addr_t dest_addr = data[pos] | (data[pos + 1] << 8);
    pos += 2;
    short_addr_t src_addr = data[pos] | (data[pos + 1] << 8);
    pos += 2;

    uint8_t radius = data[pos++];
    uint8_t seq_num = data[pos++];

    printf("[NWK] 接收帧: 类型=%d, 源=0x%04X, 目的=0x%04X, 序列=%d\n",
           frame_type, src_addr, dest_addr, seq_num);

    // 检查是否为目的地址
    if (dest_addr != node->short_addr &&
        dest_addr != 0xFFFC &&
        dest_addr != 0xFFFD &&
        dest_addr != 0xFFFF) {
        // 需要转发
        if (radius > 1) {
            // 递减radius并转发
            printf("[NWK] 转发帧到 0x%04X\n", dest_addr);
        }
        return;
    }

    // 处理数据帧
    if (frame_type == 0) {
        // 提取payload并传递给APS层
        printf("[NWK] 数据帧payload长度: %d\n", len - pos);
    } else if (frame_type == 1) {
        // 命令帧
        uint8_t cmd_id = data[pos++];
        printf("[NWK] 命令帧, ID=0x%02X\n", cmd_id);
    }
}
