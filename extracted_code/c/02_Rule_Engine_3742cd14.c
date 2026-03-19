/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\02_Rule_Engine.md
 * Line: 24
 * Language: c
 * Block ID: 3742cd14
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 基于条件-动作的规则引擎 */

typedef struct {
    char name[64];
    bool (*condition)(void* context);
    void (*action)(void* context);
    int priority;
    bool enabled;
} rule_t;

typedef struct {
    rule_t* rules;
    int count;
    int capacity;
} rule_engine_t;

rule_engine_t* rule_engine_create(void) {
    rule_engine_t* engine = malloc(sizeof(rule_engine_t));
    engine->capacity = 100;
    engine->rules = malloc(sizeof(rule_t) * engine->capacity);
    engine->count = 0;
    return engine;
}

void rule_add(rule_engine_t* engine, const char* name,
              bool (*cond)(void*), void (*action)(void*), int priority) {
    if (engine->count >= engine->capacity) return;

    rule_t* r = &engine->rules[engine->count++];
    strncpy(r->name, name, sizeof(r->name));
    r->condition = cond;
    r->action = action;
    r->priority = priority;
    r->enabled = true;

    /* 按优先级排序 */
    qsort(engine->rules, engine->count, sizeof(rule_t),
          (int(*)(const void*, const void*))
          lambda(int, (const rule_t* a, const rule_t* b) {
              return b->priority - a->priority;
          }));
}

void rule_engine_execute(rule_engine_t* engine, void* context) {
    for (int i = 0; i < engine->count; i++) {
        rule_t* r = &engine->rules[i];
        if (!r->enabled) continue;

        if (r->condition(context)) {
            printf("Rule fired: %s\n", r->name);
            r->action(context);

            /* 某些规则可能阻止后续规则 */
            /* if (r->stop_on_match) break; */
        }
    }
}

/* 示例: 电商促销规则 */
typedef struct {
    double cart_total;
    bool is_vip;
    int item_count;
    double discount;
} cart_context_t;

bool cond_vip_discount(void* ctx) {
    cart_context_t* c = ctx;
    return c->is_vip && c->cart_total > 100;
}

void action_vip_discount(void* ctx) {
    cart_context_t* c = ctx;
    c->discount += c->cart_total * 0.1;
    printf("VIP 10%% discount applied\n");
}

bool cond_bulk_discount(void* ctx) {
    cart_context_t* c = ctx;
    return c->item_count >= 10;
}

void action_bulk_discount(void* ctx) {
    cart_context_t* c = ctx;
    c->discount += 20;
    printf("Bulk purchase $20 off applied\n");
}

void example_rule_engine(void) {
    rule_engine_t* engine = rule_engine_create();

    rule_add(engine, "VIP Discount", cond_vip_discount,
             action_vip_discount, 100);
    rule_add(engine, "Bulk Discount", cond_bulk_discount,
             action_bulk_discount, 50);

    cart_context_t cart = {
        .cart_total = 200,
        .is_vip = true,
        .item_count = 5,
        .discount = 0
    };

    rule_engine_execute(engine, &cart);
    printf("Final discount: $%.2f\n", cart.discount);
}
