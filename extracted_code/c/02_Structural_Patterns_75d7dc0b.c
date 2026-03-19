/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\02_Structural_Patterns.md
 * Line: 286
 * Language: c
 * Block ID: 75d7dc0b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 本地接口 */
typedef struct {
    int (*add)(void*, int, int);
    int (*multiply)(void*, int, int);
} calculator_t;

/* 远程代理 */
typedef struct {
    calculator_t interface;
    int socket_fd;
    char server_addr[64];
    int port;
} remote_calculator_t;

int remote_add(void* self, int a, int b) {
    remote_calculator_t* proxy = self;

    /* 序列化请求 */
    rpc_request_t req = {
        .method = "add",
        .arg1 = a,
        .arg2 = b
    };
    send(proxy->socket_fd, &req, sizeof(req), 0);

    /* 接收响应 */
    rpc_response_t resp;
    recv(proxy->socket_fd, &resp, sizeof(resp), 0);
    return resp.result;
}

/* 调用方完全透明 */
void use_calculator(calculator_t* calc) {
    int result = calc->add(calc, 2, 3);  /* 不知道是本地还是远程 */
}
