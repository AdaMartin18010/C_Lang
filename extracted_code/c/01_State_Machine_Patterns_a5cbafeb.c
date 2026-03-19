/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\01_State_Machine_Patterns.md
 * Line: 177
 * Language: c
 * Block ID: a5cbafeb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 表格驱动状态机 */

typedef struct {
    int current_state;
    int event;
    int next_state;
    void (*action)(void* ctx);
    int guard;  /* 条件 */
} transition_t;

/* TCP状态机示例 */
enum { CLOSED, LISTEN, SYN_SENT, SYN_RECEIVED, ESTABLISHED,
       FIN_WAIT_1, FIN_WAIT_2, TIME_WAIT };
enum { OPEN, SEND, RECEIVE, CLOSE, TIMEOUT };

void action_passive_open(void* ctx) {
    /* 绑定端口，开始监听 */
}

void action_send_syn(void* ctx) {
    /* 发送SYN包 */
}

void action_send_syn_ack(void* ctx) {
    /* 发送SYN-ACK */
}

void action_send_ack(void* ctx) {
    /* 发送ACK */
}

/* 状态转换表 */
static const transition_t tcp_fsm[] = {
    /* 当前状态     事件         下一状态        动作           */
    { CLOSED,       OPEN,        LISTEN,         action_passive_open },
    { CLOSED,       SEND,        SYN_SENT,       action_send_syn },
    { LISTEN,       RECEIVE,     SYN_RECEIVED,   action_send_syn_ack },
    { SYN_SENT,     RECEIVE,     ESTABLISHED,    action_send_ack },
    { SYN_RECEIVED, RECEIVE,     ESTABLISHED,    action_send_ack },
    { FIN_WAIT_1,   RECEIVE,     FIN_WAIT_2,     NULL },
    { FIN_WAIT_2,   RECEIVE,     TIME_WAIT,      NULL },
    { TIME_WAIT,    TIMEOUT,     CLOSED,         NULL },
    { -1, -1, -1, NULL }  /* 结束 */
};

void process_event(int* current, int event, void* ctx) {
    for (int i = 0; tcp_fsm[i].current_state != -1; i++) {
        if (tcp_fsm[i].current_state == *current &&
            tcp_fsm[i].event == event) {

            /* 执行动作 */
            if (tcp_fsm[i].action) {
                tcp_fsm[i].action(ctx);
            }

            /* 转换状态 */
            *current = tcp_fsm[i].next_state;
            printf("State: %d -> %d\n", tcp_fsm[i].current_state, *current);
            return;
        }
    }

    printf("Invalid transition: state=%d, event=%d\n", *current, event);
}
