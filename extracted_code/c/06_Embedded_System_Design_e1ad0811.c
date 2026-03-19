/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\06_Embedded_System_Design.md
 * Line: 163
 * Language: c
 * Block ID: e1ad0811
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// E2E保护
typedef struct {
    uint16_t data;
    uint8_t counter;
    uint8_t crc;
} ProtectedMessage;

bool receive_protected(ProtectedMessage *msg, uint16_t *out_data) {
    // CRC验证
    if (msg->crc != calculate_crc(msg)) {
        return false;
    }
    *out_data = msg->data;
    return true;
}
