/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\02_Paxos_Implementation.md
 * Line: 170
 * Language: c
 * Block ID: ce4f9405
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// paxos_acceptor.c
#include "paxos_types.h"
#include <string.h>
#include <stdio.h>

// 初始化Acceptor状态
void acceptor_init(AcceptorState *state) {
    memset(state, 0, sizeof(AcceptorState));
    state->promised_num = 0;
    state->accepted_num = 0;
    pthread_mutex_init(&state->lock, NULL);
}

// 处理Prepare请求
// 返回: true表示承诺，false表示拒绝
bool acceptor_handle_prepare(AcceptorState *state,
                              const PaxosMessage *request,
                              PaxosMessage *response) {
    pthread_mutex_lock(&state->lock);

    printf("[Acceptor] Received Prepare(n=%lu, round=%u)\n",
           request->proposal_num, GET_ROUND(request->proposal_num));

    // 只接受编号更大的提案
    if (request->proposal_num > state->promised_num) {
        state->promised_num = request->proposal_num;

        // 构造Promise响应
        response->type = MSG_PROMISE;
        response->proposal_num = request->proposal_num;
        response->accepted_num = state->accepted_num;

        // 如果已经接受过值，告知Proposer
        if (state->accepted_num > 0) {
            memcpy(&response->value, &state->accepted_value, sizeof(PaxosValue));
            printf("[Acceptor] Promise with previously accepted value (n=%lu)\n",
                   state->accepted_num);
        } else {
            memset(&response->value, 0, sizeof(PaxosValue));
            printf("[Acceptor] Promise (no previous value)\n");
        }

        pthread_mutex_unlock(&state->lock);
        return true;
    }

    printf("[Acceptor] Reject Prepare (promised n=%lu)\n",
           state->promised_num);
    pthread_mutex_unlock(&state->lock);
    return false;
}

// 处理Accept请求
// 返回: true表示接受，false表示拒绝
bool acceptor_handle_accept(AcceptorState *state,
                            const PaxosMessage *request,
                            PaxosMessage *response) {
    pthread_mutex_lock(&state->lock);

    printf("[Acceptor] Received Accept(n=%lu, v=%.*s)\n",
           request->proposal_num,
           (int)request->value.value_len,
           request->value.data);

    // 必须不小于已承诺的编号（允许等于，因为可能是自己的提案）
    if (request->proposal_num >= state->promised_num) {
        // 更新状态
        state->promised_num = request->proposal_num;
        state->accepted_num = request->proposal_num;
        memcpy(&state->accepted_value, &request->value, sizeof(PaxosValue));

        // 构造Accepted响应
        response->type = MSG_ACCEPTED;
        response->proposal_num = request->proposal_num;
        response->accepted_num = request->proposal_num;
        memcpy(&response->value, &request->value, sizeof(PaxosValue));

        printf("[Acceptor] Accepted value (n=%lu)\n", request->proposal_num);

        pthread_mutex_unlock(&state->lock);
        return true;
    }

    printf("[Acceptor] Reject Accept (promised n=%lu)\n",
           state->promised_num);
    pthread_mutex_unlock(&state->lock);
    return false;
}

// 获取已接受的值
bool acceptor_get_accepted(AcceptorState *state,
                           proposal_number_t *num,
                           PaxosValue *value) {
    pthread_mutex_lock(&state->lock);

    if (state->accepted_num > 0) {
        *num = state->accepted_num;
        memcpy(value, &state->accepted_value, sizeof(PaxosValue));
        pthread_mutex_unlock(&state->lock);
        return true;
    }

    pthread_mutex_unlock(&state->lock);
    return false;
}
