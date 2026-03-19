/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\README.md
 * Line: 710
 * Language: c
 * Block ID: 81c88db0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 两阶段提交 - 协调者实现

typedef enum {
    PC_PHASE_IDLE,
    PC_PHASE_PREPARE,
    PC_PHASE_COMMIT,
    PC_PHASE_ABORT
} pc_phase_t;

typedef struct {
    transaction_id_t tx_id;
    pc_phase_t phase;
    int participant_count;
    int* participant_status;  // 每个参与者的状态
    void* coordinator_log;    // 预写日志
} pc_coordinator_t;

// 第一阶段：准备
int pc_phase_prepare(pc_coordinator_t* coord, transaction_t* tx) {
    coord->phase = PC_PHASE_PREPARE;

    // 记录事务开始日志 (用于故障恢复)
    write_coordinator_log(coord, TX_START, tx);

    int yes_count = 0;
    for (int i = 0; i < coord->participant_count; i++) {
        // 发送 PREPARE 请求
        prepare_request_t req = {
            .tx_id = tx->id,
            .operations = tx->participants[i].operations
        };

        prepare_response_t resp = send_prepare(i, &req);

        if (resp.vote == VOTE_YES) {
            coord->participant_status[i] = STATUS_PREPARED;
            yes_count++;
        } else {
            coord->participant_status[i] = STATUS_VOTED_NO;
            // 收到一个 NO 就立即进入回滚
            return pc_phase_abort(coord, tx);
        }
    }

    // 所有参与者都同意，进入第二阶段
    if (yes_count == coord->participant_count) {
        return pc_phase_commit(coord, tx);
    }

    return -1;
}

// 第二阶段：提交
int pc_phase_commit(pc_coordinator_t* coord, transaction_t* tx) {
    coord->phase = PC_PHASE_COMMIT;

    // 先记录决定 (重要！这是 2PC 的要点)
    write_coordinator_log(coord, TX_COMMIT, tx);

    // 发送 COMMIT 给所有参与者
    for (int i = 0; i < coord->participant_count; i++) {
        commit_request_t req = { .tx_id = tx->id };
        send_commit(i, &req);
        coord->participant_status[i] = STATUS_COMMITTED;
    }

    // 等待 ACK (可异步)
    wait_for_acks(coord, tx->id);

    // 记录完成
    write_coordinator_log(coord, TX_COMPLETE, tx);
    coord->phase = PC_PHASE_IDLE;

    return 0;
}

// 参与者实现
typedef struct {
    transaction_id_t tx_id;
    void* undo_log;      // 回滚日志
    void* redo_log;      // 重做日志
    bool prepared;
} participant_tx_state_t;

prepare_response_t participant_handle_prepare(int participant_id,
                                               prepare_request_t* req) {
    prepare_response_t resp = { .tx_id = req->tx_id };

    // 检查能否执行操作
    if (!can_execute_operations(req->operations)) {
        resp.vote = VOTE_NO;
        return resp;
    }

    // 预执行操作，写入 undo/redo 日志
    participant_tx_state_t* state = malloc(sizeof(participant_tx_state_t));
    state->tx_id = req->tx_id;
    state->undo_log = create_undo_log(req->operations);
    state->redo_log = create_redo_log(req->operations);
    state->prepared = true;

    // 强制刷盘，确保持久化
    fsync(state->undo_log);
    fsync(state->redo_log);

    save_participant_state(state);

    resp.vote = VOTE_YES;
    return resp;
}

void participant_handle_commit(int participant_id, commit_request_t* req) {
    participant_tx_state_t* state = load_participant_state(req->tx_id);

    if (state && state->prepared) {
        // 应用 redo 日志完成事务
        apply_redo_log(state->redo_log);

        // 清理状态
        delete_participant_state(req->tx_id);
    }
}

void participant_handle_rollback(int participant_id,
                                  rollback_request_t* req) {
    participant_tx_state_t* state = load_participant_state(req->tx_id);

    if (state && state->prepared) {
        // 应用 undo 日志回滚事务
        apply_undo_log(state->undo_log);

        // 清理状态
        delete_participant_state(req->tx_id);
    }
}
