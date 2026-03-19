/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\README.md
 * Line: 461
 * Language: c
 * Block ID: d7c3e2fc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// CCSDS 数据包处理
#define CCSDS_PRIMARY_HEADER_SIZE 6
#define CCSDS_SECONDARY_HEADER_SIZE 10

typedef struct __attribute__((packed)) {
    uint16_t packet_id;      // 版本(3) + 类型(1) + 次要标志(1) + APID(11)
    uint16_t packet_seq;     // 序列标志(2) + 序列计数(14)
    uint16_t packet_len;     // 包长度 - 1
} CCSDS_PrimaryHeader;

typedef struct __attribute__((packed)) {
    uint32_t timestamp_secs;
    uint16_t timestamp_subsecs;
    uint16_t source_id;
    uint8_t  spare[2];
} CCSDS_SecondaryHeader;

// 遥测数据包组装
int assemble_telemetry(uint16_t apid, const uint8_t *data,
                       size_t data_len, uint8_t *out_packet) {
    if (data_len > 65536 - CCSDS_PRIMARY_HEADER_SIZE -
                   CCSDS_SECONDARY_HEADER_SIZE) {
        return -1;
    }

    CCSDS_PrimaryHeader *pri = (CCSDS_PrimaryHeader *)out_packet;
    pri->packet_id = htons(0x0800 | (apid & 0x07FF));  // 遥测包
    pri->packet_seq = htons(0xC000 | (sequence_count[apid]++ & 0x3FFF));
    pri->packet_len = htons(CCSDS_SECONDARY_HEADER_SIZE + data_len - 1);

    CCSDS_SecondaryHeader *sec = (CCSDS_SecondaryHeader *)(pri + 1);
    sec->timestamp_secs = htonl(get_spacecraft_time());
    sec->timestamp_subsecs = htons(get_subseconds());
    sec->source_id = htons(apid);

    memcpy(out_packet + CCSDS_PRIMARY_HEADER_SIZE +
           CCSDS_SECONDARY_HEADER_SIZE, data, data_len);

    return CCSDS_PRIMARY_HEADER_SIZE + CCSDS_SECONDARY_HEADER_SIZE +
           data_len;
}

// 遥控命令处理
void telecommand_handler(uint8_t *packet, size_t len) {
    if (len < CCSDS_PRIMARY_HEADER_SIZE) return;

    CCSDS_PrimaryHeader *pri = (CCSDS_PrimaryHeader *)packet;
    uint16_t apid = ntohs(pri->packet_id) & 0x07FF;

    // 验证校验和
    if (!verify_checksum(packet, len)) {
        send_rejection(apid, REJECT_CHECKSUM);
        return;
    }

    // 验证授权
    if (!check_authorization(apid, get_sender_id(packet))) {
        send_rejection(apid, REJECT_UNAUTHORIZED);
        return;
    }

    // 执行命令
    execute_command(apid, packet + CCSDS_PRIMARY_HEADER_SIZE,
                    len - CCSDS_PRIMARY_HEADER_SIZE);
}
