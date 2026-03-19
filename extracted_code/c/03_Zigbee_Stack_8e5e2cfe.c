/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\03_Zigbee_Stack.md
 * Line: 223
 * Language: c
 * Block ID: 8e5e2cfe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 构建MAC层帧头
 */
static uint8_t build_mac_header(uint8_t *buffer, const mac_header_t *hdr)
{
    uint8_t pos = 0;

    // 帧控制字段 (2字节)
    uint16_t fcf = 0;
    fcf |= (hdr->fcf.frame_type & 0x07);
    fcf |= (hdr->fcf.security & 0x01) << 3;
    fcf |= (hdr->fcf.frame_pending & 0x01) << 4;
    fcf |= (hdr->fcf.ar & 0x01) << 5;
    fcf |= (hdr->fcf.panid_comp & 0x01) << 6;
    fcf |= (hdr->fcf.dest_addr_mode & 0x03) << 10;
    fcf |= (hdr->fcf.frame_version & 0x03) << 12;
    fcf |= (hdr->fcf.src_addr_mode & 0x03) << 14;

    buffer[pos++] = fcf & 0xFF;
    buffer[pos++] = (fcf >> 8) & 0xFF;

    // 序列号
    buffer[pos++] = hdr->seq_num;

    // 目的PAN ID (如果存在)
    if (hdr->fcf.dest_addr_mode != ADDR_MODE_NONE) {
        buffer[pos++] = hdr->dest_panid & 0xFF;
        buffer[pos++] = (hdr->dest_panid >> 8) & 0xFF;
    }

    // 目的地址
    if (hdr->fcf.dest_addr_mode == ADDR_MODE_SHORT) {
        // 16位短地址
        buffer[pos++] = hdr->dest_addr.addr[0];
        buffer[pos++] = hdr->dest_addr.addr[1];
    } else if (hdr->fcf.dest_addr_mode == ADDR_MODE_EXTENDED) {
        // 64位扩展地址
        for (int i = 0; i < IEEE_ADDR_LEN; i++) {
            buffer[pos++] = hdr->dest_addr.addr[IEEE_ADDR_LEN - 1 - i];
        }
    }

    // 源PAN ID (如果未压缩且源地址存在)
    if (hdr->fcf.src_addr_mode != ADDR_MODE_NONE &&
        !hdr->fcf.panid_comp) {
        buffer[pos++] = hdr->src_panid & 0xFF;
        buffer[pos++] = (hdr->src_panid >> 8) & 0xFF;
    }

    // 源地址
    if (hdr->fcf.src_addr_mode == ADDR_MODE_SHORT) {
        buffer[pos++] = hdr->src_addr.addr[0];
        buffer[pos++] = hdr->src_addr.addr[1];
    } else if (hdr->fcf.src_addr_mode == ADDR_MODE_EXTENDED) {
        for (int i = 0; i < IEEE_ADDR_LEN; i++) {
            buffer[pos++] = hdr->src_addr.addr[IEEE_ADDR_LEN - 1 - i];
        }
    }

    return pos;
}

/**
 * 解析MAC层帧头
 */
static uint8_t parse_mac_header(const uint8_t *buffer, mac_header_t *hdr)
{
    uint8_t pos = 0;

    // 解析帧控制字段
    uint16_t fcf = buffer[pos] | (buffer[pos + 1] << 8);
    pos += 2;

    hdr->fcf.frame_type = fcf & 0x07;
    hdr->fcf.security = (fcf >> 3) & 0x01;
    hdr->fcf.frame_pending = (fcf >> 4) & 0x01;
    hdr->fcf.ar = (fcf >> 5) & 0x01;
    hdr->fcf.panid_comp = (fcf >> 6) & 0x01;
    hdr->fcf.dest_addr_mode = (fcf >> 10) & 0x03;
    hdr->fcf.frame_version = (fcf >> 12) & 0x03;
    hdr->fcf.src_addr_mode = (fcf >> 14) & 0x03;

    // 序列号
    hdr->seq_num = buffer[pos++];

    // 目的PAN ID
    if (hdr->fcf.dest_addr_mode != ADDR_MODE_NONE) {
        hdr->dest_panid = buffer[pos] | (buffer[pos + 1] << 8);
        pos += 2;
    }

    // 目的地址
    if (hdr->fcf.dest_addr_mode == ADDR_MODE_SHORT) {
        hdr->dest_addr.addr[0] = buffer[pos++];
        hdr->dest_addr.addr[1] = buffer[pos++];
    } else if (hdr->fcf.dest_addr_mode == ADDR_MODE_EXTENDED) {
        for (int i = 0; i < IEEE_ADDR_LEN; i++) {
            hdr->dest_addr.addr[IEEE_ADDR_LEN - 1 - i] = buffer[pos++];
        }
    }

    // 源PAN ID
    if (hdr->fcf.src_addr_mode != ADDR_MODE_NONE &&
        !hdr->fcf.panid_comp) {
        hdr->src_panid = buffer[pos] | (buffer[pos + 1] << 8);
        pos += 2;
    }

    // 源地址
    if (hdr->fcf.src_addr_mode == ADDR_MODE_SHORT) {
        hdr->src_addr.addr[0] = buffer[pos++];
        hdr->src_addr.addr[1] = buffer[pos++];
    } else if (hdr->fcf.src_addr_mode == ADDR_MODE_EXTENDED) {
        for (int i = 0; i < IEEE_ADDR_LEN; i++) {
            hdr->src_addr.addr[IEEE_ADDR_LEN - 1 - i] = buffer[pos++];
        }
    }

    return pos;
}

/**
 * 发送MAC层数据帧
 */
int mac_data_request(zigbee_node_t *node,
                      short_addr_t dest_addr,
                      const uint8_t *payload,
                      uint8_t payload_len)
{
    uint8_t frame[256];
    mac_header_t hdr = {0};

    // 配置帧控制字段
    hdr.fcf.frame_type = 1;     // 数据帧
    hdr.fcf.security = 0;       // 无安全
    hdr.fcf.frame_pending = 0;
    hdr.fcf.ar = 1;             // 请求确认
    hdr.fcf.panid_comp = 1;     // PAN ID压缩
    hdr.fcf.dest_addr_mode = ADDR_MODE_SHORT;
    hdr.fcf.frame_version = 1;
    hdr.fcf.src_addr_mode = ADDR_MODE_SHORT;

    hdr.seq_num = node->mac_seq_num++;
    hdr.dest_panid = node->pan_id;
    hdr.dest_addr.addr[0] = dest_addr & 0xFF;
    hdr.dest_addr.addr[1] = (dest_addr >> 8) & 0xFF;
    hdr.src_panid = node->pan_id;
    hdr.src_addr.addr[0] = node->short_addr & 0xFF;
    hdr.src_addr.addr[1] = (node->short_addr >> 8) & 0xFF;

    // 构建帧头
    uint8_t hdr_len = build_mac_header(frame, &hdr);

    // 添加payload
    if (payload_len > MAX_PAYLOAD_LEN - hdr_len - 2) {  // 预留FCS
        payload_len = MAX_PAYLOAD_LEN - hdr_len - 2;
    }
    memcpy(frame + hdr_len, payload, payload_len);

    // 计算并添加FCS (简化 - 实际应为CRC16)
    uint16_t fcs = 0x1234;  // 占位
    frame[hdr_len + payload_len] = fcs & 0xFF;
    frame[hdr_len + payload_len + 1] = (fcs >> 8) & 0xFF;

    // 调用PHY层发送 (模拟)
    printf("[MAC] 发送数据帧到 0x%04X, 长度: %d\n", dest_addr,
           hdr_len + payload_len + 2);

    // 实际硬件调用: phy_transmit(frame, hdr_len + payload_len + 2);

    return 0;
}
