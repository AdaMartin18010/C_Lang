/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\README.md
 * Line: 572
 * Language: c
 * Block ID: f0c73fd4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Rete 网络节点类型
typedef enum {
    RETE_ROOT,           // 根节点
    RETE_TYPE,           // 类型节点
    RETE_ALPHA,          // Alpha 节点 (单条件测试)
    RETE_BETA,           // Beta 节点 (连接节点)
    RETE_TERMINAL        // 终端节点 (规则)
} rete_node_type_t;

// Rete 网络节点
typedef struct rete_node {
    rete_node_type_t type;
    char name[128];

    // Alpha 节点：条件测试
    struct {
        char field[64];
        int op;           // EQ, GT, LT, NE, etc.
        value_t value;
    } alpha_test;

    // Beta 节点：连接条件
    struct {
        struct rete_node* left_input;
        struct rete_node* right_input;
        join_constraint_t* constraints;
    } beta_join;

    // 子节点
    struct rete_node** children;
    size_t child_count;

    // 记忆
    token_list_t* tokens;  // Beta 记忆
    fact_list_t* facts;    // Alpha 记忆
} rete_node_t;

// 事实
typedef struct {
    char type[64];
    char id[64];
    hash_table_t* fields;
    time_t timestamp;
} fact_t;

// 规则
typedef struct {
    char name[128];
    int salience;           // 优先级
    condition_t* conditions;
    size_t condition_count;
    action_t* actions;
    size_t action_count;
    rete_node_t* terminal_node;
} rule_t;

// 创建 Rete 网络
rete_network_t* rete_create(void) {
    rete_network_t* net = calloc(1, sizeof(rete_network_t));
    net->root = calloc(1, sizeof(rete_node_t));
    net->root->type = RETE_ROOT;
    return net;
}

// 添加规则到网络
void rete_add_rule(rete_network_t* net, rule_t* rule) {
    rete_node_t* current = net->root;

    // 为每个条件创建/复用 Alpha 节点
    for (size_t i = 0; i < rule->condition_count; i++) {
        condition_t* cond = &rule->conditions[i];

        // 查找或创建类型节点
        rete_node_t* type_node = find_or_create_type_node(current, cond->fact_type);

        // 查找或创建 Alpha 节点
        rete_node_t* alpha_node = find_or_create_alpha_node(type_node, cond);

        // 创建 Beta 节点 (除第一个条件外)
        if (i > 0) {
            rete_node_t* beta_node = create_beta_node(current, alpha_node, cond);
            current = beta_node;
        } else {
            current = alpha_node;
        }
    }

    // 创建终端节点
    rete_node_t* terminal = create_terminal_node(current, rule);
    rule->terminal_node = terminal;
}

// 插入事实
void rete_assert_fact(rete_network_t* net, fact_t* fact) {
    // 找到对应的类型节点
    rete_node_t* type_node = find_type_node(net->root, fact->type);
    if (!type_node) return;

    // 传播到所有子 Alpha 节点
    for (size_t i = 0; i < type_node->child_count; i++) {
        rete_node_t* alpha = type_node->children[i];

        // 测试 Alpha 条件
        if (evaluate_alpha_test(alpha, fact)) {
            // 添加到 Alpha 记忆
            add_to_alpha_memory(alpha, fact);

            // 传播到子节点
            propagate_to_children(alpha, fact);
        }
    }
}

// Beta 节点连接
void beta_join(rete_node_t* beta_node, token_t* left_token, fact_t* right_fact) {
    // 测试连接约束
    for (size_t i = 0; i < beta_node->beta_join.constraint_count; i++) {
        if (!evaluate_join_constraint(&beta_node->beta_join.constraints[i],
                                       left_token, right_fact)) {
            return;
        }
    }

    // 创建新 token
    token_t* new_token = create_token(left_token, right_fact);
    add_to_beta_memory(beta_node, new_token);

    // 传播到子节点
    for (size_t i = 0; i < beta_node->child_count; i++) {
        rete_node_t* child = beta_node->children[i];
        if (child->type == RETE_TERMINAL) {
            // 触发规则
            activate_rule(child->rule, new_token);
        } else if (child->type == RETE_BETA) {
            // 继续连接
            for (size_t j = 0; j < child->beta_join.right_input->fact_count; j++) {
                beta_join(child, new_token, child->beta_join.right_input->facts[j]);
            }
        }
    }
}
