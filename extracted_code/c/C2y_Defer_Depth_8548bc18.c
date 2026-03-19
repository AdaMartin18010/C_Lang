/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2439
 * Language: c
 * Block ID: 8548bc18
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void critical_section_c2y(void) {
    pthread_mutex_lock(&mutex);
    defer pthread_mutex_unlock(&mutex);

    // 临界区代码...
}
