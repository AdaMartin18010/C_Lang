/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\02_Rule_Engine.md
 * Line: 198
 * Language: c
 * Block ID: 34d61aac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 简单工作流引擎 */

typedef enum {
    NODE_START,
    NODE_END,
    NODE_TASK,          /* 人工任务 */
    NODE_SERVICE,       /* 自动服务 */
    NODE_GATEWAY_EXCLUSIVE,  /* 排他网关 */
    NODE_GATEWAY_PARALLEL    /* 并行网关 */
} node_type_t;

typedef struct workflow_node {
    char id[32];
    node_type_t type;
    char name[64];

    /* 执行函数 */
    int (*execute)(struct workflow_node* node, void* context);

    /* 流转条件 */
    struct {
        char condition[256];  /* 表达式 */
        struct workflow_node* target;
    } transitions[4];
    int transition_count;
} workflow_node_t;

typedef struct {
    char id[32];
    char name[64];
    workflow_node_t* nodes;
    int node_count;
    workflow_node_t* start_node;
} workflow_definition_t;

typedef struct {
    workflow_definition_t* definition;
    workflow_node_t* current_node;
    void* context;
    int status;  /* 0=running, 1=completed, -1=error */
} workflow_instance_t;

workflow_instance_t* workflow_start(workflow_definition_t* def, void* ctx) {
    workflow_instance_t* inst = malloc(sizeof(workflow_instance_t));
    inst->definition = def;
    inst->current_node = def->start_node;
    inst->context = ctx;
    inst->status = 0;
    return inst;
}

/* 执行一步 */
int workflow_step(workflow_instance_t* inst) {
    if (inst->status != 0 || !inst->current_node) {
        return -1;
    }

    workflow_node_t* node = inst->current_node;
    printf("Executing: %s\n", node->name);

    /* 执行节点 */
    int rc = node->execute(node, inst->context);
    if (rc != 0) {
        inst->status = -1;
        return rc;
    }

    /* 选择下一个节点 */
    if (node->type == NODE_END) {
        inst->status = 1;
        inst->current_node = NULL;
        return 0;
    }

    /* 简单流转: 选择第一个满足条件的 */
    for (int i = 0; i < node->transition_count; i++) {
        /* 评估条件... */
        bool condition_met = true;

        if (condition_met) {
            inst->current_node = node->transitions[i].target;
            return 0;
        }
    }

    return -1;  /* 无法流转 */
}

/* 运行到完成 */
void workflow_run(workflow_instance_t* inst) {
    while (inst->status == 0) {
        if (workflow_step(inst) != 0) break;
    }
}

/* 示例: 请假流程 */
/* START -> 提交申请 -> [经理审批] -> [HR审批] -> END */

int task_submit(void* node, void* ctx) {
    printf("员工提交请假申请\n");
    return 0;
}

int task_manager_approve(void* node, void* ctx) {
    /* 等待人工审批... */
    printf("等待经理审批...\n");
    /* 实际应该是异步的 */
    return 0;
}

workflow_definition_t leave_workflow = {
    .name = "Leave Approval",
    .nodes = (workflow_node_t[]){
        {
            .id = "start",
            .type = NODE_START,
            .name = "Start",
            .execute = lambda(int, (void* n, void* c) { return 0; }),
            .transitions = {{.target = NULL}},  /* 设置到submit */
            .transition_count = 1
        },
        {
            .id = "submit",
            .type = NODE_TASK,
            .name = "Submit Leave Request",
            .execute = task_submit,
            .transitions = {{.target = NULL}},
            .transition_count = 1
        },
        {
            .id = "manager_approval",
            .type = NODE_TASK,
            .name = "Manager Approval",
            .execute = task_manager_approve,
            .transitions = {
                {.condition = "approved", .target = NULL},
                {.condition = "rejected", .target = NULL}
            },
            .transition_count = 2
        }
    }
};
