/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 553
 * Language: c
 * Block ID: c2847fc9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 重构后：清晰的分层架构

// 1. 协议定义层
#pragma pack(push, 1)
typedef struct {
    uint8_t type;
    uint8_t flags;
    uint16_t length;
} packet_header_t;

typedef struct {
    packet_header_t header;
    uint32_t id;
} id_packet_t;
#pragma pack(pop)

// 2. 错误码定义
typedef enum {
    PARSE_OK = 0,
    PARSE_ERROR_TOO_SHORT = -1,
    PARSE_ERROR_INVALID_TYPE = -2,
    PARSE_ERROR_LENGTH_MISMATCH = -3
} parse_result_t;

// 3. 处理器接口
typedef parse_result_t (*packet_handler_t)(const uint8_t* data, size_t len, size_t* consumed);

// 4. 具体处理器
static parse_result_t handle_data_packet(const uint8_t* data, size_t len, size_t* consumed) {
    const packet_header_t* hdr = (const packet_header_t*)data;
    size_t total_len = sizeof(packet_header_t) + ntohs(hdr->length);

    if (len < total_len) {
        return PARSE_ERROR_TOO_SHORT;
    }

    // 处理数据
    process_payload(data + sizeof(packet_header_t), ntohs(hdr->length));

    *consumed = total_len;
    return PARSE_OK;
}

static parse_result_t handle_id_packet(const uint8_t* data, size_t len, size_t* consumed) {
    if (len < sizeof(id_packet_t)) {
        return PARSE_ERROR_TOO_SHORT;
    }

    const id_packet_t* pkt = (const id_packet_t*)data;
    uint32_t id = ntohl(pkt->id);
    process_id(id);

    *consumed = sizeof(id_packet_t);
    return PARSE_OK;
}

// 5. 分发表
typedef struct {
    uint8_t type;
    size_t min_size;
    packet_handler_t handler;
    const char* name;
} packet_type_entry_t;

static const packet_type_entry_t packet_types[] = {
    { 1, sizeof(packet_header_t), handle_data_packet, "DATA" },
    { 2, sizeof(id_packet_t), handle_id_packet, "ID" },
    { 0, 0, NULL, NULL }
};

// 6. 主解析器
parse_result_t parse_packet_v2(const uint8_t* data, size_t len, size_t* consumed) {
    if (len < sizeof(packet_header_t)) {
        return PARSE_ERROR_TOO_SHORT;
    }

    const packet_header_t* hdr = (const packet_header_t*)data;
    uint8_t type = hdr->type;

    // 查找处理器
    for (const packet_type_entry_t* entry = packet_types; entry->handler; entry++) {
        if (entry->type == type) {
            if (len < entry->min_size) {
                return PARSE_ERROR_TOO_SHORT;
            }
            return entry->handler(data, len, consumed);
        }
    }

    return PARSE_ERROR_INVALID_TYPE;
}
