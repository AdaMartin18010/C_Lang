/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\03_Actor_Model_C.md
 * Line: 256
 * Language: c
 * Block ID: 2af2a43e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 消息类型 */
typedef enum {
    MSG_USER = 0,           /* 用户消息 */
    MSG_SPAWN,              /* 创建子Actor */
    MSG_KILL,               /* 终止Actor */
    MSG_LINK,               /* 建立监督关系 */
    MSG_UNLINK,             /* 解除监督 */
    MSG_EXIT,               /* Actor退出通知 */
    MSG_SUPERVISE,          /* 监督指令 */
} msg_type_t;

/* 消息结构 */
typedef struct message {
    msg_type_t type;
    aid_t sender;           /* 发送者 */
    aid_t target;           /* 目标 */

    union {
        /* 用户数据 */
        struct {
            int user_type;
            void* data;
            size_t size;
        } user;

        /* 监督数据 */
        struct {
            aid_t child;
            int reason;     /* 退出原因 */
        } exit;

        /* 创建Actor */
        struct {
            void* (*behaviour)(void*);  /* 行为函数 */
            void* arg;
            actor_ref_t* reply_to;
        } spawn;
    } payload;
} message_t;

/* 创建消息 */
message_t make_message(msg_type_t type, aid_t sender, aid_t target) {
    return (message_t){
        .type = type,
        .sender = sender,
        .target = target,
        .payload = {0}
    };
}
