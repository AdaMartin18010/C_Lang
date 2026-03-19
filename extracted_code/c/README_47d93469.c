/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\README.md
 * Line: 472
 * Language: c
 * Block ID: 47d93469
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 审批流程定义示例
workflow_definition_t* create_approval_workflow() {
    workflow_definition_t* def = calloc(1, sizeof(workflow_definition_t));
    strcpy(def->id, "approval_flow_v1");
    strcpy(def->name, "费用报销审批流程");
    def->version = 1;

    // 定义节点
    def->nodes = calloc(6, sizeof(wf_node_t));
    def->node_count = 6;

    // 开始节点
    def->nodes[0] = (wf_node_t){
        .id = "start",
        .type = NODE_START,
        .name = "开始",
        .execute = start_node_execute
    };

    // 提交申请
    def->nodes[1] = (wf_node_t){
        .id = "submit",
        .type = NODE_TASK,
        .name = "提交报销申请",
        .execute = task_node_execute,
        .config.task.assignee = "#{applicant}",
        .config.task.timeout_seconds = 86400
    };

    // 经理审批网关
    def->nodes[2] = (wf_node_t){
        .id = "manager_gateway",
        .type = NODE_GATEWAY,
        .name = "经理审批",
        .execute = gateway_node_execute,
        .config.gateway.gateway_type = GATEWAY_EXCLUSIVE
    };

    // 财务审批
    def->nodes[3] = (wf_node_t){
        .id = "finance_approve",
        .type = NODE_TASK,
        .name = "财务审批",
        .execute = task_node_execute,
        .config.task.candidate_group = "finance_dept"
    };

    // 支付处理
    def->nodes[4] = (wf_node_t){
        .id = "payment",
        .type = NODE_TASK,
        .name = "支付处理",
        .execute = task_node_execute,
        .config.task.candidate_group = "payment_system"
    };

    // 结束节点
    def->nodes[5] = (wf_node_t){
        .id = "end",
        .type = NODE_END,
        .name = "流程结束",
        .execute = end_node_execute
    };

    // 定义转换
    def->transitions = calloc(5, sizeof(wf_transition_t));
    def->transition_count = 5;

    def->transitions[0] = (wf_transition_t){"start", "submit", NULL};
    def->transitions[1] = (wf_transition_t){"submit", "manager_gateway", NULL};
    def->transitions[2] = (wf_transition_t){"manager_gateway", "finance_approve",
                                            "#{amount} < 10000"};
    def->transitions[3] = (wf_transition_t){"finance_approve", "payment", NULL};
    def->transitions[4] = (wf_transition_t){"payment", "end", NULL};

    return def;
}
