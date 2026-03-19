/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\README.md
 * Line: 871
 * Language: c
 * Block ID: 92f17f67
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Saga 模式实现

typedef enum {
    SAGA_STEP_PENDING,
    SAGA_STEP_SUCCEEDED,
    SAGA_STEP_FAILED,
    SAGA_STEP_COMPENSATED
} saga_step_status_t;

typedef struct {
    const char* name;
    int (*action)(void* context);       // 正向操作
    int (*compensate)(void* context);   // 补偿操作
    void* context;
    saga_step_status_t status;
} saga_step_t;

typedef struct {
    saga_step_t* steps;
    size_t step_count;
    size_t current_step;
    saga_status_t status;
} saga_t;

// 执行 Saga
int saga_execute(saga_t* saga) {
    saga->status = SAGA_RUNNING;

    for (size_t i = 0; i < saga->step_count; i++) {
        saga->current_step = i;
        saga_step_t* step = &saga->steps[i];

        step->status = SAGA_STEP_PENDING;

        // 执行正向操作
        int result = step->action(step->context);

        if (result == 0) {
            step->status = SAGA_STEP_SUCCEEDED;
        } else {
            step->status = SAGA_STEP_FAILED;
            saga->status = SAGA_FAILED;

            // 执行补偿操作
            return saga_compensate(saga, i);
        }
    }

    saga->status = SAGA_SUCCEEDED;
    return 0;
}

// 补偿操作
int saga_compensate(saga_t* saga, size_t failed_step) {
    // 对已经成功执行的步骤逆序执行补偿
    for (int i = (int)failed_step - 1; i >= 0; i--) {
        saga_step_t* step = &saga->steps[i];

        if (step->status == SAGA_STEP_SUCCEEDED) {
            int result = step->compensate(step->context);

            if (result == 0) {
                step->status = SAGA_STEP_COMPENSATED;
            } else {
                // 补偿失败，需要人工介入或重试
                saga->status = SAGA_COMPENSATION_FAILED;
                return -1;
            }
        }
    }

    saga->status = SAGA_COMPENSATED;
    return 0;
}

// 订单处理 Saga 示例
saga_t* create_order_saga(order_context_t* ctx) {
    saga_step_t steps[] = {
        {
            .name = "create_order",
            .action = step_create_order,
            .compensate = compensate_create_order,
            .context = ctx
        },
        {
            .name = "reserve_inventory",
            .action = step_reserve_inventory,
            .compensate = compensate_reserve_inventory,
            .context = ctx
        },
        {
            .name = "process_payment",
            .action = step_process_payment,
            .compensate = compensate_process_payment,
            .context = ctx
        },
        {
            .name = "ship_order",
            .action = step_ship_order,
            .compensate = compensate_ship_order,
            .context = ctx
        }
    };

    saga_t* saga = malloc(sizeof(saga_t));
    saga->steps = steps;
    saga->step_count = 4;
    saga->current_step = 0;
    saga->status = SAGA_PENDING;

    return saga;
}

// 具体步骤实现示例
int step_reserve_inventory(void* context) {
    order_context_t* ctx = (order_context_t*)context;

    // 调用库存服务预留库存
    inventory_response_t resp = inventory_service_reserve(
        ctx->product_id,
        ctx->quantity
    );

    if (resp.success) {
        ctx->reservation_id = resp.reservation_id;
        return 0;
    }
    return -1;
}

int compensate_reserve_inventory(void* context) {
    order_context_t* ctx = (order_context_t*)context;

    // 释放预留的库存
    inventory_service_release(ctx->reservation_id);
    return 0;
}
