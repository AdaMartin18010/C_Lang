/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\16_MISRA_C_2023_Rules_16.md
 * Line: 246
 * Language: c
 * Block ID: 6edd1fdc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 带上下文的安全回调 */
typedef void (*Callback)(void *context, int result);

typedef struct {
    Callback callback;
    void *context;
    bool valid;
} CallbackInfo;

static CallbackInfo registered_callback = {NULL, NULL, false};

void register_callback(Callback cb, void *context)
{
    if (cb == NULL) {
        registered_callback.valid = false;
        return;
    }
    
    registered_callback.callback = cb;
    registered_callback.context = context;
    registered_callback.valid = true;
}

void trigger_callback(int result)
{
    if (registered_callback.valid) {
        registered_callback.callback(
            registered_callback.context, 
            result
        );
    }
}

void unregister_callback(void)
{
    registered_callback.valid = false;
    registered_callback.callback = NULL;
    registered_callback.context = NULL;
}
