/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 128
 * Language: c
 * Block ID: db12cecd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 多个资源获取时的复杂管理
void complex_operation(void) {
    Resource* r1 = acquire_resource1();
    if (r1 == NULL) goto cleanup_none;

    Resource* r2 = acquire_resource2();
    if (r2 == NULL) goto cleanup_r1;

    Resource* r3 = acquire_resource3();
    if (r3 == NULL) goto cleanup_r2;

    // ... 使用资源 ...

cleanup_r3:
    release_resource(r3);
cleanup_r2:
    release_resource(r2);
cleanup_r1:
    release_resource(r1);
cleanup_none:
    return;
}
