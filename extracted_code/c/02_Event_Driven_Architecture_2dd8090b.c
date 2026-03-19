/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\02_Event_Driven_Architecture.md
 * Line: 374
 * Language: c
 * Block ID: 2dd8090b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Saga: 分布式事务协调 */

typedef enum {
    SAGA_STEP_PENDING,
    SAGA_STEP_SUCCEEDED,
    SAGA_STEP_FAILED,
    SAGA_STEP_COMPENSATED
} saga_step_status_t;

typedef struct saga_step {
    int id;
    char name[64];

    /* 执行动作 */
    int (*action)(void* context, void** result);

    /* 补偿动作 */
    int (*compensate)(void* context);

    saga_step_status_t status;
    void* result;
    void* context;
} saga_step_t;

typedef struct {
    saga_step_t* steps;
    int step_count;
    int current_step;

    void (*on_success)(void* result);
    void (*on_failure)(int failed_step, void* error);
} saga_t;

/* 编排式Saga (Orchestration) */
typedef struct {
    saga_t* saga;
    void* saga_context;
} saga_orchestrator_t;

void saga_execute(saga_orchestrator_t* orch) {
    saga_t* s = orch->saga;

    for (int i = 0; i < s->step_count; i++) {
        s->current_step = i;
        saga_step_t* step = &s->steps[i];

        step->status = SAGA_STEP_PENDING;
        int rc = step->action(step->context, &step->result);

        if (rc == 0) {
            step->status = SAGA_STEP_SUCCEEDED;
        } else {
            step->status = SAGA_STEP_FAILED;

            /* 回滚 */
            for (int j = i - 1; j >= 0; j--) {
                if (s->steps[j].compensate) {
                    s->steps[j].compensate(s->steps[j].context);
                    s->steps[j].status = SAGA_STEP_COMPENSATED;
                }
            }

            if (s->on_failure) {
                s->on_failure(i, step->result);
            }
            return;
        }
    }

    if (s->on_success) {
        s->on_success(s->steps[s->step_count - 1].result);
    }
}

/* 订单Saga示例 */
typedef struct {
    int order_id;
    int inventory_reserved;
    int payment_made;
    int notification_sent;
} order_context_t;

int reserve_inventory(void* ctx, void** result) {
    order_context_t* c = ctx;
    /* 调用库存服务 */
    c->inventory_reserved = 1;
    return 0;
}

int cancel_inventory(void* ctx) {
    order_context_t* c = ctx;
    if (c->inventory_reserved) {
        /* 释放库存 */
    }
    return 0;
}

int process_payment(void* ctx, void** result) {
    order_context_t* c = ctx;
    /* 调用支付服务 */
    c->payment_made = 1;
    return 0;
}

int refund_payment(void* ctx) {
    order_context_t* c = ctx;
    if (c->payment_made) {
        /* 退款 */
    }
    return 0;
}

saga_t* create_order_saga(order_context_t* ctx) {
    saga_t* saga = calloc(1, sizeof(saga_t));
    saga->steps = calloc(3, sizeof(saga_step_t));
    saga->step_count = 3;

    saga->steps[0] = (saga_step_t){
        .name = "reserve_inventory",
        .action = reserve_inventory,
        .compensate = cancel_inventory,
        .context = ctx
    };

    saga->steps[1] = (saga_step_t){
        .name = "process_payment",
        .action = process_payment,
        .compensate = refund_payment,
        .context = ctx
    };

    saga->steps[2] = (saga_step_t){
        .name = "send_notification",
        .action = send_notification,
        .compensate = NULL,  /* 不需要补偿 */
        .context = ctx
    };

    return saga;
}
