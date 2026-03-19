/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\01_Linear_Logic_Theory.md
 * Line: 229
 * Language: c
 * Block ID: d39d92c7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 线性函数的C模拟
typedef struct {
    Resource *(*apply)(void *closure, Resource *arg);
    void *closure;
    bool arg_consumed;
    bool result_produced;
} LinearFunction;

// 应用线性函数
Resource *linear_apply(LinearFunction *f, Resource *arg) {
    if (f->arg_consumed) {
        // 错误：参数已被消耗
        return NULL;
    }
    f->arg_consumed = true;
    Resource *result = f->apply(f->closure, arg);
    f->result_produced = true;
    // arg现在无效（被消耗）
    return result;
}

// 示例：linear int -> linear int
// f(x) = x + 1
Resource *inc_impl(void *closure, Resource *arg) {
    int val = resource_get_int(arg);
    resource_destroy(arg);  // 消耗参数
    return resource_create_int(val + 1);
}
