/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\02_Register_VM.md
 * Line: 413
 * Language: c
 * Block ID: fb433bbd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 单态内联缓存 - 加速表字段访问 */
typedef struct {
    uint32_t type_tag;    /* 对象类型标记 */
    uint32_t slot_offset; /* 字段偏移 */
    void *getter;         /* 内联 getter */
} InlineCache;

/* 带缓存的表访问 */
RVValue* rv_gettable_cached(RVRegisterFile *rf, RVValue *t, RVValue *key,
                            InlineCache *ic) {
    if (rv_istable(t)) {
        RVTable *tbl = rv_totable(t);

        /* 快速路径：类型匹配 */
        if (tbl->type_tag == ic->type_tag && rv_isstr(key)) {
            /* 直接访问缓存位置 */
            return &tbl->slots[ic->slot_offset];
        }

        /* 慢速路径：通用查找并更新缓存 */
        int slot = rv_table_find_slot(tbl, key);
        if (slot >= 0) {
            ic->type_tag = tbl->type_tag;
            ic->slot_offset = slot;
            return &tbl->slots[slot];
        }
    }

    /* 触发元方法 */
    return rv_gettable_meta(rf, t, key);
}
