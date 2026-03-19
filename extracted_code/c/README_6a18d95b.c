/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\README.md
 * Line: 548
 * Language: c
 * Block ID: 6a18d95b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 会话类型：描述通信协议的类型
 */

#include <stdbool.h>

// 会话类型构造器（类型级编程概念）

// ?T.S  : 接收 T，然后继续 S
// !T.S  : 发送 T，然后继续 S
// ⊕{l:S} : 选择分支
// &{l:S} : 提供选择
// end   : 终止

// 文件服务器协议示例：
// Server = &{ READ: ?String.!Content.end
//            , WRITE: ?String.!Bool.end
//            , CLOSE: end }

// 客户端必须选择其中一个分支

// C 语言中的类型编码（简化）

typedef enum { S_RECV, S_SEND, S_CHOICE, S_OFFER, S_END } SessionOp;

typedef struct SessionType {
    SessionOp op;
    union {
        struct { int type_tag; struct SessionType *cont; } action;
        struct { int num_branches; struct SessionType **branches; } choice;
    };
} SessionType;

// 类型安全的通道（概念性）
typedef struct {
    int fd;
    const SessionType *current_type;
} SessionChannel;

// 发送操作（检查类型）
bool session_send(SessionChannel *ch, const void *data, size_t len)
{
    if (ch->current_type->op != S_SEND) {
        fprintf(stderr, "Type error: expected send capability\n");
        return false;
    }
    // 执行发送...
    ch->current_type = ch->current_type->action.cont;
    return true;
}

// 接收操作（检查类型）
bool session_recv(SessionChannel *ch, void *buffer, size_t len)
{
    if (ch->current_type->op != S_RECV) {
        fprintf(stderr, "Type error: expected recv capability\n");
        return false;
    }
    // 执行接收...
    ch->current_type = ch->current_type->action.cont;
    return true;
}
