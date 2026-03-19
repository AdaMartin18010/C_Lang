/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\02_Register_VM.md
 * Line: 313
 * Language: c
 * Block ID: 2ca2a83b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 准备函数调用 */
bool rv_precall(RVRegisterFile *rf, RVValue *func, int nresults) {
    if (!rv_isfunction(func)) {
        /* 尝试元方法调用 */
        return rv_try_meta_call(rf, func, nresults);
    }

    RVClosure *cl = rv_toclosure(func);
    RVProto *p = cl->proto;

    /* 检查栈空间 */
    if (rf->top - rf->base + p->maxstack > RV_STACK_SIZE) {
        rv_raise_error(rf, RV_ERR_STACK);
        return false;
    }

    /* 初始化未提供的参数为nil */
    for (int i = p->numparams; i < p->is_vararg ? rf->top - func - 1 : p->numparams; i++) {
        rv_setnil(rf->base + i);
    }

    return true;
}

/* 尾调用优化 - 复用当前栈帧 */
bool rv_tailcall(RVRegisterFile *rf, RVValue *func, int nparams) {
    /* 将新参数移动到当前帧起始位置 */
    memmove(rf->ci->func, func, (nparams + 1) * sizeof(RVValue));

    /* 重置PC，执行新函数 */
    rf->pc = 0;
    RVClosure *cl = rv_toclosure(rf->ci->func);

    return rv_execute(rf, cl->proto->code, cl->proto->k);
}
