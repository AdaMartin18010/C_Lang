/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 440
 * Language: c
 * Block ID: be611259
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 - 未验证的输入 */
void process_packet(char* data, size_t len) {
    uint32_t size = *(uint32_t*)data;  /* 直接解引用 */
    char* payload = data + 4;
    process(payload, size);  /* 可能越界 */
}

/* 符合规范 - 严格验证 */
void process_packet(const uint8_t* data, size_t len) {
    if (len < 4) return;  /* 最小长度检查 */

    uint32_t size;
    memcpy(&size, data, sizeof(size));
    size = ntohl(size);  /* 字节序转换 */

    if (size > len - 4 || size > MAX_PAYLOAD_SIZE) {
        return;  /* 大小验证失败 */
    }

    process(data + 4, size);
}
