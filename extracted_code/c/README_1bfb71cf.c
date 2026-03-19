/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\README.md
 * Line: 718
 * Language: c
 * Block ID: 1bfb71cf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简化版规则引擎 - 适合中小规模应用

typedef bool (*condition_fn_t)(fact_t* facts, size_t fact_count);
typedef void (*action_fn_t)(fact_t* facts, size_t fact_count, void* context);

typedef struct {
    char name[128];
    int priority;
    condition_fn_t when;
    action_fn_t then;
} simple_rule_t;

typedef struct {
    simple_rule_t* rules;
    size_t rule_count;
    fact_t* facts;
    size_t fact_count;
    void* context;
} simple_engine_t;

// 执行规则引擎
void engine_execute(simple_engine_t* engine) {
    bool any_fired = true;

    while (any_fired) {
        any_fired = false;

        // 按优先级排序评估规则
        for (size_t i = 0; i < engine->rule_count; i++) {
            simple_rule_t* rule = &engine->rules[i];

            if (rule->when(engine->facts, engine->fact_count)) {
                printf("Rule fired: %s\n", rule->name);
                rule->then(engine->facts, engine->fact_count, engine->context);
                any_fired = true;

                // 重新评估 (事实可能已改变)
                break;
            }
        }
    }
}

// 示例：折扣规则
bool discount_condition(fact_t* facts, size_t count) {
    // 查找订单事实
    fact_t* order = find_fact_by_type(facts, count, "Order");
    if (!order) return false;

    double amount = get_fact_double(order, "amount");
    return amount > 1000;
}

void discount_action(fact_t* facts, size_t count, void* context) {
    fact_t* order = find_fact_by_type(facts, count, "Order");
    double amount = get_fact_double(order, "amount");
    double discount = amount * 0.1;  // 10% 折扣

    set_fact_double(order, "discount", discount);
    set_fact_double(order, "final_amount", amount - discount);

    printf("Applied 10%% discount: %.2f\n", discount);
}
