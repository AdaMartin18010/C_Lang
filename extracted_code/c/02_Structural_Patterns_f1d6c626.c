/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\02_Structural_Patterns.md
 * Line: 29
 * Language: c
 * Block ID: f1d6c626
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 目标接口 */
typedef struct {
    void (*request)(void*);
} target_t;

/* 被适配者 */
typedef struct {
    void (*specific_request)(void*);
} adaptee_t;

/* 适配器 */
typedef struct {
    target_t target;
    adaptee_t* adaptee;
} adapter_t;

void adapter_request(void* self) {
    adapter_t* adapter = self;
    /* 转换调用 */
    printf("Adapter: converting call\n");
    adapter->adaptee->specific_request(adapter->adaptee);
}

adapter_t* adapter_create(adaptee_t* adaptee) {
    adapter_t* adapter = malloc(sizeof(adapter_t));
    adapter->target.request = adapter_request;
    adapter->adaptee = adaptee;
    return adapter;
}

/* 使用 */
void example_adapter(void) {
    adaptee_t* legacy = adaptee_create();
    target_t* target = (target_t*)adapter_create(legacy);
    target->request(target);  /* 通过适配器调用 */
}
