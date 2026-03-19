/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\02_Resource_Types.md
 * Line: 538
 * Language: c
 * Block ID: ba136c15
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：在迭代时修改集合
void bad_iteration(void) {
    Vector *v = vector_create();
    // ... 填充v

    for (size_t i = 0; i < vector_len(v); i++) {
        if (should_remove(vector_get(v, i))) {
            vector_remove(v, i);  // 迭代器失效！
        }
    }
}

// 正确：延迟删除或使用迭代器安全操作
void good_iteration(void) {
    Vector *v = vector_create();
    // ...

    size_t write_idx = 0;
    for (size_t read_idx = 0; read_idx < vector_len(v); read_idx++) {
        if (!should_remove(vector_get(v, read_idx))) {
            vector_set(v, write_idx++, vector_get(v, read_idx));
        }
    }
    vector_truncate(v, write_idx);
}
