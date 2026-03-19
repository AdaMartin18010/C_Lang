/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\01_Mental_Models.md
 * Line: 217
 * Language: c
 * Block ID: c4fbdb78
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 状态可见的设计
typedef struct {
    enum { IDLE, CONNECTING, CONNECTED, ERROR } state;
    ConnectionStats stats;
    ErrorInfo last_error;
} Connection;

// 明确的状态查询
bool connection_is_ready(const Connection *c) {
    return c->state == CONNECTED;
}

// 明确的错误信息
const char *connection_error_string(const Connection *c) {
    return c->last_error.message;
}

// 反模式：隐藏状态
void connect_bad(const char *host);  // 连接失败怎么办？

// 正模式：明确状态
ConnectionResult connect_good(const char *host, Connection *out);
