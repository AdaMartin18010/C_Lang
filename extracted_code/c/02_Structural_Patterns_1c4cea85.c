/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\02_Structural_Patterns.md
 * Line: 70
 * Language: c
 * Block ID: 1c4cea85
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 将一种函数签名适配为另一种 */

typedef void (*callback_t)(int result, void* user_data);
typedef void (*legacy_callback_t)(int result);

typedef struct {
    legacy_callback_t legacy_fn;
    callback_t new_fn;
    void* user_data;
} callback_adapter_t;

void adapted_callback(int result) {
    /* 获取上下文 */
    callback_adapter_t* ctx = get_thread_local_context();
    ctx->new_fn(result, ctx->user_data);
}

/* 适配器函数 */
void call_with_adapter(legacy_callback_t legacy, callback_t modern,
                       void* user_data) {
    callback_adapter_t ctx = {
        .legacy_fn = legacy,
        .new_fn = modern,
        .user_data = user_data
    };
    set_thread_local_context(&ctx);
    legacy(adapted_callback);
}
