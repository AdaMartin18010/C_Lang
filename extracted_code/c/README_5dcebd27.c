/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\README.md
 * Line: 175
 * Language: c
 * Block ID: 5dcebd27
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 方案1: 返回错误码，输出参数
error_t parse_json(const char* input, json_t** out_result);

// 方案2: 使用上下文对象保存错误信息
context_t* ctx = context_create();
result_t* r = do_work(ctx, input);
if (!r) {
    const char* err = context_get_error(ctx);
    log_error("%s", err);
}
context_destroy(ctx);

// 方案3: 线程局部错误 (类似 errno)
result_t* r = do_work(input);
if (!r) {
    error_t err = get_last_error();
    // 处理错误
}
