/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\README.md
 * Line: 266
 * Language: c
 * Block ID: d993a94b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 工作流引擎核心数据结构

// 流程定义
typedef struct {
    char id[64];
    char name[256];
    int version;
    wf_node_t* nodes;
    size_t node_count;
    wf_transition_t* transitions;
    size_t transition_count;
} workflow_definition_t;

// 节点类型
typedef enum {
    NODE_START,      // 开始节点
    NODE_END,        // 结束节点
    NODE_TASK,       // 任务节点
    NODE_GATEWAY,    // 网关节点
    NODE_EVENT,      // 事件节点
    NODE_SUBPROCESS  // 子流程
} node_type_t;

// 工作流节点
typedef struct {
    char id[64];
    node_type_t type;
    char name[256];
    void (*execute)(wf_instance_t* inst, wf_node_t* node);
    // 任务特定配置
    union {
        struct {
            char assignee[128];
            char candidate_group[128];
            int timeout_seconds;
        } task;
        struct {
            int gateway_type;  // 排他/并行/包含
        } gateway;
    } config;
} wf_node_t;

// 流程实例
typedef struct {
    char id[64];
    workflow_definition_t* definition;
    wf_instance_state_t state;
    wf_node_t* current_nodes[16];  // 当前活动节点 (支持并行)
    size_t current_node_count;
    hash_table_t* variables;        // 流程变量
    time_t start_time;
    time_t end_time;
} wf_instance_t;

// 创建工作流实例
wf_instance_t* workflow_create_instance(workflow_definition_t* def) {
    wf_instance_t* inst = calloc(1, sizeof(wf_instance_t));
    uuid_generate(inst->id);
    inst->definition = def;
    inst->state = WF_STATE_ACTIVE;
    inst->variables = hash_table_create();
    inst->start_time = time(NULL);

    // 找到开始节点
    for (size_t i = 0; i < def->node_count; i++) {
        if (def->nodes[i].type == NODE_START) {
            workflow_enter_node(inst, &def->nodes[i]);
            break;
        }
    }

    return inst;
}

// 进入节点
void workflow_enter_node(wf_instance_t* inst, wf_node_t* node) {
    printf("Entering node: %s (%s)\n", node->name, node->id);

    // 添加到当前活动节点
    inst->current_nodes[inst->current_node_count++] = node;

    // 执行节点
    if (node->execute) {
        node->execute(inst, node);
    }

    // 自动流转 (非等待节点)
    if (node->type != NODE_TASK && node->type != NODE_EVENT) {
        workflow_leave_node(inst, node);
    }
}

// 离开节点
void workflow_leave_node(wf_instance_t* inst, wf_node_t* node) {
    // 从当前节点移除
    for (size_t i = 0; i < inst->current_node_count; i++) {
        if (inst->current_nodes[i] == node) {
            inst->current_nodes[i] =
                inst->current_nodes[--inst->current_node_count];
            break;
        }
    }

    // 查找并执行后续转换
    for (size_t i = 0; i < inst->definition->transition_count; i++) {
        wf_transition_t* trans = &inst->definition->transitions[i];
        if (strcmp(trans->from_node_id, node->id) == 0) {
            // 检查条件
            if (trans->condition == NULL ||
                evaluate_condition(inst, trans->condition)) {
                wf_node_t* next = find_node(inst->definition, trans->to_node_id);
                if (next) {
                    workflow_enter_node(inst, next);
                }
            }
        }
    }

    // 检查是否结束
    if (inst->current_node_count == 0) {
        inst->state = WF_STATE_COMPLETED;
        inst->end_time = time(NULL);
    }
}

// 完成任务
void workflow_complete_task(wf_instance_t* inst, const char* node_id,
                            const char* user_id) {
    wf_node_t* node = find_node(inst->definition, node_id);
    if (!node || node->type != NODE_TASK) return;

    // 验证权限
    if (!can_complete_task(inst, node, user_id)) {
        printf("Permission denied for user: %s\n", user_id);
        return;
    }

    printf("Task completed by: %s\n", user_id);
    workflow_leave_node(inst, node);
}

// 任务节点执行示例
void task_node_execute(wf_instance_t* inst, wf_node_t* node) {
    printf("Creating task: %s\n", node->name);
    printf("  Assignee: %s\n", node->config.task.assignee);
    printf("  Timeout: %d seconds\n", node->config.task.timeout_seconds);

    // 创建任务记录
    task_record_t* task = create_task(inst, node);

    // 发送通知
    if (strlen(node->config.task.assignee) > 0) {
        notify_user(node->config.task.assignee, task);
    }
}

// 网关节点执行示例
void gateway_node_execute(wf_instance_t* inst, wf_node_t* node) {
    switch (node->config.gateway.gateway_type) {
        case GATEWAY_EXCLUSIVE:
            // 排他网关：选择第一个满足条件的分支
            execute_exclusive_gateway(inst, node);
            break;
        case GATEWAY_PARALLEL:
            // 并行网关：同时激活所有分支
            execute_parallel_gateway(inst, node);
            break;
        case GATEWAY_INCLUSIVE:
            // 包含网关：激活所有满足条件的分支
            execute_inclusive_gateway(inst, node);
            break;
    }
}

// 并行网关实现
void execute_parallel_gateway(wf_instance_t* inst, wf_node_t* node) {
    // 找到所有出边
    wf_transition_t* outgoing[16];
    size_t outgoing_count = 0;

    for (size_t i = 0; i < inst->definition->transition_count; i++) {
        if (strcmp(inst->definition->transitions[i].from_node_id,
                   node->id) == 0) {
            outgoing[outgoing_count++] = &inst->definition->transitions[i];
        }
    }

    // 创建同步令牌
    sync_token_t* token = create_sync_token(inst, node->id, outgoing_count);

    // 同时激活所有分支
    for (size_t i = 0; i < outgoing_count; i++) {
        wf_node_t* next = find_node(inst->definition,
                                    outgoing[i]->to_node_id);
        if (next) {
            // 将令牌传递给子分支
            set_variable(inst, "sync_token", token);
            workflow_enter_node(inst, next);
        }
    }
}
