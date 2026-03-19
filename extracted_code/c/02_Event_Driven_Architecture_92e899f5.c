/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\02_Event_Driven_Architecture.md
 * Line: 521
 * Language: c
 * Block ID: 92e899f5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* CQRS: 命令查询职责分离 */

/* 命令端 (写模型) */
typedef struct {
    int id;
    char name[64];
    double balance;
    int version;  /* 乐观锁 */
} account_write_model_t;

typedef enum {
    CMD_CREATE_ACCOUNT,
    CMD_DEPOSIT,
    CMD_WITHDRAW,
    CMD_TRANSFER
} command_type_t;

typedef struct {
    command_type_t type;
    int account_id;
    union {
        struct { char name[64]; } create;
        struct { double amount; } deposit;
        struct { double amount; } withdraw;
        struct { int to_id; double amount; } transfer;
    } data;
    int expected_version;  /* 用于乐观锁 */
} command_t;

/* 命令处理器 */
int command_handler(void* store, command_t* cmd) {
    switch (cmd->type) {
        case CMD_DEPOSIT: {
            account_write_model_t* acc = store_get(store, cmd->account_id);
            if (!acc) return -ENOENT;

            /* 版本检查 */
            if (acc->version != cmd->expected_version) {
                return -EAGAIN;  /* 并发冲突 */
            }

            acc->balance += cmd->data.deposit.amount;
            acc->version++;

            /* 发布事件 */
            event_t ev = {
                .type = EVENT_DEPOSITED,
                .account_id = cmd->account_id,
                .amount = cmd->data.deposit.amount,
                .version = acc->version
            };
            event_store_append(store, &ev);
            return 0;
        }
        /* ... */
    }
}

/* 查询端 (读模型) */
typedef struct {
    int id;
    char name[64];
    double balance;
    /* 可能冗余数据以优化查询 */
    double total_deposits;
    double total_withdrawals;
} account_read_model_t;

/* 物化视图 */
typedef struct {
    account_read_model_t* accounts;
    int count;
    mtx_t lock;
} account_query_store_t;

/* 查询 */
account_read_model_t* query_account(account_query_store_t* store, int id) {
    mtx_lock(&store->lock);
    /* 查找并返回 */
    for (int i = 0; i < store->count; i++) {
        if (store->accounts[i].id == id) {
            mtx_unlock(&store->lock);
            return &store->accounts[i];
        }
    }
    mtx_unlock(&store->lock);
    return NULL;
}

/* 查询所有余额大于X的账户 */
account_read_model_t** query_accounts_by_balance(
    account_query_store_t* store,
    double min_balance,
    int* count) {
    /* 已优化的读模型可以直接查询 */
}

/* 事件处理器更新读模型 */
void on_event(event_t* ev) {
    switch (ev->type) {
        case EVENT_DEPOSITED:
            update_read_model_deposit(ev->account_id, ev->amount);
            break;
        case EVENT_WITHDRAWN:
            update_read_model_withdraw(ev->account_id, ev->amount);
            break;
    }
}
