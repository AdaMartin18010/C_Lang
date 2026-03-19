/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\01_Microkernel_Architecture.md
 * Line: 50
 * Language: c
 * Block ID: 4cc37f59
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 消息传递 */
#define MAX_MSG_SIZE 4096

typedef struct {
    int source;      /* 发送者PID */
    int dest;        /* 接收者PID */
    int type;
    size_t size;
    char data[MAX_MSG_SIZE];
} message_t;

/* 同步发送 */
int send(int dest, message_t* msg) {
    msg->source = getpid();

    /* 陷入内核 */
    return syscall(SYS_SEND, dest, msg);
}

/* 接收 */
int receive(int source, message_t* msg) {
    return syscall(SYS_RECEIVE, source, msg);
}

/* 调用-返回 */
int rpc_call(int service, int func, void* args, void* result) {
    message_t msg = {
        .dest = service,
        .type = MSG_RPC_CALL,
        .data = {func, args}
    };

    send(service, &msg);

    /* 等待回复 */
    message_t reply;
    receive(service, &reply);

    memcpy(result, reply.data, reply.size);
    return 0;
}
