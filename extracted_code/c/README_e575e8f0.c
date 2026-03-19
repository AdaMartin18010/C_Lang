/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\README.md
 * Line: 948
 * Language: c
 * Block ID: e575e8f0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用事件协调的 Saga 实现

typedef enum {
    SAGA_EVENT_STARTED,
    SAGA_EVENT_STEP_COMPLETED,
    SAGA_EVENT_STEP_FAILED,
    SAGA_EVENT_COMPENSATION_COMPLETED,
    SAGA_EVENT_COMPLETED
} saga_event_type_t;

typedef struct {
    saga_event_type_t type;
    char saga_id[64];
    int step_index;
    char error_message[256];
} saga_event_t;

// Saga 协调器
typedef struct {
    char id[64];
    saga_definition_t* definition;
    saga_status_t status;
    int current_step;
    event_bus_t* event_bus;
} saga_orchestrator_t;

// 启动 Saga
void saga_start(saga_orchestrator_t* orchestrator) {
    saga_event_t event = {
        .type = SAGA_EVENT_STARTED,
        .saga_id = orchestrator->id,
        .step_index = 0
    };
    event_bus_publish(orchestrator->event_bus, &event);

    // 执行第一步
    saga_execute_step(orchestrator, 0);
}

// 执行步骤
void saga_execute_step(saga_orchestrator_t* orchestrator, int step_index) {
    saga_step_t* step = &orchestrator->definition->steps[step_index];

    // 发送执行命令事件
    command_event_t cmd = {
        .type = step->action_command,
        .saga_id = orchestrator->id,
        .step_index = step_index,
        .payload = step->context
    };
    event_bus_publish(orchestrator->event_bus, &cmd);
}

// Saga 事件处理器
void saga_event_handler(event_t* event, void* context) {
    saga_orchestrator_t* orchestrator = (saga_orchestrator_t*)context;
    saga_event_t* saga_event = (saga_event_t*)event->payload;

    switch (saga_event->type) {
        case SAGA_EVENT_STEP_COMPLETED:
            if (saga_event->step_index == orchestrator->current_step) {
                orchestrator->current_step++;

                if (orchestrator->current_step < orchestrator->definition->step_count) {
                    // 执行下一步
                    saga_execute_step(orchestrator, orchestrator->current_step);
                } else {
                    // Saga 完成
                    saga_event_t completed = {
                        .type = SAGA_EVENT_COMPLETED,
                        .saga_id = orchestrator->id
                    };
                    event_bus_publish(orchestrator->event_bus, &completed);
                    orchestrator->status = SAGA_COMPLETED;
                }
            }
            break;

        case SAGA_EVENT_STEP_FAILED:
            // 开始补偿
            orchestrator->status = SAGA_COMPENSATING;
            saga_start_compensation(orchestrator, saga_event->step_index);
            break;

        case SAGA_EVENT_COMPENSATION_COMPLETED:
            if (orchestrator->compensation_index >= 0) {
                saga_continue_compensation(orchestrator);
            } else {
                orchestrator->status = SAGA_FAILED;
            }
            break;
    }
}
