/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 95
 * Language: c
 * Block ID: 3b426f82
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 传统错误处理模式
int complex_operation(void) {
    int rc;

    rc = step_one();
    if (rc != 0) goto cleanup1;

    rc = step_two();
    if (rc != 0) goto cleanup2;

    rc = step_three();
    if (rc != 0) goto cleanup3;

    return 0;

cleanup3:
    undo_step_two();
cleanup2:
    undo_step_one();
cleanup1:
    return rc;
}
