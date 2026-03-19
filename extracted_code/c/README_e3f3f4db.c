/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 259
 * Language: c
 * Block ID: e3f3f4db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 修复后的代码
int dtls1_process_heartbeat(SSL *s) {
    unsigned char *p = &s->s3->rrec.data[0];
    unsigned short payload;
    unsigned short max_payload;

    // 计算实际可用的最大 payload
    max_payload = s->s3->rrec.length - 1 - 2;  // 减去类型和长度字段

    n2s(p, payload);

    // 关键验证：确保请求的长度不超过实际数据
    if (payload > max_payload) {
        // 错误处理：请求长度超过实际数据
        return -1;
    }

    // 安全分配
    buffer = OPENSSL_malloc(1 + 2 + payload + padding);
    if (buffer == NULL) return -1;

    // 安全复制 - 现在 payload 已被验证
    memcpy(bp, p, payload);

    return 0;
}
