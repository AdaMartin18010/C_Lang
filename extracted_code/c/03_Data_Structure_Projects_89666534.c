/*
 * Auto-generated from: 12_Practice_Exercises\03_Data_Structure_Projects.md
 * Line: 61
 * Language: c
 * Block ID: 89666534
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void test_vector() {
    Vector *v = vec_create();

    // 测试扩容
    for (int i = 0; i < 1000; i++) {
        vec_push_back(v, i);
    }
    assert(vec_size(v) == 1000);
    assert(vec_capacity(v) >= 1000);

    // 测试访问
    assert(vec_get(v, 500) == 500);

    // 测试删除
    vec_erase(v, 500);
    assert(vec_size(v) == 999);

    vec_destroy(v);
}
